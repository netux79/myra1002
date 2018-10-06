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

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "menu_common.h"
#include "menu_navigation.h"

#include "../../gfx/gfx_common.h"
#include "../../performance.h"
#include "../../driver.h"
#include "../../file.h"
#include "../../file_ext.h"
#include "../../input/input_common.h"

rgui_handle_t *rgui;
const menu_driver_t *menugui_driver;

static void menu_parse_and_resolve(void *data, unsigned menu_type);

static void menu_update_system_info(void *data, bool *load_no_rom)
{
   rgui_handle_t *rgui = (rgui_handle_t*)data;

   // Keep track of info for the currently selected core.
   if (rgui->core_info)
   {
      if (core_info_list_get_info(rgui->core_info, &rgui->core_info_current, g_settings.libretro))
      {
         const core_info_t *info = &rgui->core_info_current;

         RARCH_LOG("[Core Info]:\n");
         if (info->display_name)
            RARCH_LOG("  Display Name: %s\n", info->display_name);
         if (info->supported_extensions)
            RARCH_LOG("  Supported Extensions: %s\n", info->supported_extensions);
         if (info->authors)
            RARCH_LOG("  Authors: %s\n", info->authors);
         if (info->permissions)
            RARCH_LOG("  Permissions: %s\n", info->permissions);
      }
   }
}

//forward decl
static int menu_iterate_func(void *data, void *video_data, unsigned action);

void menu_rom_history_push(const char *path,
      const char *core_path,
      const char *core_name)
{
   if (rgui->history)
      rom_history_push(rgui->history, path, core_path, core_name);
}

void load_menu_game_prepare(void *video_data)
{
   if (*g_extern.fullpath || rgui->load_no_rom)
   {
      if (*g_extern.fullpath)
      {
         char tmp[PATH_MAX];
         char str[PATH_MAX];

         fill_pathname_base(tmp, g_extern.fullpath, sizeof(tmp));
         path_remove_extension(tmp);
         snprintf(str, sizeof(str), "- Starting -\n%s", tmp);
         msg_queue_push(g_extern.msg_queue, str, 1, 1);
      }

      if (g_extern.system.no_game || *g_extern.fullpath)
      menu_rom_history_push(*g_extern.fullpath ? g_extern.fullpath : NULL,
            g_settings.libretro, rgui->info.library_name ? rgui->info.library_name : "");
   }

   // redraw RGUI frame
   rgui->old_input_state = rgui->trigger_state = 0;
   rgui->do_held = false;
   rgui->msg_force = true;

   if (menugui_driver)
      menu_iterate_func(rgui, video_data, RGUI_ACTION_NOOP);

   // Draw frame for loading message
   if (video_data && driver.video_poke && driver.video_poke->set_texture_enable)
      driver.video_poke->set_texture_enable(video_data, rgui->frame_buf_show, false);

   if (driver.video)
      rarch_render_cached_frame();

   if (video_data && driver.video_poke && driver.video_poke->set_texture_enable)
      driver.video_poke->set_texture_enable(video_data, false, false);
}

bool load_menu_game_new_core(const char* gamepath, const char* corepath)
{
   /* If the core is the one actually loaded, no need 
    * to load it again, just launch the game! */
   if (strcmp(g_settings.libretro, corepath) == 0)
   {
      strlcpy(g_extern.fullpath, gamepath, sizeof(g_extern.fullpath));
      g_extern.lifecycle_state |= (1ULL << MODE_LOAD_GAME);
      return false;
   }
   else
   {
      corepath = path_basename(corepath);
      /* if SET_LIBRETRO_PATH fails (core doesn't found) copy the path 
       * anyway to avoid launching current core with incorrect game. */
      if (!rarch_environment_cb(RETRO_ENVIRONMENT_SET_LIBRETRO_PATH, (void*)corepath))
         strlcpy(g_settings.libretro, corepath, sizeof(g_settings.libretro));
      rarch_environment_cb(RETRO_ENVIRONMENT_EXEC, (void*)gamepath);
      return true;
   }
}

void load_menu_game_history(unsigned game_index)
{
   const char *path = NULL;
   const char *core_path = NULL;
   const char *core_name = NULL;

   rom_history_get_index(rgui->history, game_index, &path, &core_path, &core_name);
   rgui->load_no_rom = path ? false : true;
   /* load it as a normal game */
   load_menu_game_new_core(path, core_path);
}

static void menu_init_history(void)
{
   if (rgui->history)
   {
      rom_history_free(rgui->history);
      rgui->history = NULL;
   }

   if (*g_extern.config_path)
   {
      char history_path[PATH_MAX];
      fill_pathname_resolve_relative(history_path, g_extern.config_path,
               "retroarch-game-history.txt", sizeof(history_path));

      RARCH_LOG("[RGUI]: Opening history: %s.\n", history_path);
      rgui->history = rom_history_init(history_path, g_settings.game_history_size);
   }
}

static void menu_update_libretro_info(void)
{
   retro_get_system_info(&rgui->info);
   menu_init_core_info(rgui);
}

bool load_menu_game(void)
{
   if (g_extern.main_is_init)
      rarch_main_deinit();

   struct rarch_main_wrap args = {0};

   args.config_path   = *g_extern.config_path ? g_extern.config_path : NULL;
   args.sram_path     = *g_settings.savefile_directory ? g_settings.savefile_directory : NULL;
   args.state_path    = *g_settings.savestate_directory ? g_settings.savestate_directory : NULL;
   args.rom_path      = *g_extern.fullpath ? g_extern.fullpath : NULL;
   args.libretro_path = *g_settings.libretro ? g_settings.libretro : NULL;
   args.no_rom        = rgui->load_no_rom;
   rgui->load_no_rom  = false;

   if (rarch_main_init_wrap(&args) == 0)
   {
      RARCH_LOG("rarch_main_init_wrap() succeeded.\n");
      // Update menu state which depends on config.
      menu_update_libretro_info();
      menu_init_history();

      return true;
   }
   else
   {
      char name[PATH_MAX];
      char msg[PATH_MAX];
      fill_pathname_base(name, g_extern.fullpath, sizeof(name));
      snprintf(msg, sizeof(msg), "Failed to load %s.\n", name);
      msg_queue_push(g_extern.msg_queue, msg, 1, 90);
      rgui->msg_force = true;
      RARCH_ERR("rarch_main_init_wrap() failed.\n");
      return false;
   }
}

void menu_init(void *video_data)
{
   /* Set a pointer to the menu setup */
   menugui_driver = &menu_driver_rgui;
   
   rgui = menugui_driver->init(video_data);   
   if (!rgui)
   {
      RARCH_ERR("Could not initialize menu.\n");
      rarch_fail(1, "menu_init()");
   }

   rgui->menu_stack = (file_list_t*)calloc(1, sizeof(file_list_t));
   rgui->selection_buf = (file_list_t*)calloc(1, sizeof(file_list_t));
   file_list_push(rgui->menu_stack, "", RGUI_SETTINGS, 0);
   menu_clear_navigation(rgui);
   menu_populate_entries(rgui, RGUI_SETTINGS);

   rgui->trigger_state = 0;
   rgui->old_input_state = 0;
   rgui->do_held = false;
   rgui->frame_buf_show = true;
   rgui->c_player = 0;

   menu_update_libretro_info();

   menu_init_history();
   rgui->last_time = rarch_get_time_usec();
}

void menu_free(void *video_data)
{
   if (menugui_driver && menugui_driver->free)
      menugui_driver->free(rgui);

   file_list_free(rgui->menu_stack);
   file_list_free(rgui->selection_buf);

   rom_history_free(rgui->history);
   core_info_list_free(rgui->core_info);

   free(rgui);
}

void menu_ticker_line(char *buf, size_t len, unsigned index, const char *str, bool selected)
{
   size_t str_len = strlen(str);
   if (str_len <= len)
   {
      strlcpy(buf, str, len + 1);
      return;
   }

   if (!selected)
   {
      strlcpy(buf, str, len + 1 - 3);
      strlcat(buf, "...", len + 1);
   }
   else
   {
      // Wrap long strings in options with some kind of ticker line.
      unsigned ticker_period = 2 * (str_len - len) + 4;
      unsigned phase = index % ticker_period;

      unsigned phase_left_stop = 2;
      unsigned phase_left_moving = phase_left_stop + (str_len - len);
      unsigned phase_right_stop = phase_left_moving + 2;

      unsigned left_offset = phase - phase_left_stop;
      unsigned right_offset = (str_len - len) - (phase - phase_right_stop);

      // Ticker period: [Wait at left (2 ticks), Progress to right (type_len - w), Wait at right (2 ticks), Progress to left].
      if (phase < phase_left_stop)
         strlcpy(buf, str, len + 1);
      else if (phase < phase_left_moving)
         strlcpy(buf, str + left_offset, len + 1);
      else if (phase < phase_right_stop)
         strlcpy(buf, str + str_len - len, len + 1);
      else
         strlcpy(buf, str + right_offset, len + 1);
   }
}

