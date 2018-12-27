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

#ifndef __GFX_COMMON_H
#define __GFX_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "../general.h"
#include <stdbool.h>

#define SWAPU(a,b) {unsigned t=a;a=b;b=t;}

enum aspect_ratio
{
   ASPECT_RATIO_4_3 = 0,
   ASPECT_RATIO_16_9,
   ASPECT_RATIO_16_10,
   ASPECT_RATIO_16_15,
   ASPECT_RATIO_1_1,
   ASPECT_RATIO_2_1,
   ASPECT_RATIO_3_2,
   ASPECT_RATIO_3_4,
   ASPECT_RATIO_4_1,
   ASPECT_RATIO_4_4,
   ASPECT_RATIO_5_4,
   ASPECT_RATIO_6_5,
   ASPECT_RATIO_7_9,
   ASPECT_RATIO_8_3,
   ASPECT_RATIO_8_7,
   ASPECT_RATIO_19_12,
   ASPECT_RATIO_19_14,
   ASPECT_RATIO_30_17,
   ASPECT_RATIO_32_9,
   ASPECT_RATIO_CONFIG,
   ASPECT_RATIO_SQUARE,
   ASPECT_RATIO_CORE,
   ASPECT_RATIO_CUSTOM,
   ASPECT_RATIO_ORIGINAL,
   ASPECT_RATIO_END
};

#define LAST_ASPECT_RATIO (ASPECT_RATIO_END - 1)

enum rotation
{
   ORIENTATION_NORMAL = 0,
   ORIENTATION_VERTICAL,
   ORIENTATION_FLIPPED,
   ORIENTATION_FLIPPED_ROTATED,
   ORIENTATION_AUTO,
   ORIENTATION_END
};

#define LAST_ORIENTATION (ORIENTATION_END - 1)
#define LAST_MENU_ORIENTATION (ORIENTATION_END - 2)

extern char rotation_lut[ORIENTATION_END][16];

struct aspect_ratio_elem
{
   char name[32];
   float value;
};

extern struct aspect_ratio_elem aspectratio_lut[ASPECT_RATIO_END];

struct menu_theme_elem
{
   char name[32];
   uint16_t background;
   uint16_t foreground;
   uint16_t font_nor;
   uint16_t font_sel;
};

extern struct menu_theme_elem theme_lut[];

#define FIRST_THEME  0
#define LAST_THEME   sizeof(theme_lut) / sizeof(struct menu_theme_elem) - 1

bool gfx_get_fps(char *buf_fps, size_t size_fps);
void gfx_scale_integer(struct rarch_viewport *vp, unsigned width, unsigned height,
                       unsigned aspect_ratio_idx, bool keep_aspect, unsigned orientation);
void gfx_set_square_pixel_viewport(unsigned width, unsigned height);
void gfx_set_core_viewport(void);
void gfx_set_config_viewport(void);
void gfx_match_resolution_auto(void);
void gfx_check_valid_resolution(void);

#ifdef __cplusplus
}
#endif

#endif
