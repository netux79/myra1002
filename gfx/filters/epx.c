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

// Compile: gcc -o epx.so -shared epx.c -std=c99 -O3 -Wall -pedantic -fPIC

#include "softfilter.h"
#include <stdlib.h>

#ifdef RARCH_INTERNAL
#define softfilter_get_implementation epx_get_implementation
#define filter_data epx_filter_data
#endif

#define EPX_SCALE 2

struct filter_data
{
   unsigned in_fmt;
};

static unsigned epx_generic_input_fmts(void)
{
   return SOFTFILTER_FMT_RGB565;
}

static unsigned epx_generic_output_fmts(unsigned input_fmts)
{
   return input_fmts;
}

static void *epx_generic_create(unsigned in_fmt)
{
   struct filter_data *filt = (struct filter_data*)calloc(1, sizeof(*filt));
   if (!filt)
      return NULL;

   filt->in_fmt  = in_fmt;
   
   return filt;
}

static void epx_generic_output(void *data, unsigned *out_width, unsigned *out_height,
      unsigned width, unsigned height)
{
   *out_width = width * EPX_SCALE;
   *out_height = height * EPX_SCALE;
}

static void epx_generic_destroy(void *data)
{
   struct filter_data *filt = (struct filter_data*)data;
   free(filt);
}

static void epx_generic_rgb565 (int width, int height,
      uint16_t *src, int src_stride, uint16_t *dst, int dst_stride)
{
   uint16_t colorX, colorA, colorB, colorC, colorD;
   uint16_t *sP, *uP, *lP;
   uint32_t*dP1, *dP2;
   int w;

   for (; height; height--)
   {
      sP  = (uint16_t *) src;
      uP  = (uint16_t *) (src - src_stride);
      lP  = (uint16_t *) (src + src_stride);
      dP1 = (uint32_t *) dst;
      dP2 = (uint32_t *) (dst + dst_stride);

      // left edge

      colorX = *sP;
      colorC = *++sP;
      colorB = *lP++;
      colorD = *uP++;

      if ((colorX != colorC) && (colorB != colorD))
      {
         #ifdef MSB_FIRST
         *dP1 = (colorX << 16) + ((colorC == colorD) ? colorC : colorX);
         *dP2 = (colorX << 16) + ((colorB == colorC) ? colorB : colorX);
         #else
         *dP1 = colorX + (((colorC == colorD) ? colorC : colorX) << 16);
         *dP2 = colorX + (((colorB == colorC) ? colorB : colorX) << 16);
         #endif
      }
      else
         *dP1 = *dP2 = (colorX << 16) + colorX;

      dP1++;
      dP2++;

      //

      for (w = width - 2; w; w--)
      {
         colorA = colorX;
         colorX = colorC;
         colorC = *++sP;
         colorB = *lP++;
         colorD = *uP++;

         if ((colorA != colorC) && (colorB != colorD))
         {
#ifdef MSB_FIRST
           *dP1 = (((colorD == colorA) ? colorD : colorX) << 16) + ((colorC == colorD) ? colorC : colorX);
           *dP2 = (((colorA == colorB) ? colorA : colorX) << 16) + ((colorB == colorC) ? colorB : colorX);
#else
           *dP1 = ((colorD == colorA) ? colorD : colorX) + (((colorC == colorD) ? colorC : colorX) << 16);
           *dP2 = ((colorA == colorB) ? colorA : colorX) + (((colorB == colorC) ? colorB : colorX) << 16);
#endif
         }
         else
            *dP1 = *dP2 = (colorX << 16) + colorX;

         dP1++;
         dP2++;
      }

      // right edge

      colorA = colorX;
      colorX = colorC;
      colorB = *lP;
      colorD = *uP;

      if ((colorA != colorX) && (colorB != colorD))
      {
#ifdef MSB_FIRST
         *dP1 = (((colorD == colorA) ? colorD : colorX) << 16) + colorX;
         *dP2 = (((colorA == colorB) ? colorA : colorX) << 16) + colorX;
#else
         *dP1 = ((colorD == colorA) ? colorD : colorX) + (colorX << 16);
         *dP2 = ((colorA == colorB) ? colorA : colorX) + (colorX << 16);
#endif
      }
      else
         *dP1 = *dP2 = (colorX << 16) + colorX;

      src += src_stride;
      dst += dst_stride << 1;
   }
}

#define A1565(el0, el1) (((el0) & (el1)) + ((((el0) ^ (el1)) & 0xF7DE) >> 1))