uint64_t menu_input(void)
{
   unsigned p, i, players;
   uint64_t input_state = 0;

   static const struct retro_keybind *binds[MAX_PLAYERS] = { 
    g_settings.input.menu_binds,
    g_settings.input.menu_binds,
    g_settings.input.menu_binds,
    g_settings.input.menu_binds,
}; /* Make all controllers use the same menu binds */
   
   /* read all ports or port 1 only */
   players = g_settings.input.menu_all_players_enable ? MAX_PLAYERS : 1;

   for (p = 0; p < players; p++)
      for (i = 0; i < RETRO_DEVICE_ID_JOYPAD_R; i++)
         input_state |= input_input_state_func(binds, p, RETRO_DEVICE_JOYPAD, 0, i) ? (1ULL << i) : 0;

   input_state |= input_key_pressed_func(RARCH_MENU_TOGGLE) ? (1ULL << RARCH_MENU_TOGGLE) : 0;
   input_state |= input_key_pressed_func(RARCH_QUIT_KEY) ? (1ULL << RARCH_QUIT_KEY) : 0;
   
   /* Clear lifecycle_state from QUIT and MENU TOGGLE commands to avoid further triggering */
   g_extern.lifecycle_state &= ~(1ULL << RARCH_MENU_TOGGLE);
   g_extern.lifecycle_state &= ~(1ULL << RARCH_QUIT_KEY);

   rgui->trigger_state = input_state & ~rgui->old_input_state;

   rgui->do_held = (input_state & (
            (1ULL << RETRO_DEVICE_ID_JOYPAD_UP)
            | (1ULL << RETRO_DEVICE_ID_JOYPAD_DOWN)
            | (1ULL << RETRO_DEVICE_ID_JOYPAD_LEFT)
            | (1ULL << RETRO_DEVICE_ID_JOYPAD_RIGHT)
            | (1ULL << RETRO_DEVICE_ID_JOYPAD_L)
            | (1ULL << RETRO_DEVICE_ID_JOYPAD_R)
            )) && !(input_state & (1ULL << RARCH_MENU_TOGGLE));

   return input_state;
}

static int menu_custom_bind_iterate(void *data, void *video_data, unsigned action)
{
   rgui_handle_t *rgui = (rgui_handle_t*)data;
   (void)action;

   if (video_data && menugui_driver && menugui_driver->render)
      menugui_driver->render(rgui, video_data);

   char msg[256];
   const char *padname = g_settings.input.device_names[rgui->binds.port];
   const char *keyname = input_config_bind_map[rgui->binds.begin - RGUI_SETTINGS_BIND_BEGIN].desc;
   snprintf(msg, sizeof(msg), "[%s]\nPress a key on <%d:%s> to bind it\n(Press RESET button to abort)", 
            keyname, rgui->binds.port, padname);

   if (video_data && menugui_driver && menugui_driver->render_messagebox)
      menugui_driver->render_messagebox(rgui, video_data, msg);

   struct rgui_bind_state binds = rgui->binds;
   menu_poll_bind_state(&binds);

   if ((binds.abort) || menu_poll_find_trigger(&rgui->binds, &binds))
   {
      binds.begin++;
      if (binds.begin <= binds.last)
         binds.target++;
      else
         file_list_pop(rgui->menu_stack, &rgui->selection_ptr);
   }
   rgui->binds = binds;
   
   /* Avoid binds triggering while awaiting. */
   rgui->trigger_state = 0;
   rgui->old_input_state = -1ULL;

   return 0;
}

static int menu_start_screen_iterate(void *data, void *video_data, unsigned action)
{
   unsigned i;
   char msg[1024];
   rgui_handle_t *rgui = (rgui_handle_t*)data;

   if (video_data && menugui_driver && menugui_driver->render)
      menugui_driver->render(rgui, video_data);

   char desc[6][64];
   static const unsigned binds[] = {
      RETRO_DEVICE_ID_JOYPAD_UP,
      RETRO_DEVICE_ID_JOYPAD_DOWN,
      RETRO_DEVICE_ID_JOYPAD_A,
      RETRO_DEVICE_ID_JOYPAD_B,
      RARCH_MENU_TOGGLE,
      RARCH_QUIT_KEY,
   };

   for (i = 0; i < ARRAY_SIZE(binds); i++)
   {
      if (driver.input && driver.input->set_keybinds)
      {
         struct platform_bind key_label;
         strlcpy(key_label.desc, "Unknown", sizeof(key_label.desc));
         key_label.joykey = g_settings.input.binds[0][binds[i]].joykey;
         driver.input->set_keybinds(&key_label, 0, 0, 0, 1ULL << KEYBINDS_ACTION_GET_BIND_LABEL);
         strlcpy(desc[i], key_label.desc, sizeof(desc[i]));
      }
      else
      {
         const struct retro_keybind *bind = &g_settings.input.binds[0][binds[i]];
         input_get_bind_string(desc[i], bind, 0, sizeof(desc[i]));
      }
   }

   snprintf(msg, sizeof(msg),
         "-- Welcome to RetroArch / RGUI --\n"
         " \n" // strtok_r doesn't split empty strings.

         "Basic RGUI controls:\n"
         "    Scroll (Up): %-20s\n"
         "  Scroll (Down): %-20s\n"
         "      Accept/OK: %-20s\n"
         "           Back: %-20s\n"
         "Enter/Exit RGUI: %-20s\n"
         " Exit RetroArch: %-20s\n"
         " \n"

         "To run game:\n"
         "Load a libretro core (Core).\n"
         "Load a game file (Load Game).     \n"
         " \n"

         "See Path Options to set directories\n"
         "for faster access to files.\n"
         " \n"

         "Press Accept/OK to continue.",
         desc[0], desc[1], desc[2], desc[3], desc[4], desc[5]);

   if (video_data && menugui_driver && menugui_driver->render_messagebox)
      menugui_driver->render_messagebox(rgui, video_data, msg);

   if (action == RGUI_ACTION_OK)
      file_list_pop(rgui->menu_stack, &rgui->selection_ptr);
   return 0;
}

