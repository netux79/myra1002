/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2014 - Hans-Kristian Arntzen
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

#include <stdbool.h>
#include "utils.h"

#include "../general.h"
#include "../performance.h"

void audio_convert_s16_to_float_C(float *out,
      const int16_t *in, size_t samples, float gain)
{
   size_t i;
   gain = gain / 0x8000;
   for (i = 0; i < samples; i++)
      out[i] = (float)in[i] * gain; 
}

void audio_convert_float_to_s16_C(int16_t *out,
      const float *in, size_t samples)
{
   size_t i;
   for (i = 0; i < samples; i++)
   {
      int32_t val = (int32_t)(in[i] * 0x8000);
      out[i] = (val > 0x7FFF) ? 0x7FFF : (val < -0x8000 ? -0x8000 : (int16_t)val);
   }
}

