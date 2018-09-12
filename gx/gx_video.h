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

struct gx_tvinfo
{
   unsigned tvmode;
   unsigned max_width;
   unsigned max_height;
   bool progressive;
};

typedef struct gx_video
{
   float aspect_ratio;
   unsigned frame_w, frame_h; /* keep track of frame size */ 
   unsigned resolution_idx;
   unsigned aspect_ratio_idx;
   unsigned bpp;
   unsigned scale;
   unsigned orientation;
   unsigned *menu_data;
   rarch_viewport_t vp;
#ifdef HAVE_OVERLAY
   struct gx_overlay_data *overlay;
   unsigned overlays;
#endif
   struct gx_tvinfo tvinfo;
   bool double_strike;
   bool rgb32;
   bool should_resize;
   bool rgui_texture_enable;
   bool scale_integer;
   bool force_aspect;
} gx_video_t;

#endif