static int menu_settings_iterate(void *data, void *video_data, unsigned action)
{
   rgui_handle_t *rgui = (rgui_handle_t*)data;
   rgui->frame_buf_pitch = rgui->width * 2;
   unsigned type = 0;
   const char *label = NULL;
   if (action != RGUI_ACTION_REFRESH)
      file_list_get_at_offset(rgui->selection_buf, rgui->selection_ptr, &label, &type);

   if (type == RGUI_SETTINGS_CORE)
   {
#ifdef HAVE_LIBRETRO_MANAGEMENT
      label = default_paths.core_dir;
#else
      label = ""; // Shouldn't happen ...
#endif
   }
   else if (type == RGUI_SETTINGS_DISK_APPEND)
      label = g_settings.games_directory;

   const char *dir = NULL;
   unsigned menu_type = 0;
   file_list_get_last(rgui->menu_stack, &dir, &menu_type);

   if (rgui->need_refresh)
      action = RGUI_ACTION_NOOP;

   switch (action)
   {
      case RGUI_ACTION_UP:
         if (rgui->selection_ptr > 0)
            menu_decrement_navigation(rgui);
         else
            menu_set_navigation(rgui, rgui->selection_buf->size - 1);
         break;

      case RGUI_ACTION_DOWN:
         if (rgui->selection_ptr + 1 < rgui->selection_buf->size)
            menu_increment_navigation(rgui);
         else
            menu_clear_navigation(rgui);
         break;

      case RGUI_ACTION_CANCEL:
         if (rgui->menu_stack->size > 1)
         {
            file_list_pop(rgui->menu_stack, &rgui->selection_ptr);
            rgui->need_refresh = true;
         }
         break;

      case RGUI_ACTION_LEFT:
      case RGUI_ACTION_RIGHT:
      case RGUI_ACTION_OK:
      case RGUI_ACTION_START:
      case RGUI_ACTION_SELECT:
         if ((type == RGUI_SETTINGS_OPEN_FILEBROWSER || type == RGUI_SETTINGS_OPEN_FILEBROWSER_DEFERRED_CORE)
               && action == RGUI_ACTION_OK)
         {
            rgui->defer_core = type == RGUI_SETTINGS_OPEN_FILEBROWSER_DEFERRED_CORE;
            file_list_push(rgui->menu_stack, g_settings.games_directory, RGUI_FILE_DIRECTORY, rgui->selection_ptr);
            menu_clear_navigation(rgui);
            rgui->need_refresh = true;
         }
         else if ((type == RGUI_SETTINGS_OPEN_HISTORY || menu_type_is(type) == RGUI_FILE_DIRECTORY) && action == RGUI_ACTION_OK)
         {
            file_list_push(rgui->menu_stack, "", type, rgui->selection_ptr);
            menu_clear_navigation(rgui);
            rgui->need_refresh = true;
         }
         else if ((menu_type_is(type) == RGUI_SETTINGS || type == RGUI_SETTINGS_CORE || type == RGUI_SETTINGS_DISK_APPEND) && action == RGUI_ACTION_OK)
         {
            file_list_push(rgui->menu_stack, label, type, rgui->selection_ptr);
            menu_clear_navigation(rgui);
            rgui->need_refresh = true;
         }
         else
         {
            int ret = menu_settings_toggle_setting(rgui, video_data, type, action, menu_type);
            if (ret)
               return ret;
         }
         break;

      case RGUI_ACTION_REFRESH:
         menu_clear_navigation(rgui);
         rgui->need_refresh = true;
         break;

      case RGUI_ACTION_MESSAGE:
         rgui->msg_force = true;
         break;

      default:
         break;
   }

   file_list_get_last(rgui->menu_stack, &dir, &menu_type);

   if (rgui->need_refresh && !(menu_type == RGUI_FILE_DIRECTORY ||
            menu_type_is(menu_type) == RGUI_FILE_DIRECTORY ||
            menu_type == RGUI_SETTINGS_OVERLAY_PRESET ||
            menu_type == RGUI_SETTINGS_CORE ||
            menu_type == RGUI_SETTINGS_DISK_APPEND ||
            menu_type == RGUI_SETTINGS_OPEN_HISTORY))
   {
      rgui->need_refresh = false;
      if (
               menu_type == RGUI_SETTINGS_INPUT_OPTIONS
            || menu_type == RGUI_SETTINGS_PATH_OPTIONS
            || menu_type == RGUI_SETTINGS_OVERLAY_OPTIONS
            || menu_type == RGUI_SETTINGS_OPTIONS
            || menu_type == RGUI_SETTINGS_CORE_INFO
            || menu_type == RGUI_SETTINGS_CORE_OPTIONS
            || menu_type == RGUI_SETTINGS_AUDIO_OPTIONS
            || menu_type == RGUI_SETTINGS_DISK_OPTIONS
            || menu_type == RGUI_SETTINGS_CONFIG_OPTIONS
            || menu_type == RGUI_SETTINGS_SAVE_OPTIONS
            || menu_type == RGUI_SETTINGS_VIDEO_OPTIONS
            || menu_type == RGUI_SETTINGS_BIND_PLAYER_KEYS
            || menu_type == RGUI_SETTINGS_BIND_HOTKEYS
            )
         menu_populate_entries(rgui, menu_type);
      else
         menu_populate_entries(rgui, RGUI_SETTINGS);
   }

   if (video_data && menugui_driver && menugui_driver->render)
      menugui_driver->render(rgui, video_data);

   return 0;
}

static void menu_flush_stack_type(void *data, unsigned final_type)
{
   rgui_handle_t *rgui = (rgui_handle_t*)data;
   unsigned type;
   type = 0;
   rgui->need_refresh = true;
   file_list_get_last(rgui->menu_stack, NULL, &type);
   while (type != final_type)
   {
      file_list_pop(rgui->menu_stack, &rgui->selection_ptr);
      file_list_get_last(rgui->menu_stack, NULL, &type);
   }
}

