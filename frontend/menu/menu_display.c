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

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#include "menu_common.h"
#include "menu_display.h"
#include "file_list.h"
#include "../../general.h"
#include "../../gfx/gfx_common.h"
#include "../../config.def.h"
#include "../../file.h"
#include "../../dynamic.h"
#include "../../performance.h"
#include "../../input/input_common.h"

#include "../../screenshot.h"
#include "../../gfx/fonts/bitmap.h"

static uint16_t menu_framebuf[400 * 240];

#define RGUI_TERM_START_X 15
#define RGUI_TERM_START_Y 27
#define RGUI_TERM_WIDTH (((rgui->width - RGUI_TERM_START_X - 15) / (FONT_WIDTH_STRIDE)))
#define RGUI_TERM_HEIGHT (((rgui->height - RGUI_TERM_START_Y - 15) / (FONT_HEIGHT_STRIDE)) - 1)

static uint16_t back_filler(void)
{
   return theme_lut[g_settings.menu.theme].background;
}

static uint16_t fore_filler(void)
{
   return theme_lut[g_settings.menu.theme].foreground;
}

static uint16_t font_col_nor(void)
{
   return theme_lut[g_settings.menu.theme].font_nor;
}

static uint16_t font_col_sel(void)
{
   return theme_lut[g_settings.menu.theme].font_sel;
}

static void fill_rect(uint16_t *buf, unsigned pitch,
      unsigned x, unsigned y,
      unsigned width, unsigned height,
      uint16_t (*col)(void))
{
   unsigned j, i;
   for (j = y; j < y + height; j++)
      for (i = x; i < x + width; i++)
         buf[j * (pitch >> 1) + i] = col();
}

static void blit_line(rgui_handle_t *rgui,
      int x, int y, const char *message, uint16_t (*col)(void))
{
   int j, i;
   while (*message)
   {
      for (j = 0; j < FONT_HEIGHT; j++)
      {
         for (i = 0; i < FONT_WIDTH; i++)
         {
            uint8_t rem = 1 << ((i + j * FONT_WIDTH) & 7);
            int offset = (i + j * FONT_WIDTH) >> 3;
            if(rgui->font[FONT_OFFSET((unsigned char)*message) + offset] & rem)
               rgui->frame_buf[(y + j) * (rgui->frame_buf_pitch >> 1) + (x + i)] = col();
         }
      }

      x += FONT_WIDTH_STRIDE;
      message++;
   }
}

static void rgui_render_background(rgui_handle_t *rgui)
{
   fill_rect(rgui->frame_buf, rgui->frame_buf_pitch,
         0, 0, rgui->width, rgui->height, back_filler);
}

static void rgui_render_messagebox(void *data, void *video_data, const char *message)
{
   rgui_handle_t *rgui = (rgui_handle_t*)data;
   size_t i;

   if (!message || !*message)
      return;

   struct string_list *list = string_split(message, "\n");
   if (!list)
      return;
   if (list->elems == 0)
   {
      string_list_free(list);
      return;
   }

   unsigned width = 0;
   unsigned glyphs_width = 0;
   for (i = 0; i < list->size; i++)
   {
      char *msg = list->elems[i].data;
      unsigned msglen = strlen(msg);
      if (msglen > RGUI_TERM_WIDTH)
      {
         msg[RGUI_TERM_WIDTH - 2] = '.';
         msg[RGUI_TERM_WIDTH - 1] = '.';
         msg[RGUI_TERM_WIDTH - 0] = '.';
         msg[RGUI_TERM_WIDTH + 1] = '\0';
         msglen = RGUI_TERM_WIDTH;
      }

      unsigned line_width = msglen * FONT_WIDTH_STRIDE - 1 + 6 + 10;
      width = max(width, line_width);
      glyphs_width = max(glyphs_width, msglen);
   }

   unsigned height = FONT_HEIGHT_STRIDE * list->size + 6 + 10;
   int x = (rgui->width - width) / 2;
   int y = (rgui->height - height) / 2;

   fill_rect(rgui->frame_buf, rgui->frame_buf_pitch,
         x, y, width, height, fore_filler);

   for (i = 0; i < list->size; i++)
   {
      const char *msg = list->elems[i].data;
      int offset_x = FONT_WIDTH_STRIDE * (glyphs_width - strlen(msg)) / 2;
      int offset_y = FONT_HEIGHT_STRIDE * i;
      blit_line(rgui, x + 8 + offset_x, y + 8 + offset_y, msg, font_col_nor);
   }

   string_list_free(list);
}

