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

#ifndef INPUT_OVERLAY_H__
#define INPUT_OVERLAY_H__

#include <stdbool.h>
#include "../libretro.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Overlay driver acts as a medium between input drivers and video driver.
// Coordinates are fetched from input driver, and an overlay with pressable actions are
// displayed on-screen.
//
// This interface requires that the video driver has support for the overlay interface.
typedef struct input_overlay input_overlay_t;

input_overlay_t *input_overlay_new(const char *overlay);
void input_overlay_free(input_overlay_t *ol);

void input_overlay_enable(input_overlay_t *ol, bool enable);

bool input_overlay_full_screen(input_overlay_t *ol);

// Sets a modulating factor for alpha channel. Default is 1.0.
// The alpha factor is applied for all overlays.
void input_overlay_set_alpha_mod(input_overlay_t *ol, float mod);

// Scales the overlay by a factor of scale.
void input_overlay_set_scale_factor(input_overlay_t *ol, float scale);

void input_overlay_next(input_overlay_t *ol);

#ifdef __cplusplus
}
#endif

#endif

