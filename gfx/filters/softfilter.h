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

#ifndef SOFTFILTER_API_H__
#define SOFTFILTER_API_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Dynamic library entrypoint.
typedef const struct softfilter_implementation *(*softfilter_get_implementation_t)(void);
// The same SIMD mask argument is forwarded to create() callback as well to avoid having to keep lots of state around.
const struct softfilter_implementation *softfilter_get_implementation(void);

// Required base color formats

#define SOFTFILTER_FMT_NONE     0
#define SOFTFILTER_FMT_RGB565   (1 << 0)
#define SOFTFILTER_FMT_XRGB8888 (1 << 1)

#define SOFTFILTER_BPP_RGB565   2
#define SOFTFILTER_BPP_XRGB8888 4

// Softfilter implementation.
// Returns a bitmask of supported input formats.
typedef unsigned (*softfilter_query_input_formats_t)(void);

// Returns a bitmask of supported output formats for a given input format.
typedef unsigned (*softfilter_query_output_formats_t)(unsigned input_format);

// Create a filter with given input and output formats as well as maximum possible input size.
// Input sizes can very per call to softfilter_process_t, but they will never be larger than the maximum.
typedef void *(*softfilter_create_t)(unsigned in_fmt);
typedef void (*softfilter_destroy_t)(void *data);

// Given an input size, query the output size of the filter.
// If width and height == max_width/max_height, no other combination of width/height must return a larger size in any dimension.
typedef void (*softfilter_query_output_size_t)(void *data,
      unsigned *out_width, unsigned *out_height,
      unsigned width, unsigned height);

// First step of processing a frame. It provides all the information to the filter to
// rendered to the ouput buffer.
typedef void (*softfilter_render_filter_t)(void *data,
      void *output, size_t output_stride,
      const void *input, unsigned width, unsigned height, size_t input_stride);

struct softfilter_implementation
{
   softfilter_query_input_formats_t query_input_formats;
   softfilter_query_output_formats_t query_output_formats;

   softfilter_create_t create;
   softfilter_destroy_t destroy;

   softfilter_query_output_size_t query_output_size;
   softfilter_render_filter_t render_filter;

   const char *ident; // Human readable identifier of implementation.
};

#ifdef __cplusplus
}
#endif

#endif