static void rgui_render(void *data, void *video_data)
{
   rgui_handle_t *rgui = (rgui_handle_t*)data;

   if (rgui->need_refresh &&
         (g_extern.lifecycle_state & (1ULL << MODE_MENU))
         && !rgui->msg_force)
      return;

   size_t begin = rgui->selection_ptr >= RGUI_TERM_HEIGHT / 2 ?
      rgui->selection_ptr - RGUI_TERM_HEIGHT / 2 : 0;
   size_t end = rgui->selection_ptr + RGUI_TERM_HEIGHT <= rgui->selection_buf->size ?
      rgui->selection_ptr + RGUI_TERM_HEIGHT : rgui->selection_buf->size;

   // Do not scroll if all items are visible.
   if (rgui->selection_buf->size <= RGUI_TERM_HEIGHT)
      begin = 0;

   if (end - begin > RGUI_TERM_HEIGHT)
      end = begin + RGUI_TERM_HEIGHT;

   rgui_render_background(rgui);

   char title[256];
   const char *dir = NULL;
   unsigned menu_type = 0;
   file_list_get_last(rgui->menu_stack, &dir, &menu_type);

   if (menu_type == RGUI_SETTINGS_CORE)
      snprintf(title, sizeof(title), "CORE SELECTION %s", dir);
   else if (menu_type == RGUI_SETTINGS_DEFERRED_CORE)
      snprintf(title, sizeof(title), "DETECTED CORES %s", dir);
   else if (menu_type == RGUI_SETTINGS_DISK_APPEND)
      snprintf(title, sizeof(title), "DISK APPEND %s", dir);
   else if (menu_type == RGUI_SETTINGS_VIDEO_OPTIONS)
      strlcpy(title, "VIDEO OPTIONS", sizeof(title));
   else if (menu_type == RGUI_SETTINGS_INPUT_OPTIONS)
      strlcpy(title, "INPUT OPTIONS", sizeof(title));
   else if (menu_type == RGUI_SETTINGS_CUSTOM_BIND)
      strlcpy(title, "BIND KEY", sizeof(title));
   else if (menu_type == RGUI_SETTINGS_BIND_PLAYER_KEYS)
      strlcpy(title, "BIND PLAYER KEYS", sizeof(title));
   else if (menu_type == RGUI_SETTINGS_BIND_HOTKEYS)
      strlcpy(title, "BIND HOTKEYS", sizeof(title));
   else if (menu_type == RGUI_SETTINGS_OVERLAY_OPTIONS)
      strlcpy(title, "OVERLAY OPTIONS", sizeof(title));
   else if (menu_type == RGUI_SETTINGS_PATH_OPTIONS)
      strlcpy(title, "PATH OPTIONS", sizeof(title));
   else if (menu_type == RGUI_SETTINGS_OPTIONS)
      strlcpy(title, "SETTINGS", sizeof(title));
   else if (menu_type == RGUI_SETTINGS_CONFIG_OPTIONS)
      strlcpy(title, "CONFIG OPTIONS", sizeof(title));
   else if (menu_type == RGUI_SETTINGS_MENU_OPTIONS)
      strlcpy(title, "MENU OPTIONS", sizeof(title));
   else if (menu_type == RGUI_SETTINGS_SAVE_OPTIONS)
      strlcpy(title, "SAVE OPTIONS", sizeof(title));
   else if (menu_type == RGUI_SETTINGS_AUDIO_OPTIONS)
      strlcpy(title, "AUDIO OPTIONS", sizeof(title));
   else if (menu_type == RGUI_SETTINGS_DISK_OPTIONS)
      strlcpy(title, "DISK OPTIONS", sizeof(title));
   else if (menu_type == RGUI_SETTINGS_CORE_OPTIONS)
      strlcpy(title, "CORE SETUP", sizeof(title));
   else if (menu_type == RGUI_SETTINGS_SYSTEM_INFO)
      strlcpy(title, "SYSTEM INFO", sizeof(title));
   else if (menu_type == RGUI_SETTINGS)
      snprintf(title, sizeof(title), "%s | %s ", PACKAGE_NAME, *g_extern.basename ? path_basename(g_extern.basename) : "No Game Loaded");
   else if (menu_type == RGUI_SETTINGS_OPEN_HISTORY)
      strlcpy(title, "LOAD RECENT", sizeof(title));
#ifdef HAVE_OVERLAY
   else if (menu_type == RGUI_SETTINGS_OVERLAY_PRESET)
      snprintf(title, sizeof(title), "OVERLAY %s", dir);
#endif
   else if (menu_type == RGUI_GAMES_DIR_PATH)
      snprintf(title, sizeof(title), "GAMES PATH %s", dir);
   else if (menu_type == RGUI_LIBRETRO_INFO_DIR_PATH)
      snprintf(title, sizeof(title), "CORE INFO PATH %s", dir);
#ifdef HAVE_SCREENSHOTS
   else if (menu_type == RGUI_SCREENSHOT_DIR_PATH)
      snprintf(title, sizeof(title), "SCREENSHOT PATH %s", dir);
#endif
   else if (menu_type == RGUI_SAVESTATE_DIR_PATH)
      snprintf(title, sizeof(title), "SAVESTATE PATH %s", dir);
   else if (menu_type == RGUI_CONFIG_DIR_PATH)
      snprintf(title, sizeof(title), "CONFIG PATH %s", dir);
   else if (menu_type == RGUI_SAVEFILE_DIR_PATH)
      snprintf(title, sizeof(title), "SAVEFILE PATH %s", dir);
#ifdef HAVE_OVERLAY
   else if (menu_type == RGUI_OVERLAY_DIR_PATH)
      snprintf(title, sizeof(title), "OVERLAY PATH %s", dir);
#endif
   else if (menu_type == RGUI_SYSTEM_DIR_PATH)
      snprintf(title, sizeof(title), "SYSTEM PATH %s", dir);
   else
   {
      if (rgui->defer_core)
         snprintf(title, sizeof(title), "GAME %s", dir);
      else
      {
         const char *core_name = rgui->info.library_name;
         if (!core_name)
            core_name = g_extern.system.info.library_name;
         if (!core_name)
            core_name = "No Core";
         snprintf(title, sizeof(title), "GAME (%s) %s", core_name, dir);
      }
   }

   char buf[256];
   menu_ticker_line(buf, RGUI_TERM_WIDTH - 3, g_extern.frame_count / 15, title, true);
   blit_line(rgui, RGUI_TERM_START_X + 15, 15, buf, font_col_sel);

   const char *core_name = rgui->core_info_current.data ? rgui->core_info_current.display_name : rgui->info.library_name;
   strlcpy(buf, core_name, sizeof(buf));
   blit_line(rgui, RGUI_TERM_START_X + 15, (RGUI_TERM_HEIGHT * FONT_HEIGHT_STRIDE) + RGUI_TERM_START_Y + 2, buf, font_col_sel);

   /* get the time */
   time_t _time;
   time(&_time);
   char time_display[9];
   /* Time (hours-minutes-seconds) */
   strftime(time_display, sizeof(time_display), "%H:%M:%S", localtime(&_time));
   blit_line(rgui, RGUI_TERM_WIDTH * FONT_WIDTH_STRIDE + RGUI_TERM_START_X - (FONT_WIDTH_STRIDE*9),
                   RGUI_TERM_HEIGHT * FONT_HEIGHT_STRIDE + RGUI_TERM_START_Y + 2, 
                   time_display, font_col_sel);

   unsigned x, y;
   size_t i;

   x = RGUI_TERM_START_X;
   y = RGUI_TERM_START_Y;

   for (i = begin; i < end; i++, y += FONT_HEIGHT_STRIDE)
   {
      const char *path = 0;
      unsigned type = 0;
      file_list_get_at_offset(rgui->selection_buf, i, &path, &type);
      char message[256];
      char type_str[256];

      unsigned w = RGUI_TERM_WIDTH / 2.8;

      // Pretty-print libretro cores from menu.
      if (menu_type == RGUI_SETTINGS_CORE || menu_type == RGUI_SETTINGS_DEFERRED_CORE)
      {
         if (type == RGUI_FILE_PLAIN)
         {
            strlcpy(type_str, "(CORE)", sizeof(type_str));
            file_list_get_alt_at_offset(rgui->selection_buf, i, &path);
            w = 6;
         }
         else
         {
            strlcpy(type_str, "(DIR)", sizeof(type_str));
            type = RGUI_FILE_DIRECTORY;
            w = 5;
         }
      }
      else if (
#ifdef HAVE_OVERLAY
            menu_type == RGUI_SETTINGS_OVERLAY_PRESET ||
#endif
            menu_type == RGUI_SETTINGS_DISK_APPEND ||
            menu_type_is(menu_type) == RGUI_FILE_DIRECTORY)
      {
         if (type == RGUI_FILE_PLAIN)
         {
            strlcpy(type_str, "(FILE)", sizeof(type_str));
            w = 6;
         }
         else if (type == RGUI_FILE_USE_DIRECTORY)
         {
            *type_str = '\0';
            w = 0;
         }
         else
         {
            strlcpy(type_str, "(DIR)", sizeof(type_str));
            type = RGUI_FILE_DIRECTORY;
            w = 5;
         }
      }
      else if (menu_type == RGUI_SETTINGS_OPEN_HISTORY)
      {
         *type_str = '\0';
         w = 0;
      }
      else if (type >= RGUI_SETTINGS_CORE_OPTION_START)
         strlcpy(type_str,
               core_option_get_val(g_extern.system.core_options, type - RGUI_SETTINGS_CORE_OPTION_START),
               sizeof(type_str));
      else
         menu_set_settings_label(type_str, sizeof(type_str), &w, type);

      char entry_title_buf[256];
      char type_str_buf[64];
      bool selected = i == rgui->selection_ptr;

      strlcpy(entry_title_buf, path, sizeof(entry_title_buf));
      strlcpy(type_str_buf, type_str, sizeof(type_str_buf));

      if (type == RGUI_FILE_PLAIN || type == RGUI_FILE_DIRECTORY || type == RGUI_SETTINGS_SYSTEM_INFO_NONE)
         menu_ticker_line(entry_title_buf, RGUI_TERM_WIDTH - (w + 1 + 2), g_extern.frame_count / 15, path, selected);
      else
         menu_ticker_line(type_str_buf, w, g_extern.frame_count / 15, type_str, selected);

      snprintf(message, sizeof(message), "%c %-*.*s %-*s",
            selected ? '>' : ' ',
            RGUI_TERM_WIDTH - (w + 1 + 2), RGUI_TERM_WIDTH - (w + 1 + 2),
            entry_title_buf,
            w,
            type_str_buf);

      blit_line(rgui, x, y, message, selected ? font_col_sel : font_col_nor);
   }

   const char *message_queue;

   if (rgui->msg_force)
   {
      message_queue = msg_queue_pull(g_extern.msg_queue);
      rgui->msg_force = false;
   }
   else
      message_queue = driver.current_msg;

   rgui_render_messagebox(rgui, video_data, message_queue);
}

