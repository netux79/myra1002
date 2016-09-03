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
#define filter_data blargg_ntsc_filter_data
#else
#include "snes_ntsc/snes_ntsc.c"
#endif

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MAX_LOWRES_WIDTH 340

struct filter_data
{
   unsigned in_fmt;
   struct snes_ntsc_t *ntsc;
   int burst;
   int burst_toggle;
};

static unsigned blargg_ntsc_generic_input_fmts(void)
{
   return SOFTFILTER_FMT_RGB565;
}

static unsigned blargg_ntsc_generic_output_fmts(unsigned input_fmts)
{
   return input_fmts;
}

enum {
   BLARGG_RF,
   BLARGG_COMPOSITE,
   BLARGG_RGB,
   BLARGG_SVIDEO,
   BLARGG_MONOCHROME,
};

static void blargg_ntsc_initialize(void *data, unsigned char ntsc_type)
{
   snes_ntsc_setup_t setup;
   struct filter_data *filt = (struct filter_data*)data;
   
   /* By default we are merging fields */
   setup.merge_fields = 1;
   
   switch (ntsc_type)
   {
      case BLARGG_RF: setup.merge_fields = 0;
      case BLARGG_COMPOSITE: setup = snes_ntsc_composite; break;
      case BLARGG_RGB: setup = snes_ntsc_rgb; break;
      case BLARGG_SVIDEO: setup = snes_ntsc_svideo; break;
      case BLARGG_MONOCHROME: setup.merge_fields = 0; setup = snes_ntsc_monochrome; break;
   }
   
   filt->ntsc = (snes_ntsc_t*)calloc(1, sizeof(*filt->ntsc));
   snes_ntsc_init(filt->ntsc, &setup);
   
   filt->burst = 0;
   filt->burst_toggle = (setup.merge_fields ? 0 : 1);
}

static void *blargg_ntsc_generic_create(unsigned in_fmt, unsigned char ntsc_type)
{
   struct filter_data *filt = (struct filter_data*)calloc(1, sizeof(*filt));
   if (!filt)
      return NULL;

   filt->in_fmt = in_fmt;

   blargg_ntsc_initialize(filt, ntsc_type);

   return filt;
}

static void *blargg_ntsc_rf_create(unsigned in_fmt) { return blargg_ntsc_generic_create(in_fmt, BLARGG_RF); }
static void *blargg_ntsc_composite_create(unsigned in_fmt) { return blargg_ntsc_generic_create(in_fmt, BLARGG_COMPOSITE); }
#ifndef GEKKO
static void *blargg_ntsc_rgb_create(unsigned in_fmt) { return blargg_ntsc_generic_create(in_fmt, BLARGG_RGB); }
static void *blargg_ntsc_svideo_create(unsigned in_fmt) { return blargg_ntsc_generic_create(in_fmt, BLARGG_SVIDEO); }
static void *blargg_ntsc_monochrome_create(unsigned in_fmt) { return blargg_ntsc_generic_create(in_fmt, BLARGG_MONOCHROME); }
#endif

static void blargg_ntsc_generic_output(void *data, unsigned *out_width, unsigned *out_height,
      unsigned width, unsigned height)
{
   *out_width  = (width > MAX_LOWRES_WIDTH) ? SNES_NTSC_OUT_WIDTH(width / 2) : SNES_NTSC_OUT_WIDTH(width);
   *out_height = height;
}

static void blargg_ntsc_generic_maxoutput(void *data, unsigned *out_width, unsigned *out_height,
      unsigned width, unsigned height)
{
   unsigned max_width = (width > MAX_LOWRES_WIDTH) ? MAX(MAX_LOWRES_WIDTH, width / 2) : width;
   *out_width  = SNES_NTSC_OUT_WIDTH(max_width);
   *out_height = height;
}

static void blargg_ntsc_generic_destroy(void *data)
{
   struct filter_data *filt = (struct filter_data*)data;

   if(filt->ntsc)
      free(filt->ntsc);

   free(filt);
}

static void blargg_ntsc_rgb565(void *data, int width, int height,
      uint16_t *input, int pitch, uint16_t *output, int outpitch)
{
   struct filter_data *filt = (struct filter_data*)data;

   if(width > MAX_LOWRES_WIDTH)
      snes_ntsc_blit_hires(filt->ntsc, input, pitch, filt->burst, width, height, output, outpitch * 2);
   else
      snes_ntsc_blit(filt->ntsc, input, pitch, filt->burst, width, height, output, outpitch * 2);

   filt->burst ^= filt->burst_toggle;
}

static void blargg_ntsc_generic_render(void *data,
      void *output, size_t output_stride,
      const void *input, unsigned width, unsigned height, size_t input_stride)
{
   struct filter_data *filt = (struct filter_data*)data;

   if (filt->in_fmt == SOFTFILTER_FMT_RGB565)
      blargg_ntsc_rgb565(data, width, height,
         (uint16_t*)input, input_stride / SOFTFILTER_BPP_RGB565, 
         (uint16_t*)output, output_stride / SOFTFILTER_BPP_RGB565);
}

const softfilter_implementation_t blargg_ntsc_rf_implementation = {
   blargg_ntsc_generic_input_fmts,
   blargg_ntsc_generic_output_fmts,

   blargg_ntsc_rf_create,
   blargg_ntsc_generic_destroy,

   blargg_ntsc_generic_output,
   blargg_ntsc_generic_maxoutput,

   blargg_ntsc_generic_render,
   "Blargg NTSC RF",
};

const softfilter_implementation_t blargg_ntsc_composite_implementation = {
   blargg_ntsc_generic_input_fmts,
   blargg_ntsc_generic_output_fmts,

   blargg_ntsc_composite_create,
   blargg_ntsc_generic_destroy,

   blargg_ntsc_generic_output,
   blargg_ntsc_generic_maxoutput,

   blargg_ntsc_generic_render,
   "Blargg NTSC Composite",
};

#ifndef GEKKO
const softfilter_implementation_t blargg_ntsc_rgb_implementation = {
   blargg_ntsc_generic_input_fmts,
   blargg_ntsc_generic_output_fmts,

   blargg_ntsc_rgb_create,
   blargg_ntsc_generic_destroy,

   blargg_ntsc_generic_output,
   blargg_ntsc_generic_maxoutput,

   blargg_ntsc_generic_render,
   "Blargg NTSC RGB",
};

const softfilter_implementation_t blargg_ntsc_svideo_implementation = {
   blargg_ntsc_generic_input_fmts,
   blargg_ntsc_generic_output_fmts,

   blargg_ntsc_svideo_create,
   blargg_ntsc_generic_destroy,

   blargg_ntsc_generic_output,
   blargg_ntsc_generic_maxoutput,

   blargg_ntsc_generic_render,
   "Blargg NTSC S-Video",
};

const softfilter_implementation_t blargg_ntsc_monochrome_implementation = {
   blargg_ntsc_generic_input_fmts,
   blargg_ntsc_generic_output_fmts,

   blargg_ntsc_monochrome_create,
   blargg_ntsc_generic_destroy,

   blargg_ntsc_generic_output,
   blargg_ntsc_generic_maxoutput,

   blargg_ntsc_generic_render,
   "Blargg NTSC Monochrome",
};
#endif

#ifdef RARCH_INTERNAL
#undef filter_data
#endif
