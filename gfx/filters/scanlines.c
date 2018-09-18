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

// Compile: gcc -o scanlines.so -shared scanlines.c -std=c99 -O3 -Wall -pedantic -fPIC

// Useless filter, just nice as a reference for other filters.

#include "softfilter.h"
#include <stdlib.h>

#ifdef RARCH_INTERNAL
#define filter_data scanlines_filter_data
#endif

struct filter_data
{
   unsigned in_fmt;
};

static unsigned scanlines_input_fmts(void)
{
   return SOFTFILTER_FMT_XRGB8888 | SOFTFILTER_FMT_RGB565;
}

static unsigned scanlines_output_fmts(unsigned input_fmts)
{
   return input_fmts;
}

static void *scanlines_create(unsigned in_fmt)
{
   struct filter_data *filt = (struct filter_data*)calloc(1, sizeof(*filt));
   if (!filt)
      return NULL;

   filt->in_fmt  = in_fmt;

   return filt;
}

static void scanlines_output(void *data, unsigned *out_width, unsigned *out_height,
      unsigned width, unsigned height)
{
   *out_width = width;
   *out_height = height * 2;
}

static void scanlines_destroy(void *data)
{
   struct filter_data *filt = (struct filter_data*)data;
   free(filt);
}

static void scanlines_work_xrgb8888(unsigned width, unsigned height, uint32_t *input, int pitch, 
      uint32_t *output, int outpitch)
{
   pitch >>= 2;
   outpitch >>= 2;
   width *= sizeof(uint32_t);

   while (height--)
   {
      memcpy(output, input, width);
      output += outpitch;
      memset(output, 0, width);
      output += outpitch;
      input += pitch;
   }
}

static void scanlines_work_rgb565(unsigned width, unsigned height, uint16_t *input, int pitch, 
      uint16_t *output, int outpitch)
{
   pitch >>= 1;
   outpitch >>= 1;
   width *= sizeof(uint16_t);

   while (height--)
   {
      memcpy(output, input, width);
      output += outpitch;
      memset(output, 0, width);
      output += outpitch;
      input += pitch;
   }
}

static void scanlines_render(void *data,
      void *output, size_t output_stride,
      const void *input, unsigned width, unsigned height, size_t input_stride)
{
   struct filter_data *filt = (struct filter_data*)data;

   if (filt->in_fmt == SOFTFILTER_FMT_RGB565)
      scanlines_work_rgb565(width, height, 
         (uint16_t *)input, input_stride, 
         (uint16_t *)output, output_stride);   
   else if (filt->in_fmt == SOFTFILTER_FMT_XRGB8888)
      scanlines_work_xrgb8888(width, height, 
         (uint32_t *)input, input_stride, 
         (uint32_t *)output, output_stride);
}

const softfilter_implementation_t scanlines_implementation = {
   scanlines_input_fmts,
   scanlines_output_fmts,

   scanlines_create,
   scanlines_destroy,

   scanlines_output,
   NULL,
   
   scanlines_render,
   "Scanlines",
};

#ifdef RARCH_INTERNAL
#undef filter_data
#endif
