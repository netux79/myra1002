/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2014 - Hans-Kristian Arntzen
 *  Copyright (C) 2011-2014 - Daniel De Matteis
 *  Copyright (C) 2012-2014 - Michael Lelli
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

#ifndef _GX_VIDEO_H__
#define _GX_VIDEO_H__

struct gx_overlay_data
{
   GXTexObj tex;
   float tex_coord[8];
   float vertex_coord[8];
   float alpha_mod;
};

typedef struct gx_video
{
   bool should_resize;
   unsigned aspect_ratio_idx;
   float aspect_ratio;
   bool double_strike;
   bool rgb32;
   uint32_t *menu_data; // FIXME: Should be const uint16_t*.
   bool rgui_texture_enable;
   rarch_viewport_t vp;
   unsigned scale;
   bool scale_integer;
   unsigned orientation;
   struct 
   {
      unsigned tvmode;
      unsigned max_width;
      unsigned max_height;
      bool progressive;
   } tvinfo;
#ifdef HAVE_OVERLAY
   struct gx_overlay_data *overlay;
   unsigned overlays;
   bool overlay_enable;
#endif
} gx_video_t;

enum
{
   GX_RESOLUTIONS_256_192 = 0,
   GX_RESOLUTIONS_299_200,
   GX_RESOLUTIONS_320_200,
   GX_RESOLUTIONS_170_224,
   GX_RESOLUTIONS_192_224,
   GX_RESOLUTIONS_224_224,
   GX_RESOLUTIONS_240_224,
   GX_RESOLUTIONS_256_224,
   GX_RESOLUTIONS_304_224,
   GX_RESOLUTIONS_320_224,
   GX_RESOLUTIONS_384_224,
   GX_RESOLUTIONS_512_224,
   GX_RESOLUTIONS_256_232,
   GX_RESOLUTIONS_256_236,
   GX_RESOLUTIONS_192_240,
   GX_RESOLUTIONS_256_240,
   GX_RESOLUTIONS_265_240,
   GX_RESOLUTIONS_288_240,
   GX_RESOLUTIONS_320_240,
   GX_RESOLUTIONS_512_384,
   GX_RESOLUTIONS_598_400,
   GX_RESOLUTIONS_640_400,
   GX_RESOLUTIONS_340_448,
   GX_RESOLUTIONS_384_448,
   GX_RESOLUTIONS_448_448,
   GX_RESOLUTIONS_480_448,
   GX_RESOLUTIONS_512_448,
   GX_RESOLUTIONS_608_448,
   GX_RESOLUTIONS_640_448,
   GX_RESOLUTIONS_512_464,
   GX_RESOLUTIONS_512_472,
   GX_RESOLUTIONS_384_480,
   GX_RESOLUTIONS_512_480,
   GX_RESOLUTIONS_530_480,
   GX_RESOLUTIONS_576_480,
   GX_RESOLUTIONS_640_480,
   GX_RESOLUTIONS_AUTO,
   GX_RESOLUTIONS_LAST = GX_RESOLUTIONS_AUTO,
};

#endif
