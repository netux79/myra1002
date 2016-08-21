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

// Compile: gcc -o scale2x.so -shared scale2x.c -std=c99 -O3 -Wall -pedantic -fPIC

#include "softfilter.h"
#include <stdlib.h>

#ifdef RARCH_INTERNAL
#define filter_data scale2x_filter_data
#endif

#define SCALE2X_SCALE 2

struct filter_data
{
   unsigned in_fmt;
};

#define SCALE2X_GENERIC(typename_t, width, height, src, src_stride, dst, dst_stride, out0, out1) \
   for (y = 0; y < height; ++y) \
   { \
      const int prevline = (y == 0) ? 0 : src_stride; \
      const int nextline = (y == height - 1) ? 0 : src_stride; \
      \
      for (x = 0; x < width; ++x) \
      { \
         const typename_t A = *(src - prevline); \
         const typename_t B = (x > 0) ? *(src - 1) : *src; \
         const typename_t C = *src; \
         const typename_t D = (x < width - 1) ? *(src + 1) : *src; \
         const typename_t E = *(src++ + nextline); \
         \
         if (A != E && B != D) \
         { \
            *out0++ = (A == B ? A : C); \
            *out0++ = (A == D ? A : C); \
            *out1++ = (E == B ? E : C); \
            *out1++ = (E == D ? E : C); \
         } \
         else \
         { \
            *out0++ = C; \
            *out0++ = C; \
            *out1++ = C; \
            *out1++ = C; \
         } \
      } \
      \
      src += src_stride - width; \
      out0 += dst_stride + dst_stride - (width * SCALE2X_SCALE); \
      out1 += dst_stride + dst_stride - (width * SCALE2X_SCALE); \
   }

static void scale2x_generic_rgb565(unsigned width, unsigned height,
      const uint16_t *src, unsigned src_stride,
      uint16_t *dst, unsigned dst_stride)
{
   unsigned x, y;
   uint16_t *out0, *out1;
   out0 = (uint16_t*)dst;
   out1 = (uint16_t*)(dst + dst_stride);
   SCALE2X_GENERIC(uint16_t, width, height, src, src_stride, dst, dst_stride, out0, out1);
}

static void scale2x_generic_xrgb8888(unsigned width, unsigned height,
      const uint32_t *src, unsigned src_stride,
      uint32_t *dst, unsigned dst_stride)
{
   unsigned x, y;
   uint32_t *out0, *out1;
   out0 = (uint32_t*)dst;
   out1 = (uint32_t*)(dst + dst_stride);
   SCALE2X_GENERIC(uint32_t, width, height, src, src_stride, dst, dst_stride, out0, out1);
}

static unsigned scale2x_generic_input_fmts(void)
{
   return SOFTFILTER_FMT_XRGB8888 | SOFTFILTER_FMT_RGB565;
}

static unsigned scale2x_generic_output_fmts(unsigned input_fmts)
{
   return input_fmts;
}

static void *scale2x_generic_create(unsigned in_fmt)
{
   struct filter_data *filt = (struct filter_data*)calloc(1, sizeof(*filt));
   if (!filt)
      return NULL;

   filt->in_fmt  = in_fmt;

   return filt;
}

static void scale2x_generic_output(void *data, unsigned *out_width, unsigned *out_height,
      unsigned width, unsigned height)
{
   *out_width = width * SCALE2X_SCALE;
   *out_height = height * SCALE2X_SCALE;
}

static void scale2x_generic_destroy(void *data)
{
   struct filter_data *filt = (struct filter_data*)data;
   free(filt);
}

static void scale2x_generic_render(void *data,
      void *output, size_t output_stride,
      const void *input, unsigned width, unsigned height, size_t input_stride)
{
   struct filter_data *filt = (struct filter_data*)data;

   if (filt->in_fmt == SOFTFILTER_FMT_XRGB8888)
      scale2x_generic_xrgb8888(width, height,
         (uint32_t*)input, input_stride / SOFTFILTER_BPP_XRGB8888, 
         (uint32_t*)output, output_stride / SOFTFILTER_BPP_XRGB8888);
   else if (filt->in_fmt == SOFTFILTER_FMT_RGB565)
      scale2x_generic_rgb565(width, height,
         (uint16_t*)input, input_stride / SOFTFILTER_BPP_RGB565, 
         (uint16_t*)output, output_stride / SOFTFILTER_BPP_RGB565);
}

const softfilter_implementation_t scale2x_implementation = {
   scale2x_generic_input_fmts,
   scale2x_generic_output_fmts,

   scale2x_generic_create,
   scale2x_generic_destroy,

   scale2x_generic_output,
   scale2x_generic_render,
   "Scale2x",
};

#ifdef RARCH_INTERNAL
#undef filter_data
#endif
