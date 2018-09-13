/****************************************************************************
 *  vi_encoder.c
 *
 *  Wii Audio/Video Encoder support
 *
 *  Copyright (C) 2009 Eke-Eke, with some code from libogc (C) Hector Martin
 *
 *  Redistribution and use of this code or any derivative works are permitted
 *  provided that the following conditions are met:
 *
 *   - Redistributions may not be sold, nor may they be used in a commercial
 *     product or activity.
 *
 *   - Redistributions that are modified from the original source must include the
 *     complete source code, including the source code for all components used by a
 *     binary built from the modified sources. However, as a special exception, the
 *     source code distributed need not include anything that is normally distributed
 *     (in either source or binary form) with the major components (compiler, kernel,
 *     and so on) of the operating system on which the executable runs, unless that
 *     component itself accompanies the executable.
 *
 *   - Redistributions must reproduce the above copyright notice, this list of
 *     conditions and the following disclaimer in the documentation and/or other
 *     materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************************/

#include <string.h>
#include <gccore.h>
#include <ogcsys.h>
#include <ogc/machine/processor.h>

#include "vi_encoder.h"

/****************************************************************************
 *  I2C driver by Hector Martin (marcan)
 *
 ****************************************************************************/

#define _SHIFTL(v, s, w)	\
    ((uint32_t) (((uint32_t)(v) & ((0x01 << (w)) - 1)) << (s)))
#define _SHIFTR(v, s, w)	\
    ((uint32_t)(((uint32_t)(v) >> (s)) & ((0x01 << (w)) - 1)))

extern void udelay(int us);

static uint32_t i2cIdentFirst = 0;
static uint32_t i2cIdentFlag = 1;
static volatile uint32_t* const _i2cReg = (uint32_t*)0xCD800000;

static inline void __viOpenI2C(uint32_t channel)
{
	uint32_t val = ((_i2cReg[49]&~0x8000)|0x4000);
	val |= _SHIFTL(channel,15,1);
	_i2cReg[49] = val;
}

static inline void __viSetSCL(uint32_t channel)
{
	uint32_t val = (_i2cReg[48]&~0x4000);
	val |= _SHIFTL(channel,14,1);
	_i2cReg[48] = val;
}
static inline void __viSetSDA(uint32_t channel)
{
	uint32_t val = (_i2cReg[48]&~0x8000);
	val |= _SHIFTL(channel,15,1);
	_i2cReg[48] = val;
}

#define __viGetSDA() (_SHIFTR(_i2cReg[50],15,1))

static uint32_t __sendSlaveAddress(uint8_t addr)
{
	uint32_t i;

	__viSetSDA(i2cIdentFlag^1);
	udelay(2);

	__viSetSCL(0);
	for(i=0;i<8;i++) {
		if(addr&0x80) __viSetSDA(i2cIdentFlag);
		else __viSetSDA(i2cIdentFlag^1);
		udelay(2);

		__viSetSCL(1);
		udelay(2);

		__viSetSCL(0);
		addr <<= 1;
	}

	__viOpenI2C(0);
	udelay(2);

	__viSetSCL(1);
	udelay(2);

	if(i2cIdentFlag==1 && __viGetSDA()!=0) return 0;

	__viSetSDA(i2cIdentFlag^1);
	__viOpenI2C(1);
	__viSetSCL(0);

	return 1;
}

void VIDEO_SetTrapFilter(bool enable)
{
  uint8_t disable;
  uint8_t data;
  uint8_t reg;
  uint8_t buf[2];

  disable = !enable;
  data = !disable;
  reg = 0x03;

  buf[0] = reg;
  buf[1] = data;

  uint8_t addr = 0xe0;
  void* val = buf;
  uint32_t len = 2;

  uint8_t c;
  int32_t i,j;
  uint32_t level,ret;

  if(i2cIdentFirst==0)
  {
	  __viOpenI2C(0);
	  udelay(4);

	  i2cIdentFlag = 0;
	  if(__viGetSDA()!=0)
		  i2cIdentFlag = 1;
	  i2cIdentFirst = 1;
  }

  _CPU_ISR_Disable(level);
  (void)level;

  __viOpenI2C(1);
  __viSetSCL(1);

  __viSetSDA(i2cIdentFlag);
  udelay(4);

  ret = __sendSlaveAddress(addr);
  if(ret==0) {
	  _CPU_ISR_Restore(level);
	  goto end;
  }

  __viOpenI2C(1);
  for(i=0;i<len;i++) {
	  c = ((uint8_t*)val)[i];
	  for(j=0;j<8;j++) {
                  uint32_t chan = i2cIdentFlag;
		  if(c&0x80) {}
                  else
                    chan ^= 1;

		  __viSetSDA(chan);
		  udelay(2);

		  __viSetSCL(1);
		  udelay(2);
		  __viSetSCL(0);

		  c <<= 1;
	  }
	  __viOpenI2C(0);
	  udelay(2);
	  __viSetSCL(1);
	  udelay(2);

	  if(i2cIdentFlag==1 && __viGetSDA()!=0) {
		  _CPU_ISR_Restore(level);
		  goto end;
	  }

	  __viSetSDA(i2cIdentFlag^1);
	  __viOpenI2C(1);
	  __viSetSCL(0);
  }

  __viOpenI2C(1);
  __viSetSDA(i2cIdentFlag^1);
  udelay(2);
  __viSetSDA(i2cIdentFlag);

  _CPU_ISR_Restore(level);

end:

  udelay(2);
}
