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

#include "filter.h"
#include "filters/softfilter.h"
#include "../dynamic.h"
#include "../general.h"
#include "../performance.h"
#include <stdlib.h>


struct rarch_softfilter
{
   const struct softfilter_implementation *impl;
   void *impl_data;

   unsigned max_width, max_height;
   enum retro_pixel_format pix_fmt, out_pix_fmt;

   struct softfilter_work_packet *packets;
};

static const struct softfilter_implementation *(*softfilter_drivers[]) (softfilter_simd_mask_t) =
{
   NULL,
   &blargg_ntsc_snes_rf_get_implementation,
   &blargg_ntsc_snes_composite_get_implementation,
   &lq2x_get_implementation,
   &phosphor2x_get_implementation,
   &twoxsai_get_implementation,
   &supereagle_get_implementation,
   &epx_get_implementation,   
   &scale2x_get_implementation,
   &twoxbr_get_implementation,
   &supertwoxsai_get_implementation,
};

unsigned softfilter_get_last_idx(void)
{
   return sizeof(softfilter_drivers) / sizeof(softfilter_drivers[0]);
}

static softfilter_get_implementation_t softfilter_get_implementation_from_idx(unsigned i)
{
   if (i < softfilter_get_last_idx())
      return softfilter_drivers[i];
   return NULL;
}

const char *rarch_softfilter_get_name(void *data)
{
   (void)data;
   unsigned cpu_features;
   const struct softfilter_implementation *impl;
   softfilter_get_implementation_t cb = (softfilter_get_implementation_t)softfilter_get_implementation_from_idx(g_settings.video.filter_idx);
   if (cb)
   {
      cpu_features = rarch_get_cpu_features();
      impl = (const struct softfilter_implementation *)cb(cpu_features);
      if (impl)
         return impl->ident;
   }

   return NULL;
}

rarch_softfilter_t *rarch_softfilter_new(
      enum retro_pixel_format in_pixel_format,
      unsigned max_width, unsigned max_height)
{
   unsigned cpu_features, output_fmts, input_fmts, input_fmt;
   softfilter_get_implementation_t cb;
    
   rarch_softfilter_t *filt = (rarch_softfilter_t*)calloc(1, sizeof(*filt));
   if (!filt)
      return NULL;

   cb = NULL;
   cb = (softfilter_get_implementation_t)softfilter_get_implementation_from_idx(g_settings.video.filter_idx);
   if (!cb)
   {
      RARCH_ERR("Couldn't find softfilter symbol.\n");
      goto error;
   }

   cpu_features = rarch_get_cpu_features();
   filt->impl = cb(cpu_features);
   if (!filt->impl)
      goto error;

   RARCH_LOG("Loaded softfilter \"%s\".\n", filt->impl->ident);

   if (filt->impl->api_version != SOFTFILTER_API_VERSION)
   {
      RARCH_ERR("Softfilter ABI mismatch.\n");
      goto error;
   }

   // Simple assumptions.
   filt->pix_fmt = in_pixel_format;
   input_fmts = filt->impl->query_input_formats();

   switch (in_pixel_format)
   {
      case RETRO_PIXEL_FORMAT_XRGB8888:
         input_fmt = SOFTFILTER_FMT_XRGB8888;
         break;
      case RETRO_PIXEL_FORMAT_RGB565:
         input_fmt = SOFTFILTER_FMT_RGB565;
         break;
      default:
         goto error;
   }

   if (!(input_fmt & input_fmts))
   {
      RARCH_ERR("Softfilter does not support input format.\n");
      goto error;
   }

   output_fmts = filt->impl->query_output_formats(input_fmt);
   if (output_fmts & input_fmt) // If we have a match of input/output formats, use that.
      filt->out_pix_fmt = in_pixel_format;
   else if (output_fmts & SOFTFILTER_FMT_XRGB8888)
      filt->out_pix_fmt = RETRO_PIXEL_FORMAT_XRGB8888;
   else if (output_fmts & SOFTFILTER_FMT_RGB565)
      filt->out_pix_fmt = RETRO_PIXEL_FORMAT_RGB565;
   else
   {
      RARCH_ERR("Did not find suitable output format for softfilter.\n");
      goto error;
   }

   filt->max_width = max_width;
   filt->max_height = max_height;

   filt->impl_data = filt->impl->create(input_fmt, input_fmt, max_width, max_height, 1, cpu_features);
   if (!filt->impl_data)
   {
      RARCH_ERR("Failed to create softfilter state.\n");
      goto error;
   }

   filt->packets = (struct softfilter_work_packet*)calloc(1, sizeof(*filt->packets));
   if (!filt->packets)
   {
      RARCH_ERR("Failed to allocate softfilter packets.\n");
      goto error;
   }

   return filt;

error:
   rarch_softfilter_free(filt);
   return NULL;
}

void rarch_softfilter_free(rarch_softfilter_t *filt)
{
   if (!filt)
      return;

   free(filt->packets);
   if (filt->impl && filt->impl_data)
      filt->impl->destroy(filt->impl_data);
   free(filt);
}

void rarch_softfilter_get_max_output_size(rarch_softfilter_t *filt,
      unsigned *width, unsigned *height)
{
   rarch_softfilter_get_output_size(filt, width, height, filt->max_width, filt->max_height);
}

void rarch_softfilter_get_output_size(rarch_softfilter_t *filt,
      unsigned *out_width, unsigned *out_height,
      unsigned width, unsigned height)
{
   if (filt && filt->impl && filt->impl->query_output_size)
      filt->impl->query_output_size(filt->impl_data, out_width, out_height, width, height);
}

enum retro_pixel_format rarch_softfilter_get_output_format(rarch_softfilter_t *filt)
{
   return filt->out_pix_fmt;
}

void rarch_softfilter_process(rarch_softfilter_t *filt,
      void *output, size_t output_stride,
      const void *input, unsigned width, unsigned height, size_t input_stride)
{
   if (filt && filt->impl && filt->impl->get_work_packets)
   {
      filt->impl->get_work_packets(filt->impl_data, filt->packets,
            output, output_stride, input, width, height, input_stride);
   
      filt->packets->work(filt->impl_data, filt->packets->thread_data);
   }
}
