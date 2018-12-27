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

#include "gfx_common.h"
#include "../general.h"
#include "../performance.h"

bool gfx_get_fps(char *buf_fps, size_t size_fps)
{
   uint32_t now = gettime();
   uint32_t delta = diff_usec(g_extern.start_frame_time, now);
   
   if (delta > 1000000)
   {
      snprintf(buf_fps, size_fps, "FPS %3.1f", (float)g_extern.frame_count * 1000000.0 / (float)delta);
      g_extern.frame_count = 0;
      g_extern.start_frame_time = now;
      return true;
   }

   return false;
}

void gfx_scale_integer(struct rarch_viewport *vp, unsigned width, unsigned height, unsigned aspect_ratio_idx, bool keep_aspect, unsigned orientation)
{
   int padding_x = 0;
   int padding_y = 0;
   unsigned base_height;
   unsigned base_width;
   bool rotated = (orientation == ORIENTATION_VERTICAL || orientation == ORIENTATION_FLIPPED_ROTATED); 

   if (aspect_ratio_idx == ASPECT_RATIO_CUSTOM)
   {
      base_width = g_settings.video.custom_vp.width;
      base_height = g_settings.video.custom_vp.height;
      if (rotated) SWAPU(base_width, base_height);

      padding_x = width - base_width;
      padding_y = height - base_height;
      width = base_width;
      height = base_height;
   }
   else
   {
      float aspect_ratio = aspectratio_lut[aspect_ratio_idx].value;
      
      base_height = g_extern.frame.height;
      base_width = aspect_ratio ? (unsigned)roundf(base_height * aspect_ratio) : g_extern.frame.width;
      if (rotated) SWAPU(base_width, base_height);

      if (keep_aspect) // X/Y scale must be same.
      {
         unsigned max_scale = min(width / base_width, height / base_height);
         if (max_scale == 0) max_scale = 1; /* ensure non-zero scale */

         padding_x = width - base_width * max_scale;
         padding_y = height - base_height * max_scale;
      }
      else // X/Y can be independent, each scaled as much as possible.
      {
         padding_x = (base_width > width) ? width - base_width : width % base_width;
         padding_y = (base_height > height) ? height - base_height : height % base_height;
      }

      width     -= padding_x;
      height    -= padding_y;
   }

   vp->width  = width;
   vp->height = height;
   vp->x      = padding_x / 2;
   vp->y      = padding_y / 2;
}

struct aspect_ratio_elem aspectratio_lut[ASPECT_RATIO_END] = {
   { "4:3",           1.3333f },
   { "16:9",          1.7778f },
   { "16:10",         1.6f },
   { "16:15",         16.0f / 15.0f },
   { "1:1",           1.0f },
   { "2:1",           2.0f },
   { "3:2",           1.5f },
   { "3:4",           0.75f },
   { "4:1",           4.0f },
   { "4:4",           1.0f },
   { "5:4",           1.25f },
   { "6:5",           1.2f },
   { "7:9",           0.7777f },
   { "8:3",           2.6666f },
   { "8:7",           1.1428f },
   { "19:12",         1.5833f },
   { "19:14",         1.3571f },
   { "30:17",         1.7647f },
   { "32:9",          3.5555f },
   { "Config",        0.0f },
   { "Square pixel",  1.0f },
   { "Core provided", 1.0f },
   { "Custom",        0.0f },
   { "Original",      0.0f }   
};

char rotation_lut[ORIENTATION_END][16] =
{
   "NORMAL",
   "90 DEG",
   "180 DEG",
   "270 DEG",
   "AUTO"
};

struct menu_theme_elem theme_lut[] = {
   /* NAME      BACKG   FOREG   FONT    HIGHL */
   { "MAROON",  0x7300, 0x7040, 0x7fff, 0x70c0 },
   { "BLUE",    0x7007, 0x7707, 0x7fff, 0x7ff0 },
   { "BLACK",   0x7000, 0x7740, 0x7fff, 0x7f70 },
   { "RETRO",   0x7020, 0x7070, 0x7fff, 0x70f0 },
   { "GRAY",    0x788a, 0x7a47, 0x7fff, 0x7337 },
   { "WHITE",   0x7fff, 0x70aa, 0x7000, 0x70aa }
};

void gfx_set_square_pixel_viewport(unsigned width, unsigned height)
{
   unsigned len, highest, i;
   if (width == 0 || height == 0)
      return;

   len = min(width, height);
   highest = 1;
   for (i = 1; i < len; i++)
   {
      if ((width % i) == 0 && (height % i) == 0)
         highest = i;
   }

   unsigned aspect_x = width / highest;
   unsigned aspect_y = height / highest;

   snprintf(aspectratio_lut[ASPECT_RATIO_SQUARE].name,
         sizeof(aspectratio_lut[ASPECT_RATIO_SQUARE].name),
         "%u:%u (1:1 PAR)", aspect_x, aspect_y);

   aspectratio_lut[ASPECT_RATIO_SQUARE].value = (float)aspect_x / aspect_y;
}

void gfx_set_core_viewport(void)
{
   const struct retro_game_geometry *geom = &g_extern.system.av_info.geometry;
   if (geom->base_width <= 0.0f || geom->base_height <= 0.0f)
      return;

   // Fallback to 1:1 pixel ratio if none provided
   if (geom->aspect_ratio > 0.0f)
      aspectratio_lut[ASPECT_RATIO_CORE].value = geom->aspect_ratio;
   else
      aspectratio_lut[ASPECT_RATIO_CORE].value = (float)geom->base_width / geom->base_height;
}

void gfx_set_config_viewport(void)
{
   if (g_settings.video.manual_aspect_ratio < 0.0f)
   {
      const struct retro_game_geometry *geom = &g_extern.system.av_info.geometry;
      if (geom->aspect_ratio > 0.0f)
         aspectratio_lut[ASPECT_RATIO_CONFIG].value = geom->aspect_ratio;
      else
      {
         unsigned base_width, base_height;
         base_width  = geom->base_width;
         base_height = geom->base_height;

         // Get around division by zero errors
         if (base_width == 0)
            base_width = 1;
         if (base_height == 0)
            base_height = 1;
         aspectratio_lut[ASPECT_RATIO_CONFIG].value = (float)base_width / base_height; // 1:1 PAR.
      }
   }
   else
      aspectratio_lut[ASPECT_RATIO_CONFIG].value = g_settings.video.manual_aspect_ratio;
}

void gfx_check_valid_resolution(void)
{
   /* Ensure we have a valid resolution index when using
    * component cable or forcing interlaced resolutions */
   if (g_extern.video.using_component || g_settings.video.interlaced_resolution_only)
      if(g_settings.video.resolution_idx < g_extern.video.resolution_first_hires)
         g_settings.video.resolution_idx = g_extern.video.resolution_first_hires;   
}

void gfx_match_resolution_auto(void)
{
   /* on resolution AUTO call fnt to refresh the matching resolution */
   if (g_settings.video.resolution_idx == GX_RESOLUTIONS_AUTO && *g_extern.basename)
      driver.video_poke->match_resolution_auto(g_extern.frame.width, g_extern.frame.height);
}
