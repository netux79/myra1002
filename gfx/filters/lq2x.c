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

// Compile: gcc -o lq2x.so -shared lq2x.c -std=c99 -O3 -Wall -pedantic -fPIC

#include "softfilter.h"
#include <stdlib.h>

#ifdef RARCH_INTERNAL
#define softfilter_get_implementation lq2x_get_implementation
#define filter_data lq2x_filter_data
#endif

#define LQ2X_SCALE 2

struct filter_data
{
   unsigned in_fmt;
};

static unsigned lq2x_generic_input_fmts(void)
{
   return SOFTFILTER_FMT_RGB565 | SOFTFILTER_FMT_XRGB8888;
}

static unsigned lq2x_generic_output_fmts(unsigned input_fmts)
{
   return input_fmts;
}

static void *lq2x_generic_create(unsigned in_fmt)
{
   struct filter_data *filt = (struct filter_data*)calloc(1, sizeof(*filt));
   if (!filt)
      return NULL;

   filt->in_fmt  = in_fmt;

   return filt;
}

static void lq2x_generic_output(void *data, unsigned *out_width, unsigned *out_height,
      unsigned width, unsigned height)
{
   *out_width = width * LQ2X_SCALE;
   *out_height = height * LQ2X_SCALE;
}

static void lq2x_generic_destroy(void *data)
{
   struct filter_data *filt = (struct filter_data*)data;
   free(filt);
}

static void lq2x_generic_rgb565(unsigned width, unsigned height,
      uint16_t *src, unsigned src_stride, uint16_t *dst, unsigned dst_stride)
{
   unsigned x, y;
   uint16_t *out0, *out1;
   out0 = (uint16_t*)dst;
   out1 = (uint16_t*)(dst + dst_stride);

   for(y = 0; y < height; y++)
   {
      int prevline, nextline;
      prevline = (y == 0 ? 0 : src_stride);
      nextline = (y == height - 1) ? 0 : src_stride;

      for(x = 0; x < width; x++)
      {
         uint16_t A, B, C, D, E, c;
         A = *(src - prevline);
         B = (x > 0) ? *(src - 1) : *src;
         C = *src;
         D = (x < width - 1) ? *(src + 1) : *src;
         E = *(src++ + nextline);
         c = C;

         if(A != E && B != D)
         {
            *out0++ = (A == B ? ((C + A - ((C ^ A) & 0x0821)) >> 1) : c);
            *out0++ = (A == D ? ((C + A - ((C ^ A) & 0x0821)) >> 1) : c);
            *out1++ = (E == B ? ((C + E - ((C ^ E) & 0x0821)) >> 1) : c);
            *out1++ = (E == D ? ((C + E - ((C ^ E) & 0x0821)) >> 1) : c);
         }
         else
         {
            *out0++ = c;
            *out0++ = c;
            *out1++ = c;
            *out1++ = c;
         }
      }

      src += src_stride - width;
      out0 += dst_stride + dst_stride - (width << 1);
      out1 += dst_stride + dst_stride - (width << 1);
   }
}

static void lq2x_generic_xrgb8888(unsigned width, unsigned height,
      uint32_t *src, unsigned src_stride, uint32_t *dst, unsigned dst_stride)
{
   unsigned x, y;
   uint32_t *out0, *out1;
   out0 = (uint32_t*)dst;
   out1 = (uint32_t*)(dst + dst_stride);

   for(y = 0; y < height; y++)
   {
      int prevline = (y == 0 ? 0 : src_stride);
      int nextline = (y == height - 1) ? 0 : src_stride;

      for(x = 0; x < width; x++)
      {
         uint32_t A = *(src - prevline);
         uint32_t B = (x > 0) ? *(src - 1) : *src;
         uint32_t C = *src;
         uint32_t D = (x < width - 1) ? *(src + 1) : *src;
         uint32_t E = *(src++ + nextline);
         uint32_t c = C;

         if(A != E && B != D)
         {
            *out0++ = (A == B ? (C + A - ((C ^ A) & 0x0421)) >> 1 : c);
            *out0++ = (A == D ? (C + A - ((C ^ A) & 0x0421)) >> 1 : c);
            *out1++ = (E == B ? (C + E - ((C ^ E) & 0x0421)) >> 1 : c);
            *out1++ = (E == D ? (C + E - ((C ^ E) & 0x0421)) >> 1 : c);
         }
         else
         {
            *out0++ = c;
            *out0++ = c;
            *out1++ = c;
            *out1++ = c;
         }
      }

      src += src_stride - width;
      out0 += dst_stride + dst_stride - (width << 1);
      out1 += dst_stride + dst_stride - (width << 1);
   }
}

static void lq2x_generic_render(void *data,
      void *output, size_t output_stride,
      const void *input, unsigned width, unsigned height, size_t input_stride)
{
   struct filter_data *filt = (struct filter_data*)data;

   if (filt->in_fmt == SOFTFILTER_FMT_RGB565)
      lq2x_generic_rgb565(width, height,
         (uint16_t*)input, input_stride / SOFTFILTER_BPP_RGB565, 
         (uint16_t*)output, output_stride / SOFTFILTER_BPP_RGB565);
   else if (filt->in_fmt == SOFTFILTER_FMT_XRGB8888)
      lq2x_generic_xrgb8888(width, height,
         (uint32_t*)input, input_stride / SOFTFILTER_BPP_XRGB8888, 
         (uint32_t*)output, output_stride / SOFTFILTER_BPP_XRGB8888);
}

static const struct softfilter_implementation lq2x_generic = {
   lq2x_generic_input_fmts,
   lq2x_generic_output_fmts,

   lq2x_generic_create,
   lq2x_generic_destroy,

   lq2x_generic_output,
   lq2x_generic_render,
   "LQ2x",
};

const struct softfilter_implementation *softfilter_get_implementation(void)
{
   return &lq2x_generic;
}

#ifdef RARCH_INTERNAL
#undef softfilter_get_implementation
#undef filter_data
#endif
