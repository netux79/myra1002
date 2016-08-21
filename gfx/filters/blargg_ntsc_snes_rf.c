/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2014 - Hans-Kristian Arntzen
 *  Copyright (C) 2011-2014 - Daniel De Matteis
 *
 *  RetroArch is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with RetroArch.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include "softfilter.h"
#include <stdlib.h>
#include "snes_ntsc/snes_ntsc.h"

#ifdef RARCH_INTERNAL
#define filter_data blargg_ntsc_snes_rf_filter_data
#else
#include "snes_ntsc/snes_ntsc.c"
#endif

struct filter_data
{
   unsigned in_fmt;
   struct snes_ntsc_t *ntsc;
   int burst;
   int burst_toggle;
};

static unsigned blargg_ntsc_snes_rf_generic_input_fmts(void)
{
   return SOFTFILTER_FMT_RGB565;
}

static unsigned blargg_ntsc_snes_rf_generic_output_fmts(unsigned input_fmts)
{
   return input_fmts;
}

static void blargg_ntsc_snes_rf_initialize(void *data)
{
   snes_ntsc_setup_t setup;
   struct filter_data *filt = (struct filter_data*)data;

   filt->ntsc = (snes_ntsc_t*)calloc(1, sizeof(*filt->ntsc));
   setup = snes_ntsc_composite;
   setup.merge_fields = 0;
   snes_ntsc_init(filt->ntsc, &setup);

   filt->burst = 0;
   filt->burst_toggle = (setup.merge_fields ? 0 : 1);
}

static void *blargg_ntsc_snes_rf_generic_create(unsigned in_fmt)
{
   struct filter_data *filt = (struct filter_data*)calloc(1, sizeof(*filt));
   if (!filt)
      return NULL;

   filt->in_fmt  = in_fmt;

   blargg_ntsc_snes_rf_initialize(filt);

   return filt;
}

static void blargg_ntsc_snes_rf_generic_output(void *data, unsigned *out_width, unsigned *out_height,
      unsigned width, unsigned height)
{
   *out_width  = (width > 256) ? SNES_NTSC_OUT_WIDTH(width / 2) : SNES_NTSC_OUT_WIDTH(width);
   *out_height = height;
}

static void blargg_ntsc_snes_rf_generic_destroy(void *data)
{
   struct filter_data *filt = (struct filter_data*)data;

   if(filt->ntsc)
      free(filt->ntsc);

   free(filt);
}

static void blargg_ntsc_snes_rf_render_rgb565(void *data, int width, int height,
      uint16_t *input, int pitch, uint16_t *output, int outpitch)
{
   struct filter_data *filt = (struct filter_data*)data;

   if(width > 256)
      snes_ntsc_blit_hires(filt->ntsc, input, pitch, filt->burst, width, height, output, outpitch * 2);
   else
      snes_ntsc_blit(filt->ntsc, input, pitch, filt->burst, width, height, output, outpitch * 2);

   filt->burst ^= filt->burst_toggle;
}

static void blargg_ntsc_snes_rf_generic_render(void *data,
      void *output, size_t output_stride,
      const void *input, unsigned width, unsigned height, size_t input_stride)
{
   struct filter_data *filt = (struct filter_data*)data;

   if (filt->in_fmt == SOFTFILTER_FMT_RGB565)
      blargg_ntsc_snes_rf_render_rgb565(data, width, height,
         (uint16_t*)input, input_stride / SOFTFILTER_BPP_RGB565, 
         (uint16_t*)output, output_stride / SOFTFILTER_BPP_RGB565);
}

const softfilter_implementation_t blargg_ntsc_snes_rf_implementation = {
   blargg_ntsc_snes_rf_generic_input_fmts,
   blargg_ntsc_snes_rf_generic_output_fmts,

   blargg_ntsc_snes_rf_generic_create,
   blargg_ntsc_snes_rf_generic_destroy,

   blargg_ntsc_snes_rf_generic_output,
   blargg_ntsc_snes_rf_generic_render,
   "Blargg NTSC RF",
};

#ifdef RARCH_INTERNAL
#undef filter_data
#endif
