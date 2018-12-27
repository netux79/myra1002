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

#ifndef __MENU_DISPLAY_H
#define __MENU_DISPLAY_H

#include <stdbool.h>
#include "../../driver.h"

typedef struct menu_driver
{
   void  (*set_texture)(void*, void*, bool);
   void  (*render_messagebox)(void*, void*, const char*);
   void  (*render)(void*, void*);
   void* (*init)(void*);
   void  (*free)(void*);
   void  (*set_size)(void*);
   int   (*input_postprocess)(void *, uint64_t);

   // Human readable string.
   const char *ident;
} menu_driver_t;

extern const menu_driver_t menu_driver_rgui;

#endif