static int menu_iterate_func(void *data, void *video_data, unsigned action)
{
   rgui_handle_t *rgui = (rgui_handle_t*)data;

   const char *dir = 0;
   unsigned menu_type = 0;
   file_list_get_last(rgui->menu_stack, &dir, &menu_type);
   int ret = 0;

   if (video_data && menugui_driver && menugui_driver->set_texture)
      menugui_driver->set_texture(rgui, video_data, false);

   if (menu_type == RGUI_HELP_SCREEN)
      return menu_start_screen_iterate(rgui, video_data, action);
   else if (menu_type_is(menu_type) == RGUI_SETTINGS)
      return menu_settings_iterate(rgui, video_data, action);
   else if (menu_type == RGUI_SETTINGS_CUSTOM_BIND)
      return menu_custom_bind_iterate(rgui, video_data, action);

   if (rgui->need_refresh && action != RGUI_ACTION_MESSAGE)
      action = RGUI_ACTION_NOOP;

   unsigned scroll_speed = (max(rgui->scroll_accel, 2) - 2) / 4 + 1;
   unsigned fast_scroll_speed = 4 + 4 * scroll_speed;

   switch (action)
   {
      case RGUI_ACTION_UP:
         if (rgui->selection_ptr >= scroll_speed)
            menu_set_navigation(rgui, rgui->selection_ptr - scroll_speed);
         else
            menu_set_navigation(rgui, rgui->selection_buf->size - 1);
         break;

      case RGUI_ACTION_DOWN:
         if (rgui->selection_ptr + scroll_speed < rgui->selection_buf->size)
            menu_set_navigation(rgui, rgui->selection_ptr + scroll_speed);
         else
            menu_clear_navigation(rgui);
         break;

      case RGUI_ACTION_LEFT:
         if (rgui->selection_ptr > fast_scroll_speed)
            menu_set_navigation(rgui, rgui->selection_ptr - fast_scroll_speed);
         else
            menu_clear_navigation(rgui);
         break;

      case RGUI_ACTION_RIGHT:
         if (rgui->selection_ptr + fast_scroll_speed < rgui->selection_buf->size)
            menu_set_navigation(rgui, rgui->selection_ptr + fast_scroll_speed);
         else
            menu_set_navigation_last(rgui);
         break;

      case RGUI_ACTION_SCROLL_UP:
         menu_descend_alphabet(rgui, &rgui->selection_ptr);
         break;
      case RGUI_ACTION_SCROLL_DOWN:
         menu_ascend_alphabet(rgui, &rgui->selection_ptr);
         break;

      case RGUI_ACTION_CANCEL:
         if (rgui->menu_stack->size > 1)
         {
            file_list_pop(rgui->menu_stack, &rgui->selection_ptr);
            rgui->need_refresh = true;
         }
         break;

      case RGUI_ACTION_OK:
      {
         if (rgui->selection_buf->size == 0)
            return 0;

         const char *path = 0;
         unsigned type = 0;
         file_list_get_at_offset(rgui->selection_buf, rgui->selection_ptr, &path, &type);

         if (
               menu_type_is(type) == RGUI_FILE_DIRECTORY ||
               type == RGUI_SETTINGS_OVERLAY_PRESET ||
               type == RGUI_SETTINGS_CORE ||
               type == RGUI_SETTINGS_DISK_APPEND ||
               type == RGUI_FILE_DIRECTORY)
         {
            char cat_path[PATH_MAX];
            fill_pathname_join(cat_path, dir, path, sizeof(cat_path));

            file_list_push(rgui->menu_stack, cat_path, type, rgui->selection_ptr);
            menu_clear_navigation(rgui);
            rgui->need_refresh = true;
         }
         else
         {
            if (menu_type == RGUI_SETTINGS_DEFERRED_CORE)
            {
               if (!load_menu_game_new_core(rgui->deferred_path, path))
                  menu_flush_stack_type(rgui, RGUI_SETTINGS);
               rgui->msg_force = true;
               ret = -1;
            }
            else if (menu_type == RGUI_SETTINGS_CORE)
            {
               rarch_environment_cb(RETRO_ENVIRONMENT_SET_LIBRETRO_PATH, (void*)path);

#ifdef HW_RVL
               fill_pathname_join(g_extern.fullpath, default_paths.core_dir,
                     SALAMANDER_FILE, sizeof(g_extern.fullpath));
#else
               fill_pathname_join(g_settings.libretro, dir, path, sizeof(g_settings.libretro));
#endif
               g_extern.lifecycle_state &= ~(1ULL << MODE_GAME);
               g_extern.lifecycle_state |= (1ULL << MODE_EXITSPAWN);
               ret = -1;

               menu_flush_stack_type(rgui, RGUI_SETTINGS);
            }
#ifdef HAVE_OVERLAY
            else if (menu_type == RGUI_SETTINGS_OVERLAY_PRESET)
            {
               fill_pathname_join(g_settings.input.overlay_path, dir, path, sizeof(g_settings.input.overlay_path));

               if (driver.overlay)
                  input_overlay_free(driver.overlay);
               driver.overlay = input_overlay_new(g_settings.input.overlay_path);
               if (!driver.overlay)
                  RARCH_ERR("Unable to load overlay.\n");

               menu_flush_stack_type(rgui, RGUI_SETTINGS_OVERLAY_OPTIONS);
            }
#endif
            else if (menu_type == RGUI_SETTINGS_DISK_APPEND)
            {
               char image[PATH_MAX];
               fill_pathname_join(image, dir, path, sizeof(image));
               rarch_disk_control_append_image(image);

               g_extern.lifecycle_state |= 1ULL << MODE_GAME;

               menu_flush_stack_type(rgui, RGUI_SETTINGS);
               ret = -1;
            }
            else if (menu_type == RGUI_SETTINGS_OPEN_HISTORY)
            {
               load_menu_game_history(rgui->selection_ptr);
               menu_flush_stack_type(rgui, RGUI_SETTINGS);
               ret = -1;
            }
            else if (menu_type == RGUI_GAMES_DIR_PATH)
            {
               strlcpy(g_settings.games_directory, dir, sizeof(g_settings.games_directory));
               menu_flush_stack_type(rgui, RGUI_SETTINGS_PATH_OPTIONS);
            }
#ifdef HAVE_SCREENSHOTS
            else if (menu_type == RGUI_SCREENSHOT_DIR_PATH)
            {
               strlcpy(g_settings.screenshot_directory, dir, sizeof(g_settings.screenshot_directory));
               menu_flush_stack_type(rgui, RGUI_SETTINGS_PATH_OPTIONS);
            }
#endif
            else if (menu_type == RGUI_SAVEFILE_DIR_PATH)
            {
               strlcpy(g_settings.savefile_directory, dir, sizeof(g_settings.savefile_directory));
               menu_flush_stack_type(rgui, RGUI_SETTINGS_PATH_OPTIONS);
            }
#ifdef HAVE_OVERLAY
            else if (menu_type == RGUI_OVERLAY_DIR_PATH)
            {
               strlcpy(g_settings.overlay_directory, dir, sizeof(g_settings.overlay_directory));
               menu_flush_stack_type(rgui, RGUI_SETTINGS_PATH_OPTIONS);
            }
#endif
            else if (menu_type == RGUI_SAVESTATE_DIR_PATH)
            {
               strlcpy(g_settings.savestate_directory, dir, sizeof(g_settings.savestate_directory));
               menu_flush_stack_type(rgui, RGUI_SETTINGS_PATH_OPTIONS);
            }
            else if (menu_type == RGUI_LIBRETRO_INFO_DIR_PATH)
            {
               strlcpy(g_settings.libretro_info_directory, dir, sizeof(g_settings.libretro_info_directory));
               menu_init_core_info(rgui);
               menu_flush_stack_type(rgui, RGUI_SETTINGS_PATH_OPTIONS);
            }
            else if (menu_type == RGUI_SYSTEM_DIR_PATH)
            {
               strlcpy(g_settings.system_directory, dir, sizeof(g_settings.system_directory));
               menu_flush_stack_type(rgui, RGUI_SETTINGS_PATH_OPTIONS);
            }
            else if (menu_type == RGUI_CONFIG_DIR_PATH)
            {
               strlcpy(g_settings.config_directory, dir, sizeof(g_settings.config_directory));
               menu_flush_stack_type(rgui, RGUI_SETTINGS_PATH_OPTIONS);
            }
            else
            {
               if (rgui->defer_core)
               {
                  fill_pathname_join(rgui->deferred_path, dir, path, sizeof(rgui->deferred_path));

                  const core_info_t *info = NULL;
                  size_t supported = 0;
                  if (rgui->core_info)
                     core_info_list_get_supported_cores(rgui->core_info, rgui->deferred_path, &info, &supported);

                  if (supported == 1) /* Can make a decision right now. */
                  {
                     if (!load_menu_game_new_core(rgui->deferred_path, info->path))
                        menu_flush_stack_type(rgui, RGUI_SETTINGS);
                     rgui->msg_force = true;
                     ret = -1;
                  }
                  else /* Present a selection. */
                  {
                     file_list_push(rgui->menu_stack, default_paths.core_dir, RGUI_SETTINGS_DEFERRED_CORE, rgui->selection_ptr);
                     menu_clear_navigation(rgui);
                     rgui->need_refresh = true;
                  }
               }
               else
               {
                  fill_pathname_join(g_extern.fullpath, dir, path, sizeof(g_extern.fullpath));
                  g_extern.lifecycle_state |= (1ULL << MODE_LOAD_GAME);

                  menu_flush_stack_type(rgui, RGUI_SETTINGS);
                  rgui->msg_force = true;
                  ret = -1;
               }
            }
         }
         break;
      }

      case RGUI_ACTION_REFRESH:
         menu_clear_navigation(rgui);
         rgui->need_refresh = true;
         break;

      case RGUI_ACTION_MESSAGE:
         rgui->msg_force = true;
         break;

      default:
         break;
   }


   // refresh values in case the stack changed
   file_list_get_last(rgui->menu_stack, &dir, &menu_type);

   if (rgui->need_refresh && (menu_type == RGUI_FILE_DIRECTORY ||
            menu_type_is(menu_type) == RGUI_FILE_DIRECTORY ||
            menu_type == RGUI_SETTINGS_OVERLAY_PRESET ||
            menu_type == RGUI_SETTINGS_DEFERRED_CORE ||
            menu_type == RGUI_SETTINGS_CORE ||
            menu_type == RGUI_SETTINGS_OPEN_HISTORY ||
            menu_type == RGUI_SETTINGS_DISK_APPEND))
   {
      rgui->need_refresh = false;
      menu_parse_and_resolve(rgui, menu_type);
   }

   if (video_data && menugui_driver && menugui_driver->render)
      menugui_driver->render(rgui, video_data);

   return ret;
}