static void rgui_set_size(void *data)
{
   rgui_handle_t *rgui = (rgui_handle_t*)data;

   rgui->width = g_settings.menu.rotation % 2 ? 240: 320;
   rgui->height = g_settings.menu.rotation % 2 ? 320: 240;
   rgui->frame_buf_pitch = rgui->width * sizeof(uint16_t);
}

static void *rgui_init(void *video_data)
{
   rgui_handle_t *rgui = (rgui_handle_t*)calloc(1, sizeof(*rgui));

   rgui->frame_buf = menu_framebuf;
   rgui_set_size(rgui);
   rgui->font = bitmap_bin;

   return rgui;
}

static void rgui_free(void *data)
{
   rgui_handle_t *rgui = (rgui_handle_t*)data;
   free(rgui);
}

static int rgui_input_postprocess(void *data, uint64_t old_state)
{
   (void)data;
   (void)old_state;

   /* check if we need to return to game */
   if ((rgui->trigger_state & (1ULL << RARCH_MENU_TOGGLE))
         && g_extern.main_is_init && !g_extern.libretro_dummy)
   {
      g_extern.lifecycle_state |= (1ULL << MODE_GAME);
      return 1;
   }
   
   /* Check if we need to exit */
   if (rgui->trigger_state & (1ULL << RARCH_QUIT_KEY))
      return 1;

   return 0;
}

void rgui_set_texture(void *data, void *video_data, bool enable)
{
   rgui_handle_t *rgui = (rgui_handle_t*)data;
   driver.video_poke->set_texture_frame(video_data, menu_framebuf, enable, rgui->width, rgui->height, 1.0f);
}

const menu_driver_t menu_driver_rgui = {
   rgui_set_texture,
   rgui_render_messagebox,
   rgui_render,
   rgui_init,
   rgui_free,
   rgui_set_size,
   rgui_input_postprocess,
   "rgui",
};