static void epxsmooth_generic_rgb565 (int width, int height,
      uint16_t *src, int src_stride, uint16_t *dst, int dst_stride)
{
   uint16_t colorX, colorA, colorB, colorC, colorD;
   uint16_t *sP, *uP, *lP;
   uint32_t*dP1, *dP2;
   int w;

   for (; height; height--)
   {
      sP  = (uint16_t *) src;
      uP  = (uint16_t *) (src - src_stride);
      lP  = (uint16_t *) (src + src_stride);
      dP1 = (uint32_t *) dst;
      dP2 = (uint32_t *) (dst + dst_stride);

      // left edge

      colorX = *sP;
      colorC = *++sP;
      colorB = *lP++;
      colorD = *uP++;

      if ((colorX != colorC) && (colorB != colorD))
      {
         #ifdef MSB_FIRST
         *dP1 = (colorX << 16) + ((colorC == colorD) ? A1565(colorC, colorX) : colorX);
         *dP2 = (colorX << 16) + ((colorB == colorC) ? A1565(colorB, colorX) : colorX);
         #else
         *dP1 = colorX + (((colorC == colorD) ? A1565(colorC, colorX) : colorX) << 16);
         *dP2 = colorX + (((colorB == colorC) ? A1565(colorB, colorX) : colorX) << 16);
         #endif
      }
      else
         *dP1 = *dP2 = (colorX << 16) + colorX;

      dP1++;
      dP2++;

      //

      for (w = width - 2; w; w--)
      {
         colorA = colorX;
         colorX = colorC;
         colorC = *++sP;
         colorB = *lP++;
         colorD = *uP++;

         if ((colorA != colorC) && (colorB != colorD))
         {
#ifdef MSB_FIRST
           *dP1 = (((colorD == colorA) ? A1565(colorD, colorX) : colorX) << 16) + ((colorC == colorD) ? A1565(colorC, colorX) : colorX);
           *dP2 = (((colorA == colorB) ? A1565(colorA, colorX) : colorX) << 16) + ((colorB == colorC) ? A1565(colorB, colorX) : colorX);
#else
           *dP1 = ((colorD == colorA) ? A1565(colorD, colorX) : colorX) + (((colorC == colorD) ? A1565(colorC, colorX) : colorX) << 16);
           *dP2 = ((colorA == colorB) ? A1565(colorA, colorX) : colorX) + (((colorB == colorC) ? A1565(colorB, colorX) : colorX) << 16);
#endif
         }
         else
            *dP1 = *dP2 = (colorX << 16) + colorX;

         dP1++;
         dP2++;
      }

      // right edge

      colorA = colorX;
      colorX = colorC;
      colorB = *lP;
      colorD = *uP;

      if ((colorA != colorX) && (colorB != colorD))
      {
#ifdef MSB_FIRST
         *dP1 = (((colorD == colorA) ? A1565(colorD, colorX) : colorX) << 16) + colorX;
         *dP2 = (((colorA == colorB) ? A1565(colorA, colorX) : colorX) << 16) + colorX;
#else
         *dP1 = ((colorD == colorA) ? A1565(colorD, colorX) : colorX) + (colorX << 16);
         *dP2 = ((colorA == colorB) ? A1565(colorA, colorX) : colorX) + (colorX << 16);
#endif
      }
      else
         *dP1 = *dP2 = (colorX << 16) + colorX;

      src += src_stride;
      dst += dst_stride << 1;
   }
}

static void epx_generic_render(void *data,
      void *output, size_t output_stride,
      const void *input, unsigned width, unsigned height, size_t input_stride)
{
   struct filter_data *filt = (struct filter_data*)data;

   if (filt->in_fmt == SOFTFILTER_FMT_RGB565)
      epx_generic_rgb565(width, height,
         (uint16_t*)input, input_stride / SOFTFILTER_BPP_RGB565, 
         (uint16_t*)output, output_stride / SOFTFILTER_BPP_RGB565);
}

static void epxsmooth_generic_render(void *data,
      void *output, size_t output_stride,
      const void *input, unsigned width, unsigned height, size_t input_stride)
{
   struct filter_data *filt = (struct filter_data*)data;

   if (filt->in_fmt == SOFTFILTER_FMT_RGB565)
      epxsmooth_generic_rgb565(width, height,
         (uint16_t*)input, input_stride / SOFTFILTER_BPP_RGB565, 
         (uint16_t*)output, output_stride / SOFTFILTER_BPP_RGB565);
}

static const struct softfilter_implementation epx_generic = {
   epx_generic_input_fmts,
   epx_generic_output_fmts,

   epx_generic_create,
   epx_generic_destroy,

   epx_generic_output,
   epx_generic_render,
   "EPX",
};

static const struct softfilter_implementation epxsmooth_generic = {
   epx_generic_input_fmts,
   epx_generic_output_fmts,

   epx_generic_create,
   epx_generic_destroy,

   epx_generic_output,
   epxsmooth_generic_render,
   "EPX Smooth",
};

const struct softfilter_implementation *softfilter_get_implementation(void)
{
   return &epx_generic;
}

const struct softfilter_implementation *epxsmooth_get_implementation(void)
{
   return &epxsmooth_generic;
}

#ifdef RARCH_INTERNAL
#undef softfilter_get_implementation
#undef filter_data
#endif