bool menu_iterate(void *video_data)
{
   retro_time_t time, delta, target_msec, sleep_msec;
   unsigned action;
   static bool initial_held = true;
   static bool first_held = false;
   uint64_t input_state = 0;

   rarch_check_block_hotkey();
   rarch_input_poll();

   input_state = menu_input();

   if (rgui->do_held)
   {
      if (!first_held)
      {
         first_held = true;
         rgui->delay_timer = initial_held ? 12 : 6;
         rgui->delay_count = 0;
      }

      if (rgui->delay_count >= rgui->delay_timer)
      {
         first_held = false;
         rgui->trigger_state = input_state;
         rgui->scroll_accel = min(rgui->scroll_accel + 1, 64);
      }

      initial_held = false;
   }
   else
   {
      first_held = false;
      initial_held = true;
      rgui->scroll_accel = 0;
   }

   rgui->delay_count++;
   rgui->old_input_state = input_state;

   if (driver.block_input)
      rgui->trigger_state = 0;

   action = RGUI_ACTION_NOOP;

   // don't run anything first frame, only capture held inputs for old_input_state
   if (rgui->trigger_state & (1ULL << RETRO_DEVICE_ID_JOYPAD_UP))
      action = RGUI_ACTION_UP;
   else if (rgui->trigger_state & (1ULL << RETRO_DEVICE_ID_JOYPAD_DOWN))
      action = RGUI_ACTION_DOWN;
   else if (rgui->trigger_state & (1ULL << RETRO_DEVICE_ID_JOYPAD_LEFT))
      action = RGUI_ACTION_LEFT;
   else if (rgui->trigger_state & (1ULL << RETRO_DEVICE_ID_JOYPAD_RIGHT))
      action = RGUI_ACTION_RIGHT;
   else if (rgui->trigger_state & (1ULL << RETRO_DEVICE_ID_JOYPAD_L))
      action = RGUI_ACTION_SCROLL_UP;
   else if (rgui->trigger_state & (1ULL << RETRO_DEVICE_ID_JOYPAD_R))
      action = RGUI_ACTION_SCROLL_DOWN;
   else if (rgui->trigger_state & (1ULL << RETRO_DEVICE_ID_JOYPAD_B))
      action = RGUI_ACTION_CANCEL;
   else if (rgui->trigger_state & (1ULL << RETRO_DEVICE_ID_JOYPAD_A))
      action = RGUI_ACTION_OK;
   else if (rgui->trigger_state & (1ULL << RETRO_DEVICE_ID_JOYPAD_START))
      action = RGUI_ACTION_START;
   else if (rgui->trigger_state & (1ULL << RETRO_DEVICE_ID_JOYPAD_SELECT))
      action = RGUI_ACTION_SELECT;

   if (menugui_driver)
      if (menu_iterate_func(rgui, video_data, action))
         return false;

   if (video_data && driver.video_poke && driver.video_poke->set_texture_enable)
      driver.video_poke->set_texture_enable(video_data, rgui->frame_buf_show, false);

   rarch_render_cached_frame();

   // Throttle in case VSync is broken (avoid 1000+ FPS RGUI).
   time = rarch_get_time_usec();
   delta = (time - rgui->last_time) / 1000;
   target_msec = 750 / g_settings.video.refresh_rate; // Try to sleep less, so we can hopefully rely on FPS logger.
   sleep_msec = target_msec - delta;
   if (sleep_msec > 0)
      rarch_sleep((unsigned int)sleep_msec);
   rgui->last_time = rarch_get_time_usec();

   if (video_data && driver.video_poke && driver.video_poke->set_texture_enable)
      driver.video_poke->set_texture_enable(video_data, false, false);

   if (menugui_driver && menugui_driver->input_postprocess)
      if (menugui_driver->input_postprocess(rgui, rgui->old_input_state))
         return false;

   return true;
}

void menu_poll_bind_state(struct rgui_bind_state *state)
{
   unsigned p, b, a, h;
   memset(state->state, 0, sizeof(state->state));
   
   /* if MENU TOGGLE is pressed, abort. */
   state->abort = rgui->old_input_state & (1ULL << RARCH_MENU_TOGGLE);
   if (state->abort) return;

   const rarch_joypad_driver_t *joypad = NULL;
   if (driver.input && driver.input_data && driver.input->get_joypad_driver)
      joypad = driver.input->get_joypad_driver(driver.input_data);

   if (!joypad)
   {
      RARCH_ERR("Cannot poll raw joypad state.");
      return;
   }

   input_joypad_poll(joypad);
   p = state->port;
   
   for (b = 0; b < RGUI_MAX_BUTTONS; b++)
      state->state[p].buttons[b] = input_joypad_button_raw(joypad, p, b);
   for (a = 0; a < RGUI_MAX_AXES; a++)
      state->state[p].axes[a] = input_joypad_axis_raw(joypad, p, a);
   for (h = 0; h < RGUI_MAX_HATS; h++)
   {
      state->state[p].hats[h] |= input_joypad_hat_raw(joypad, p, HAT_UP_MASK, h) ? HAT_UP_MASK : 0;
      state->state[p].hats[h] |= input_joypad_hat_raw(joypad, p, HAT_DOWN_MASK, h) ? HAT_DOWN_MASK : 0;
      state->state[p].hats[h] |= input_joypad_hat_raw(joypad, p, HAT_LEFT_MASK, h) ? HAT_LEFT_MASK : 0;
      state->state[p].hats[h] |= input_joypad_hat_raw(joypad, p, HAT_RIGHT_MASK, h) ? HAT_RIGHT_MASK : 0;
   }
}

void menu_poll_bind_get_rested_axes(struct rgui_bind_state *state)
{
   unsigned i, a;
   const rarch_joypad_driver_t *joypad = NULL;
   if (driver.input && driver.input_data && driver.input->get_joypad_driver)
      joypad = driver.input->get_joypad_driver(driver.input_data);

   if (!joypad)
   {
      RARCH_ERR("Cannot poll raw joypad state.");
      return;
   }

   for (i = 0; i < MAX_PLAYERS; i++)
      for (a = 0; a < RGUI_MAX_AXES; a++)
         state->axis_state[i].rested_axes[a] = input_joypad_axis_raw(joypad, i, a);
}

bool menu_poll_find_trigger(struct rgui_bind_state *state, struct rgui_bind_state *new_state)
{
   unsigned a, b, h;
   unsigned p = state->port;
   const struct rgui_bind_state_port *n = &new_state->state[p];
   const struct rgui_bind_state_port *o = &state->state[p];

   for (b = 0; b < RGUI_MAX_BUTTONS; b++)
   {
      if (n->buttons[b] && !o->buttons[b])
      {
         state->target->joykey = b;
         state->target->joyaxis = AXIS_NONE;
         return true;
      }
   }

   // Axes are a bit tricky ...
   for (a = 0; a < RGUI_MAX_AXES; a++)
   {
      int locked_distance = abs(n->axes[a] - new_state->axis_state[p].locked_axes[a]);
      int rested_distance = abs(n->axes[a] - new_state->axis_state[p].rested_axes[a]);

      if (abs(n->axes[a]) >= 20000 &&
            locked_distance >= 20000 &&
            rested_distance >= 20000) // Take care of case where axis rests on +/- 0x7fff (e.g. 360 controller on Linux)
      {
         state->target->joyaxis = n->axes[a] > 0 ? AXIS_POS(a) : AXIS_NEG(a);
         state->target->joykey = NO_BTN;

         // Lock the current axis.
         new_state->axis_state[p].locked_axes[a] = n->axes[a] > 0 ? 0x7fff : -0x7fff;
         return true;
      }

      if (locked_distance >= 20000) // Unlock the axis.
         new_state->axis_state[p].locked_axes[a] = 0;
   }

   for (h = 0; h < RGUI_MAX_HATS; h++)
   {
      uint16_t trigged = n->hats[h] & (~o->hats[h]);
      uint16_t sane_trigger = 0;
      if (trigged & HAT_UP_MASK)
         sane_trigger = HAT_UP_MASK;
      else if (trigged & HAT_DOWN_MASK)
         sane_trigger = HAT_DOWN_MASK;
      else if (trigged & HAT_LEFT_MASK)
         sane_trigger = HAT_LEFT_MASK;
      else if (trigged & HAT_RIGHT_MASK)
         sane_trigger = HAT_RIGHT_MASK;

      if (sane_trigger)
      {
         state->target->joykey = HAT_MAP(h, sane_trigger);
         state->target->joyaxis = AXIS_NONE;
         return true;
      }
   }

   return false;
}

static inline int menu_list_get_first_char(file_list_t *buf, unsigned offset)
{
   const char *path = NULL;
   file_list_get_alt_at_offset(buf, offset, &path);
   int ret = tolower(*path);

   // "Normalize" non-alphabetical entries so they are lumped together for purposes of jumping.
   if (ret < 'a')
      ret = 'a' - 1;
   else if (ret > 'z')
      ret = 'z' + 1;
   return ret;
}

static inline bool menu_list_elem_is_dir(file_list_t *buf, unsigned offset)
{
   const char *path = NULL;
   unsigned type = 0;
   file_list_get_at_offset(buf, offset, &path, &type);
   return type != RGUI_FILE_PLAIN;
}

static void menu_build_scroll_indices(void *data, file_list_t *buf)
{
   size_t i;
   int current;
   bool current_is_dir;
   rgui_handle_t *rgui = (rgui_handle_t*)data;

   rgui->scroll_indices_size = 0;
   if (!buf->size)
      return;

   rgui->scroll_indices[rgui->scroll_indices_size++] = 0;

   current = menu_list_get_first_char(buf, 0);
   current_is_dir = menu_list_elem_is_dir(buf, 0);

   for (i = 1; i < buf->size; i++)
   {
      int first;
      bool is_dir;

      first = menu_list_get_first_char(buf, i);
      is_dir = menu_list_elem_is_dir(buf, i);

      if ((current_is_dir && !is_dir) || (first > current))
         rgui->scroll_indices[rgui->scroll_indices_size++] = i;

      current = first;
      current_is_dir = is_dir;
   }

   rgui->scroll_indices[rgui->scroll_indices_size++] = buf->size - 1;
}

