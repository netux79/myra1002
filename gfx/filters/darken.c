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

// Compile: gcc -o darken.so -shared darken.c -std=c99 -O3 -Wall -pedantic -fPIC

// Useless filter, just nice as a reference for other filters.

#include "softfilter.h"
#include <stdlib.h>

#ifdef RARCH_INTERNAL
#define filter_data darken_filter_data
#endif

struct filter_data
{
   unsigned in_fmt;
};

static unsigned darken_input_fmts(void)
{
   return SOFTFILTER_FMT_XRGB8888 | SOFTFILTER_FMT_RGB565;
}

static unsigned darken_output_fmts(unsigned input_fmts)
{
   return input_fmts;
}

static void *darken_create(unsigned in_fmt)
{
   struct filter_data *filt = (struct filter_data*)calloc(1, sizeof(*filt));
   if (!filt)
      return NULL;

   filt->in_fmt  = in_fmt;

   return filt;
}

static void darken_output(void *data, unsigned *out_width, unsigned *out_height,
      unsigned width, unsigned height)
{
   *out_width = width;
   *out_height = height;
}

static void darken_destroy(void *data)
{
   struct filter_data *filt = (struct filter_data*)data;
   free(filt);
}

static void darken_work_xrgb8888(unsigned width, unsigned height, uint32_t *input, int pitch, 
      uint32_t *output, int outpitch)
{
   unsigned x, y;
   
   for (y = 0; y < height; y++, input += pitch >> 2, output += outpitch >> 2)
      for (x = 0; x < width; x++)
         output[x] = (input[x] >> 2) & (0x3f * 0x01010101);
}

static void darken_work_rgb565(unsigned width, unsigned height, uint16_t *input, int pitch, 
      uint16_t *output, int outpitch)
{
   unsigned x, y;

   for (y = 0; y < height; y++, input += pitch >> 1, output += outpitch >> 1)
      for (x = 0; x < width; x++)
         output[x] = (input[x] >> 2) & ((0x7 << 0) | (0xf << 5) | (0x7 << 11));
}

static void darken_render(void *data,
      void *output, size_t output_stride,
      const void *input, unsigned width, unsigned height, size_t input_stride)
{
   struct filter_data *filt = (struct filter_data*)data;

   if (filt->in_fmt == SOFTFILTER_FMT_XRGB8888)
      darken_work_xrgb8888(width, height, (uint32_t *)input, input_stride, (uint32_t *)output, output_stride);
   else if (filt->in_fmt == SOFTFILTER_FMT_RGB565)
      darken_work_rgb565(width, height, (uint16_t *)input, input_stride, (uint16_t *)output, output_stride);
}

const softfilter_implementation_t darken_implementation = {
   darken_input_fmts,
   darken_output_fmts,

   darken_create,
   darken_destroy,

   darken_output,
   darken_render,
   "Darken",
};

#ifdef RARCH_INTERNAL
#undef filter_data
#endif