void menu_populate_entries(void *data, unsigned menu_type)
{
   rgui_handle_t *rgui = (rgui_handle_t*)data;
   unsigned i;
   char tmp[256];
   switch (menu_type)
   {
      case RGUI_SETTINGS_CONFIG_OPTIONS:
         file_list_clear(rgui->selection_buf);
         file_list_push(rgui->selection_buf, "Save Config On Exit [G]", RGUI_SETTINGS_CONFIG_SAVE_ON_EXIT, 0);
         file_list_push(rgui->selection_buf, "Active Config Type", RGUI_SETTINGS_CONFIG_TYPE, 0);
         file_list_push(rgui->selection_buf, "Per-Game Config", RGUI_SETTINGS_CONFIG_SAVE_GAME_SPECIFIC, 0);
         break;
      case RGUI_SETTINGS_SAVE_OPTIONS:
         file_list_clear(rgui->selection_buf);
         file_list_push(rgui->selection_buf, "Rewind", RGUI_SETTINGS_REWIND_ENABLE, 0);
         file_list_push(rgui->selection_buf, "Rewind Granularity", RGUI_SETTINGS_REWIND_GRANULARITY, 0);
         file_list_push(rgui->selection_buf, "SRAM Block Overwrite", RGUI_SETTINGS_BLOCK_SRAM_OVERWRITE, 0);
         file_list_push(rgui->selection_buf, "Savestate Autosave On Exit", RGUI_SETTINGS_SAVESTATE_AUTO_SAVE, 0);
         file_list_push(rgui->selection_buf, "Savestate Autoload", RGUI_SETTINGS_SAVESTATE_AUTO_LOAD, 0); 
      break;
      case RGUI_SETTINGS_VIDEO_OPTIONS:
         file_list_clear(rgui->selection_buf);
         file_list_push(rgui->selection_buf, "Game Resolution", RGUI_SETTINGS_VIDEO_RESOLUTION, 0);
         if (!g_extern.video.using_component)
            file_list_push(rgui->selection_buf, "Interlaced Resolution Only", RGUI_SETTINGS_VIDEO_INTERLACED_ONLY, 0);
         file_list_push(rgui->selection_buf, "Screen Position X", RGUI_SETTINGS_VIDEO_SCREEN_POS_X, 0);
         file_list_push(rgui->selection_buf, "Screen Position Y", RGUI_SETTINGS_VIDEO_SCREEN_POS_Y, 0);
         file_list_push(rgui->selection_buf, "Show Framerate [G]", RGUI_SETTINGS_SHOW_FRAMERATE, 0);
#ifdef HAVE_SCALERS_BUILTIN
         file_list_push(rgui->selection_buf, "Soft Scaling", RGUI_SETTINGS_VIDEO_SOFT_SCALER, 0);
#endif
         file_list_push(rgui->selection_buf, "Bilinear Filtering", RGUI_SETTINGS_VIDEO_BILINEAR, 0);
#ifdef HW_RVL
         file_list_push(rgui->selection_buf, "VI Trap filtering", RGUI_SETTINGS_VIDEO_VITRAP_FILTER, 0);
         file_list_push(rgui->selection_buf, "Gamma", RGUI_SETTINGS_VIDEO_GAMMA, 0);
#endif
         file_list_push(rgui->selection_buf, "VSync", RGUI_SETTINGS_VIDEO_VSYNC, 0);
         file_list_push(rgui->selection_buf, "Crop Overscan", RGUI_SETTINGS_VIDEO_CROP_OVERSCAN, 0);
         file_list_push(rgui->selection_buf, "Aspect Ratio", RGUI_SETTINGS_VIDEO_ASPECT_RATIO, 0);
         if (g_settings.video.aspect_ratio_idx == ASPECT_RATIO_CUSTOM)
         {
            file_list_push(rgui->selection_buf, "Custom Viewport X", RGUI_SETTINGS_CUSTOM_VIEWPORT_X, 0);
            file_list_push(rgui->selection_buf, "Custom Viewport Y", RGUI_SETTINGS_CUSTOM_VIEWPORT_Y, 0);
            file_list_push(rgui->selection_buf, "Custom Viewport Width", RGUI_SETTINGS_CUSTOM_VIEWPORT_WIDTH, 0);
            file_list_push(rgui->selection_buf, "Custom Viewport Height", RGUI_SETTINGS_CUSTOM_VIEWPORT_HEIGHT, 0);
         }
         file_list_push(rgui->selection_buf, "Integer Scale", RGUI_SETTINGS_VIDEO_INTEGER_SCALE, 0);
         file_list_push(rgui->selection_buf, "Force Aspect", RGUI_SETTINGS_VIDEO_FORCE_ASPECT, 0);
         file_list_push(rgui->selection_buf, "Rotation", RGUI_SETTINGS_VIDEO_ROTATION, 0);
         break;
      case RGUI_SETTINGS_CORE_OPTIONS:
         file_list_clear(rgui->selection_buf);

         if (g_extern.system.core_options)
         {
            size_t i, opts;

            opts = core_option_size(g_extern.system.core_options);
            for (i = 0; i < opts; i++)
               file_list_push(rgui->selection_buf,
                     core_option_get_desc(g_extern.system.core_options, i), RGUI_SETTINGS_CORE_OPTION_START + i, 0);
         }
         else
            file_list_push(rgui->selection_buf, "Load a game first.", RGUI_SETTINGS_CORE_OPTION_NONE, 0);
         break;
      case RGUI_SETTINGS_CORE_INFO:
         file_list_clear(rgui->selection_buf);
         if (rgui->core_info_current.data)
         {
            snprintf(tmp, sizeof(tmp), "Core name: %s",
                  rgui->core_info_current.display_name ? rgui->core_info_current.display_name : "");
            file_list_push(rgui->selection_buf, tmp, RGUI_SETTINGS_CORE_INFO_NONE, 0);

            snprintf(tmp, sizeof(tmp), "Running game: %s",
                  *g_extern.basename ? path_basename(g_extern.basename) : "None");
            file_list_push(rgui->selection_buf, tmp, RGUI_SETTINGS_CORE_INFO_NONE, 0);

            if (rgui->core_info_current.authors_list)
            {
               strlcpy(tmp, "Authors: ", sizeof(tmp));
               string_list_join_concat(tmp, sizeof(tmp), rgui->core_info_current.authors_list, ", ");
               file_list_push(rgui->selection_buf, tmp, RGUI_SETTINGS_CORE_INFO_NONE, 0);
            }

            if (rgui->core_info_current.permissions_list)
            {
               strlcpy(tmp, "Permissions: ", sizeof(tmp));
               string_list_join_concat(tmp, sizeof(tmp), rgui->core_info_current.permissions_list, ", ");
               file_list_push(rgui->selection_buf, tmp, RGUI_SETTINGS_CORE_INFO_NONE, 0);
            }

            if (rgui->core_info_current.supported_extensions_list)
            {
               strlcpy(tmp, "Supported extensions: ", sizeof(tmp));
               string_list_join_concat(tmp, sizeof(tmp), rgui->core_info_current.supported_extensions_list, ", ");
               file_list_push(rgui->selection_buf, tmp, RGUI_SETTINGS_CORE_INFO_NONE, 0);
            }

            if (rgui->core_info_current.firmware_count > 0)
            {
               core_info_list_update_missing_firmware(rgui->core_info, rgui->core_info_current.path,
                     g_settings.system_directory);

               file_list_push(rgui->selection_buf, "Firmware: ", RGUI_SETTINGS_CORE_INFO_NONE, 0);
               for (i = 0; i < rgui->core_info_current.firmware_count; i++)
               {
                  if (rgui->core_info_current.firmware[i].desc)
                  {
                     snprintf(tmp, sizeof(tmp), "	name: %s",
                           rgui->core_info_current.firmware[i].desc ? rgui->core_info_current.firmware[i].desc : "");
                     file_list_push(rgui->selection_buf, tmp, RGUI_SETTINGS_CORE_INFO_NONE, 0);

                     snprintf(tmp, sizeof(tmp), "	status: %s, %s",
                           rgui->core_info_current.firmware[i].missing ? "missing" : "present",
                           rgui->core_info_current.firmware[i].optional ? "optional" : "required");
                     file_list_push(rgui->selection_buf, tmp, RGUI_SETTINGS_CORE_INFO_NONE, 0);
                  }
               }
            }

            if (rgui->core_info_current.notes)
            {
               snprintf(tmp, sizeof(tmp), "Core notes: ");
               file_list_push(rgui->selection_buf, tmp, RGUI_SETTINGS_CORE_INFO_NONE, 0);

               for (i = 0; i < rgui->core_info_current.note_list->size; i++)
               {
                  snprintf(tmp, sizeof(tmp), " %s", rgui->core_info_current.note_list->elems[i].data);
                  file_list_push(rgui->selection_buf, tmp, RGUI_SETTINGS_CORE_INFO_NONE, 0);
               }
            }
         }
         else
            file_list_push(rgui->selection_buf, "No information available.", RGUI_SETTINGS_CORE_OPTION_NONE, 0);
         break;
      case RGUI_SETTINGS_OPTIONS:
         file_list_clear(rgui->selection_buf);
         file_list_push(rgui->selection_buf, "Video", RGUI_SETTINGS_VIDEO_OPTIONS, 0);
         file_list_push(rgui->selection_buf, "Input", RGUI_SETTINGS_INPUT_OPTIONS, 0);
         file_list_push(rgui->selection_buf, "Audio", RGUI_SETTINGS_AUDIO_OPTIONS, 0);
#ifdef HAVE_OVERLAY
         file_list_push(rgui->selection_buf, "Overlay", RGUI_SETTINGS_OVERLAY_OPTIONS, 0);
#endif
         file_list_push(rgui->selection_buf, "Configs", RGUI_SETTINGS_CONFIG_OPTIONS, 0);
         file_list_push(rgui->selection_buf, "Saves", RGUI_SETTINGS_SAVE_OPTIONS, 0);
         file_list_push(rgui->selection_buf, "Paths", RGUI_SETTINGS_PATH_OPTIONS, 0);
         if (g_extern.main_is_init && !g_extern.libretro_dummy)
         {
            if (g_extern.system.disk_control.get_num_images)
               file_list_push(rgui->selection_buf, "Disks", RGUI_SETTINGS_DISK_OPTIONS, 0);
         }
         file_list_push(rgui->selection_buf, "Help", RGUI_HELP_SCREEN, 0);
         break;
      case RGUI_SETTINGS_DISK_OPTIONS:
         file_list_clear(rgui->selection_buf);
         file_list_push(rgui->selection_buf, "Disk Index", RGUI_SETTINGS_DISK_INDEX, 0);
         file_list_push(rgui->selection_buf, "Disk Image Append", RGUI_SETTINGS_DISK_APPEND, 0);
         break;
      case RGUI_SETTINGS_OVERLAY_OPTIONS:
         file_list_clear(rgui->selection_buf);
         file_list_push(rgui->selection_buf, "Overlay Preset", RGUI_SETTINGS_OVERLAY_PRESET, 0);
         file_list_push(rgui->selection_buf, "Overlay Opacity", RGUI_SETTINGS_OVERLAY_OPACITY, 0);
         file_list_push(rgui->selection_buf, "Overlay Scale", RGUI_SETTINGS_OVERLAY_SCALE, 0);
         break;
      case RGUI_SETTINGS_PATH_OPTIONS:
         file_list_clear(rgui->selection_buf);
         file_list_push(rgui->selection_buf, "Games Path [G]", RGUI_GAMES_DIR_PATH, 0);
         file_list_push(rgui->selection_buf, "Core Info Path [G]", RGUI_LIBRETRO_INFO_DIR_PATH, 0);
         file_list_push(rgui->selection_buf, "Specif Conf Path [G]", RGUI_CONFIG_DIR_PATH, 0);
         file_list_push(rgui->selection_buf, "Savestate Path [G]", RGUI_SAVESTATE_DIR_PATH, 0);
         file_list_push(rgui->selection_buf, "Savefile Path [G]", RGUI_SAVEFILE_DIR_PATH, 0);
#ifdef HAVE_OVERLAY
         file_list_push(rgui->selection_buf, "Overlay Path [G]", RGUI_OVERLAY_DIR_PATH, 0);
#endif
         file_list_push(rgui->selection_buf, "System Path [G]", RGUI_SYSTEM_DIR_PATH, 0);
#ifdef HAVE_SCREENSHOTS
         file_list_push(rgui->selection_buf, "Screenshot Path [G]", RGUI_SCREENSHOT_DIR_PATH, 0);
#endif
         break;
      case RGUI_SETTINGS_INPUT_OPTIONS:
         file_list_clear(rgui->selection_buf);
         file_list_push(rgui->selection_buf, "Input Type (Restart) [G]", RGUI_SETTINGS_INPUT_TYPE, 0);
         file_list_push(rgui->selection_buf, "All P. Control Menu [G]", RGUI_SETTINGS_MENU_ALL_PLAYERS_ENABLE, 0);
         file_list_push(rgui->selection_buf, "Autoconfig Buttons", RGUI_SETTINGS_DEVICE_AUTOCONF_BUTTONS, 0);
         file_list_push(rgui->selection_buf, "Quick Swap", RGUI_SETTINGS_QUICK_SWAP_PLAYERS, 0);
         file_list_push(rgui->selection_buf, "Bind Player Keys", RGUI_SETTINGS_BIND_PLAYER_KEYS, 0);
         file_list_push(rgui->selection_buf, "Bind Hotkeys", RGUI_SETTINGS_BIND_HOTKEYS, 0);
         /* init the selected device to the current player's device */
         rgui->s_device = g_settings.input.device_port[rgui->c_player];
         break;
      case RGUI_SETTINGS_BIND_PLAYER_KEYS:
         file_list_clear(rgui->selection_buf);
         file_list_push(rgui->selection_buf, "Player", RGUI_SETTINGS_BIND_PLAYER, 0);
         file_list_push(rgui->selection_buf, "Controller", RGUI_SETTINGS_BIND_DEVICE, 0);
         file_list_push(rgui->selection_buf, "Device Type", RGUI_SETTINGS_BIND_DEVICE_TYPE, 0);
         file_list_push(rgui->selection_buf, "Analog D-pad Mode", RGUI_SETTINGS_BIND_ANALOG_MODE, 0);
         file_list_push(rgui->selection_buf, "Reset All Buttons", RGUI_SETTINGS_BIND_DEFAULT_ALL, 0);
         for (i = RGUI_SETTINGS_BIND_BEGIN; i <= RGUI_SETTINGS_BIND_TURBO_ENABLE; i++)
            file_list_push(rgui->selection_buf, input_config_bind_map[i - RGUI_SETTINGS_BIND_BEGIN].desc, i, 0);
         break;
      case RGUI_SETTINGS_BIND_HOTKEYS:
         file_list_clear(rgui->selection_buf);
         for (i = RGUI_SETTINGS_BIND_FAST_FORWARD_KEY; i <= RGUI_SETTINGS_BIND_MENU_TOGGLE; i++)
            file_list_push(rgui->selection_buf, input_config_bind_map[i - RGUI_SETTINGS_BIND_BEGIN].desc, i, 0);
         break;                  
      case RGUI_SETTINGS_AUDIO_OPTIONS:
         file_list_clear(rgui->selection_buf);
         file_list_push(rgui->selection_buf, "Mute Audio [G]", RGUI_SETTINGS_AUDIO_MUTE, 0);
         file_list_push(rgui->selection_buf, "Audio Sync", RGUI_SETTINGS_AUDIO_SYNC, 0);
         file_list_push(rgui->selection_buf, "Rate Control Delta", RGUI_SETTINGS_AUDIO_CONTROL_RATE_DELTA, 0);
         file_list_push(rgui->selection_buf, "Volume Level", RGUI_SETTINGS_AUDIO_VOLUME, 0);
         break;
      case RGUI_SETTINGS:
         file_list_clear(rgui->selection_buf);

         if (g_extern.main_is_init && !g_extern.libretro_dummy)
         {
            file_list_push(rgui->selection_buf, "Resume Game", RGUI_SETTINGS_RESUME_GAME, 0);
            file_list_push(rgui->selection_buf, "Restart Game", RGUI_SETTINGS_RESTART_GAME, 0);
            file_list_push(rgui->selection_buf, "Core Setup", RGUI_SETTINGS_CORE_OPTIONS, 0);
            file_list_push(rgui->selection_buf, "Save State", RGUI_SETTINGS_SAVESTATE_SAVE, 0);
            file_list_push(rgui->selection_buf, "Load State", RGUI_SETTINGS_SAVESTATE_LOAD, 0);
#ifdef HAVE_SCREENSHOTS
            file_list_push(rgui->selection_buf, "Take Screenshot", RGUI_SETTINGS_SCREENSHOT, 0);
#endif
            /* Add a menu separator */
            file_list_push(rgui->selection_buf, "------------------------------", RGUI_SETTINGS_CORE_OPTION_NONE, 0);
         }
         if (rgui->history)
            file_list_push(rgui->selection_buf, "Load Recent", RGUI_SETTINGS_OPEN_HISTORY, 0);

         if (rgui->core_info && core_info_list_num_info_files(rgui->core_info))
            file_list_push(rgui->selection_buf, "Detect Game", RGUI_SETTINGS_OPEN_FILEBROWSER_DEFERRED_CORE, 0);

         if (rgui->info.library_name || g_extern.system.info.library_name)
         {
            char load_game_core_msg[64];
            snprintf(load_game_core_msg, sizeof(load_game_core_msg), "Load Game (%s)",
                  rgui->info.library_name ? rgui->info.library_name : g_extern.system.info.library_name);
            file_list_push(rgui->selection_buf, load_game_core_msg, RGUI_SETTINGS_OPEN_FILEBROWSER, 0);
         }

#if defined(HAVE_LIBRETRO_MANAGEMENT)
         file_list_push(rgui->selection_buf, "Select Core", RGUI_SETTINGS_CORE, 0);
#endif
         file_list_push(rgui->selection_buf, "Core Information", RGUI_SETTINGS_CORE_INFO, 0);
         file_list_push(rgui->selection_buf, "Settings", RGUI_SETTINGS_OPTIONS, 0);
         file_list_push(rgui->selection_buf, "Restart RetroArch", RGUI_SETTINGS_RESTART_RARCH, 0);
         file_list_push(rgui->selection_buf, "Quit RetroArch", RGUI_SETTINGS_QUIT_RARCH, 0);
         break;
   }
}

static void menu_parse_and_resolve(void *data, unsigned menu_type)
{
   const core_info_t *info = NULL;
   const char *dir;
   size_t i, list_size;
   file_list_t *list;
   rgui_handle_t *rgui;

   rgui = (rgui_handle_t*)data;
   dir = NULL;

   file_list_clear(rgui->selection_buf);

   // parsing switch
   switch (menu_type)
   {
      case RGUI_SETTINGS_OPEN_HISTORY:
         /* History parse */
         list_size = rom_history_size(rgui->history);

         for (i = 0; i < list_size; i++)
         {
            const char *path, *core_path, *core_name;
            char fill_buf[PATH_MAX];

            path = NULL;
            core_path = NULL;
            core_name = NULL;

            rom_history_get_index(rgui->history, i,
                  &path, &core_path, &core_name);

            if (path)
            {
               char path_short[PATH_MAX];
               fill_pathname(path_short, path_basename(path), "", sizeof(path_short));

               snprintf(fill_buf, sizeof(fill_buf), "%s (%s)",
                     path_short, core_name);
            }
            else
               strlcpy(fill_buf, core_name, sizeof(fill_buf));

            file_list_push(rgui->selection_buf, fill_buf, RGUI_FILE_PLAIN, 0);
         }
         break;
      case RGUI_SETTINGS_DEFERRED_CORE:
         break;
      default:
         {
            /* Directory parse */
            file_list_get_last(rgui->menu_stack, &dir, &menu_type);

            if (!*dir)
            {
#ifdef HW_RVL
               file_list_push(rgui->selection_buf, "sd:/", menu_type, 0);
               file_list_push(rgui->selection_buf, "usb:/", menu_type, 0);
#endif
               file_list_push(rgui->selection_buf, "carda:/", menu_type, 0);
               file_list_push(rgui->selection_buf, "cardb:/", menu_type, 0);
               return;
            }
#ifdef HW_RVL
            LWP_MutexLock(gx_device_mutex);
            int dev = gx_get_device_from_path(dir);

            if (dev != -1 && !gx_devices[dev].mounted && gx_devices[dev].interface->isInserted())
               fatMountSimple(gx_devices[dev].name, gx_devices[dev].interface);

            LWP_MutexUnlock(gx_device_mutex);
#endif

            const char *exts;
            char ext_buf[1024];
            if (menu_type == RGUI_SETTINGS_CORE)
               exts = EXT_EXECUTABLES;
            else if (menu_type == RGUI_SETTINGS_OVERLAY_PRESET)
               exts = "cfg";
            else if (menu_type_is(menu_type) == RGUI_FILE_DIRECTORY)
               exts = ""; // we ignore files anyway
            else if (rgui->defer_core)
               exts = rgui->core_info ? core_info_list_get_all_extensions(rgui->core_info) : "";
            else if (rgui->info.valid_extensions)
            {
               exts = ext_buf;
               if (*rgui->info.valid_extensions)
                  snprintf(ext_buf, sizeof(ext_buf), "%s|zip", rgui->info.valid_extensions);
               else
                  *ext_buf = '\0';
            }
            else
               exts = g_extern.system.valid_extensions;

            struct string_list *list = dir_list_new(dir, exts, true);
            if (!list)
               return;

            dir_list_sort(list, true);

            if (menu_type_is(menu_type) == RGUI_FILE_DIRECTORY)
               file_list_push(rgui->selection_buf, "<Use this directory>", RGUI_FILE_USE_DIRECTORY, 0);

            for (i = 0; i < list->size; i++)
            {
               bool is_dir = list->elems[i].attr.b;

               if ((menu_type_is(menu_type) == RGUI_FILE_DIRECTORY) && !is_dir)
                  continue;

               // Need to preserve slash first time.
               const char *path = list->elems[i].data;
               if (*dir)
                  path = path_basename(path);

#ifdef HAVE_LIBRETRO_MANAGEMENT
               if (menu_type == RGUI_SETTINGS_CORE && (is_dir || strcasecmp(path, SALAMANDER_FILE) == 0))
                  continue;
#endif

               // Push menu_type further down in the chain.
               // Needed for shader manager currently.
               file_list_push(rgui->selection_buf, path,
                     is_dir ? menu_type : RGUI_FILE_PLAIN, 0);
            }

            string_list_free(list);
         }
   }

   // resolving switch
   switch (menu_type)
   {
      case RGUI_SETTINGS_CORE:
         dir = NULL;
         list = (file_list_t*)rgui->selection_buf;
         file_list_get_last(rgui->menu_stack, &dir, &menu_type);
         list_size = list->size;
         for (i = 0; i < list_size; i++)
         {
            const char *path;
            unsigned type = 0;
            file_list_get_at_offset(list, i, &path, &type);
            if (type != RGUI_FILE_PLAIN)
               continue;

            char core_path[PATH_MAX];
            fill_pathname_join(core_path, dir, path, sizeof(core_path));

            char display_name[256];
            if (rgui->core_info &&
                  core_info_list_get_display_name(rgui->core_info,
                     core_path, display_name, sizeof(display_name)))
               file_list_set_alt_at_offset(list, i, display_name);
         }
         file_list_sort_on_alt(rgui->selection_buf);
         break;
      case RGUI_SETTINGS_DEFERRED_CORE:
         core_info_list_get_supported_cores(rgui->core_info, rgui->deferred_path, &info, &list_size);
         for (i = 0; i < list_size; i++)
         {
            file_list_push(rgui->selection_buf, info[i].path, RGUI_FILE_PLAIN, 0);
            file_list_set_alt_at_offset(rgui->selection_buf, i, info[i].display_name);
         }
         file_list_sort_on_alt(rgui->selection_buf);
         break;
      default:
         (void)0;
   }

   rgui->scroll_indices_size = 0;
   if (menu_type != RGUI_SETTINGS_OPEN_HISTORY)
      menu_build_scroll_indices(rgui, rgui->selection_buf);

   // Before a refresh, we could have deleted a file on disk, causing
   // selection_ptr to suddendly be out of range. Ensure it doesn't overflow.
   if (rgui->selection_ptr >= rgui->selection_buf->size && rgui->selection_buf->size)
      menu_set_navigation(rgui, rgui->selection_buf->size - 1);
   else if (!rgui->selection_buf->size)
      menu_clear_navigation(rgui);
}

void menu_init_core_info(void *data)
{
   rgui_handle_t *rgui = (rgui_handle_t*)data;

   core_info_list_free(rgui->core_info);
   rgui->core_info = NULL;
   if (*default_paths.core_dir)
      rgui->core_info = core_info_list_new(default_paths.core_dir);
   menu_update_system_info(rgui, NULL);
}
