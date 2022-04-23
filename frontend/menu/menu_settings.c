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

#include <stdint.h>
#include <string.h>
#include "../../file.h"
#include "menu_common.h"
#include "menu_navigation.h"
#include "../../gfx/gfx_common.h"
#include "../../input/input_common.h"
#include "../../config.def.h"

#define MAX_GAMMA_SETTING 2

unsigned menu_type_is(unsigned type)
{
   unsigned ret = 0;
   bool type_found;

   type_found =
      type == RGUI_SETTINGS ||
      type == RGUI_SETTINGS_CONFIG_OPTIONS ||
      type == RGUI_SETTINGS_SAVE_OPTIONS ||
      type == RGUI_SETTINGS_CORE_OPTIONS ||
      type == RGUI_SETTINGS_SYSTEM_INFO ||
      type == RGUI_SETTINGS_VIDEO_OPTIONS ||
      type == RGUI_SETTINGS_AUDIO_OPTIONS ||
      type == RGUI_SETTINGS_DISK_OPTIONS ||
      type == RGUI_SETTINGS_PATH_OPTIONS ||
      type == RGUI_SETTINGS_OVERLAY_OPTIONS ||
      type == RGUI_SETTINGS_OPTIONS ||
      type == RGUI_SETTINGS_BIND_PLAYER_KEYS ||
      type == RGUI_SETTINGS_BIND_HOTKEYS ||
      type == RGUI_SETTINGS_INPUT_OPTIONS ||
      type == RGUI_SETTINGS_MENU_OPTIONS;

   if (type_found)
   {
      ret = RGUI_SETTINGS;
      return ret;
   }

   type_found = type == RGUI_GAMES_DIR_PATH ||
      type == RGUI_SAVESTATE_DIR_PATH ||
      type == RGUI_LIBRETRO_INFO_DIR_PATH ||
      type == RGUI_CONFIG_DIR_PATH ||
      type == RGUI_SAVEFILE_DIR_PATH ||
      type == RGUI_OVERLAY_DIR_PATH ||
      type == RGUI_SCREENSHOT_DIR_PATH ||
      type == RGUI_SYSTEM_DIR_PATH;

   if (type_found)
   {
      ret = RGUI_FILE_DIRECTORY;
      return ret;
   }

   return ret;
}

static int menu_core_setting_toggle(unsigned setting, unsigned action)
{
   unsigned index = setting - RGUI_SETTINGS_CORE_OPTION_START;
   switch (action)
   {
      case RGUI_ACTION_LEFT:
         core_option_prev(g_extern.system.core_options, index);
         break;

      case RGUI_ACTION_RIGHT:
      case RGUI_ACTION_OK:
         core_option_next(g_extern.system.core_options, index);
         break;

      case RGUI_ACTION_START:
         core_option_set_default(g_extern.system.core_options, index);
         break;

      default:
         break;
   }

   return 0;
}

int menu_settings_toggle_setting(void *data, void *video_data, unsigned setting, unsigned action, unsigned menu_type)
{
   rgui_handle_t *rgui = (rgui_handle_t*)data;

   if (setting >= RGUI_SETTINGS_CORE_OPTION_START)
      return menu_core_setting_toggle(setting, action);

   return menu_set_settings(rgui, video_data, setting, action);
}

void update_config_params()
{
   config_load();
   rarch_reset_drivers();
}

static void set_custom_bind(unsigned port, unsigned setting, unsigned action)
{
   struct retro_keybind *bind = &g_settings.input.binds[port][setting - RGUI_SETTINGS_BIND_BEGIN];
   
   if (g_settings.input.device_names[port][0]=='\0')
   {
      char msg[255];
      snprintf(msg, sizeof(msg), "Binding disabled on <%d:No Device>\nAssign a device first", port+1);
      msg_queue_push(g_extern.msg_queue, msg, 1, 90);
   }
   else if (action == RGUI_ACTION_OK)
   {
      rgui->binds.begin = setting;
      rgui->binds.last = setting;
      rgui->binds.target = bind;
      rgui->binds.port = port;
      file_list_push(rgui->menu_stack, "", RGUI_SETTINGS_CUSTOM_BIND, rgui->selection_ptr);
      menu_poll_bind_get_rested_axes(&rgui->binds);
      menu_poll_bind_state(&rgui->binds);
   }
   else if (action == RGUI_ACTION_START)
   {
      bind->joykey = NO_BTN;
      bind->joyaxis = AXIS_NONE;
   }
   else if (action == RGUI_ACTION_SELECT)
   {
      if (driver.input && driver.input->set_keybinds)
            driver.input->set_keybinds(NULL, 0, port, setting - RGUI_SETTINGS_BIND_BEGIN, (1ULL << KEYBINDS_ACTION_SET_DEFAULT_BIND));
   }
}

int menu_set_settings(void *data, void *video_data, unsigned setting, unsigned action)
{
   rgui_handle_t *rgui = (rgui_handle_t*)data;

   switch (setting)
   {
      case RGUI_SETTINGS_REWIND_ENABLE:
         if (action == RGUI_ACTION_OK ||
               action == RGUI_ACTION_LEFT ||
               action == RGUI_ACTION_RIGHT)
         {
            g_settings.rewind_enable = !g_settings.rewind_enable;
            if (g_settings.rewind_enable)
               rarch_init_rewind();
            else
               rarch_deinit_rewind();
         }
         else if (action == RGUI_ACTION_START)
         {
            g_settings.rewind_enable = DEFAULT_REWIND_ENABLE;
            rarch_deinit_rewind();
         }
         break;
      case RGUI_SETTINGS_REWIND_GRANULARITY:
         if (action == RGUI_ACTION_OK || action == RGUI_ACTION_RIGHT)
            g_settings.rewind_granularity++;
         else if (action == RGUI_ACTION_LEFT)
         {
            if (g_settings.rewind_granularity > 1)
               g_settings.rewind_granularity--;
         }
         else if (action == RGUI_ACTION_START)
            g_settings.rewind_granularity = DEFAULT_REWIND_GRANULARITY;
         break;
      case RGUI_SETTINGS_CONFIG_SAVE_ON_EXIT:
         if (action == RGUI_ACTION_OK || action == RGUI_ACTION_RIGHT
               || action == RGUI_ACTION_LEFT)
            g_settings.config_save_on_exit = !g_settings.config_save_on_exit;
         else if (action == RGUI_ACTION_START)
            g_settings.config_save_on_exit = DEFAULT_CONFIG_SAVE_ON_EXIT;
         break;
      case RGUI_SETTINGS_SAVESTATE_AUTO_SAVE:
         if (action == RGUI_ACTION_OK || action == RGUI_ACTION_RIGHT
               || action == RGUI_ACTION_LEFT)
            g_settings.savestate_auto_save = !g_settings.savestate_auto_save;
         else if (action == RGUI_ACTION_START)
            g_settings.savestate_auto_save = DEFAULT_SAVESTATE_AUTO_SAVE;
         break;
      case RGUI_SETTINGS_SAVESTATE_AUTO_LOAD:
         if (action == RGUI_ACTION_OK || action == RGUI_ACTION_RIGHT
               || action == RGUI_ACTION_LEFT)
            g_settings.savestate_auto_load = !g_settings.savestate_auto_load;
         else if (action == RGUI_ACTION_START)
            g_settings.savestate_auto_load = DEFAULT_SAVESTATE_AUTO_LOAD;
         break;
      case RGUI_SETTINGS_BLOCK_SRAM_OVERWRITE:
         if (action == RGUI_ACTION_OK || action == RGUI_ACTION_RIGHT
               || action == RGUI_ACTION_LEFT)
            g_settings.block_sram_overwrite = !g_settings.block_sram_overwrite;
         else if (action == RGUI_ACTION_START)
            g_settings.block_sram_overwrite = DEFAULT_BLOCK_SRAM_OVERWRITE;
         break;
      case RGUI_SETTINGS_CONFIG_SAVE_GAME_SPECIFIC:
         if (action == RGUI_ACTION_OK) 
         {
            if (*g_extern.basename && g_extern.config_type != CONFIG_PER_GAME)
            {
               /* Calculate the game specific config path */
               path_basedir(g_extern.specific_config_path);
               fill_pathname_dir(g_extern.specific_config_path, g_extern.basename, ".cfg", sizeof(g_extern.specific_config_path));
               RARCH_LOG("Saving game-specific config from: %s.\n", g_extern.specific_config_path);            
               if (config_save_file(g_extern.specific_config_path))
               {
                  g_extern.config_type = CONFIG_PER_GAME;
                  char msg[64];
                  snprintf(msg, sizeof(msg), "Game-specific config successfully created.\n");
                  msg_queue_push(g_extern.msg_queue, msg, 0, 80);
               }
            }
         }
         else if (action == RGUI_ACTION_START)
         {
            if (g_extern.config_type == CONFIG_PER_GAME)
            {
               /* Remove game specific config file, */
               RARCH_LOG("Removing game-specific config from: %s.\n", g_extern.specific_config_path);            
               remove(g_extern.specific_config_path);
               *g_extern.specific_config_path = '\0';
               /* and load per-core config if available.*/
               update_config_params();
            }
         }
         break;
      case RGUI_SETTINGS_SAVESTATE_SAVE:
      case RGUI_SETTINGS_SAVESTATE_LOAD:
         if (action == RGUI_ACTION_OK)
         {
            if (setting == RGUI_SETTINGS_SAVESTATE_SAVE)
               rarch_save_state();
            else
               rarch_load_state();
            g_extern.lifecycle_state |= (1ULL << MODE_GAME);
            return -1;
         }
         else if (action == RGUI_ACTION_START)
            g_settings.state_slot = DEFAULT_SAVESTATE_SLOT;
         else if (action == RGUI_ACTION_LEFT)
         {
            // Slot -1 is (auto) slot.
            if (g_settings.state_slot >= 0)
               g_settings.state_slot--;
         }
         else if (action == RGUI_ACTION_RIGHT)
            g_settings.state_slot++;
         break;
#ifdef HAVE_SCREENSHOTS
      case RGUI_SETTINGS_SCREENSHOT:
         if (action == RGUI_ACTION_OK)
            rarch_take_screenshot();
         break;
#endif
      case RGUI_SETTINGS_RESTART_GAME:
         if (action == RGUI_ACTION_OK)
         {
            rarch_game_reset();
            g_extern.lifecycle_state |= (1ULL << MODE_GAME);
            return -1;
         }
         break;
      case RGUI_SETTINGS_AUDIO_MUTE:
         if (action == RGUI_ACTION_START)
            g_settings.audio.mute = DEFAULT_AUDIO_MUTE;
         else if (action == RGUI_ACTION_OK || action == RGUI_ACTION_RIGHT
               || action == RGUI_ACTION_LEFT)
            g_settings.audio.mute = !g_settings.audio.mute;
         break;
      case RGUI_SETTINGS_AUDIO_SYNC:
         if (action == RGUI_ACTION_START)
            g_settings.audio.sync = DEFAULT_AUDIO_AUDIO_SYNC;
         else if (action == RGUI_ACTION_OK || action == RGUI_ACTION_RIGHT
               || action == RGUI_ACTION_LEFT)
            g_settings.audio.sync = !g_settings.audio.sync;
         break;
      case RGUI_SETTINGS_AUDIO_CONTROL_RATE_DELTA:
         if (action == RGUI_ACTION_START)
         {
            g_settings.audio.rate_control_delta = DEFAULT_AUDIO_RATE_CONTROL_DELTA;
            g_settings.audio.rate_control = DEFAULT_AUDIO_RATE_CONTROL;
         }
         else if (action == RGUI_ACTION_LEFT)
         {
            if (g_settings.audio.rate_control_delta > 0.0)
               g_settings.audio.rate_control_delta -= 0.001;

            if (g_settings.audio.rate_control_delta < 0.0005)
            {
               g_settings.audio.rate_control = false;
               g_settings.audio.rate_control_delta = 0.0;
            }
            else
               g_settings.audio.rate_control = true;
         }
         else if (action == RGUI_ACTION_RIGHT || action == RGUI_ACTION_OK)
         {
            if (g_settings.audio.rate_control_delta < 0.2)
               g_settings.audio.rate_control_delta += 0.001;
            g_settings.audio.rate_control = true;
         }
         break;
      case RGUI_SETTINGS_AUDIO_VOLUME:
      {
         float db_delta = 0.0f;
         if (action == RGUI_ACTION_START)
         {
            g_extern.audio_data.volume_db = DEFAULT_AUDIO_VOLUME;
            g_extern.audio_data.volume_gain = db_to_gain(DEFAULT_AUDIO_VOLUME);
         }
         else if (action == RGUI_ACTION_LEFT)
            db_delta -= 1.0f;
         else if (action == RGUI_ACTION_RIGHT || action == RGUI_ACTION_OK)
            db_delta += 1.0f;

         if (db_delta != 0.0f)
         {
            g_extern.audio_data.volume_db += db_delta;
            g_extern.audio_data.volume_db = max(g_extern.audio_data.volume_db, -80.0f);
            g_extern.audio_data.volume_db = min(g_extern.audio_data.volume_db, 12.0f);
            g_extern.audio_data.volume_gain = db_to_gain(g_extern.audio_data.volume_db);
         }
         break;
      }
      case RGUI_SETTINGS_SHOW_FRAMERATE:
         if (action == RGUI_ACTION_START)
            g_settings.fps_show = DEFAULT_VIDEO_SHOW_FRAMERATE;
         else if (action == RGUI_ACTION_OK || action == RGUI_ACTION_RIGHT
               || action == RGUI_ACTION_LEFT)
            g_settings.fps_show = !g_settings.fps_show;
         break;
      case RGUI_SETTINGS_DISK_INDEX:
         {
            const struct retro_disk_control_callback *control = &g_extern.system.disk_control;

            unsigned num_disks = control->get_num_images();
            unsigned current   = control->get_image_index();

            int step = 0;
            if (action == RGUI_ACTION_RIGHT || action == RGUI_ACTION_OK)
               step = 1;
            else if (action == RGUI_ACTION_LEFT)
               step = -1;

            if (step)
            {
               unsigned next_index = (current + num_disks + 1 + step) % (num_disks + 1);
               rarch_disk_control_set_eject(true, false);
               rarch_disk_control_set_index(next_index);
               rarch_disk_control_set_eject(false, false);
            }

            break;
         }
      case RGUI_SETTINGS_RESTART_RARCH:
         if (action == RGUI_ACTION_OK)
         {
#ifdef HW_RVL
            fill_pathname_join(g_extern.fullpath, default_paths.core_dir, SALAMANDER_FILE,
                  sizeof(g_extern.fullpath));
            char path[MAX_LEN];
            strlcpy(path, path_basename(g_settings.libretro), sizeof(path));
            rarch_environment_cb(RETRO_ENVIRONMENT_SET_LIBRETRO_PATH, (void*)path);
#endif
            g_extern.lifecycle_state &= ~(1ULL << MODE_GAME);
            g_extern.lifecycle_state |= (1ULL << MODE_EXITSPAWN);
            return -1;
         }
         break;
      case RGUI_SETTINGS_RESUME_GAME:
         if (action == RGUI_ACTION_OK)
         {
            g_extern.lifecycle_state |= (1ULL << MODE_GAME);
            return -1;
         }
         break;
      case RGUI_SETTINGS_QUIT_RARCH:
         if (action == RGUI_ACTION_OK)
         {
            g_extern.lifecycle_state &= ~(1ULL << MODE_GAME);
            return -1;
         }
         break;
#ifdef HAVE_OVERLAY
      case RGUI_SETTINGS_OVERLAY_PRESET:
         switch (action)
         {
            case RGUI_ACTION_OK:
               file_list_push(rgui->menu_stack, g_settings.overlay_directory, setting, rgui->selection_ptr);
               menu_clear_navigation(rgui);
               rgui->need_refresh = true;
               break;

            case RGUI_ACTION_START:
               if (driver.overlay)
                  input_overlay_free(driver.overlay);
               driver.overlay = NULL;
               *g_settings.input.overlay_path = DEFAULT_INPUT_OVERLAY_PATH;
               break;

            default:
               break;
         }
         break;

      case RGUI_SETTINGS_OVERLAY_OPACITY:
         {
            bool changed = true;
            switch (action)
            {
               case RGUI_ACTION_LEFT:
                  g_settings.input.overlay_opacity -= 0.01f;

                  if (g_settings.input.overlay_opacity < 0.0f)
                     g_settings.input.overlay_opacity = 0.0f;
                  break;

               case RGUI_ACTION_RIGHT:
               case RGUI_ACTION_OK:
                  g_settings.input.overlay_opacity += 0.01f;

                  if (g_settings.input.overlay_opacity > 1.0f)
                     g_settings.input.overlay_opacity = 1.0f;
                  break;

               case RGUI_ACTION_START:
                  g_settings.input.overlay_opacity = DEFAULT_INPUT_OVERLAY_OPACITY;
                  break;

               default:
                  changed = false;
                  break;
            }

            if (changed && driver.overlay)
               input_overlay_set_alpha_mod(driver.overlay,
                     g_settings.input.overlay_opacity);
            break;
         }

      case RGUI_SETTINGS_OVERLAY_SCALE:
         {
            bool changed = true;
            switch (action)
            {
               case RGUI_ACTION_LEFT:
                  g_settings.input.overlay_scale -= 0.01f;

                  if (g_settings.input.overlay_scale < 0.01f) // Avoid potential divide by zero.
                     g_settings.input.overlay_scale = 0.01f;
                  break;

               case RGUI_ACTION_RIGHT:
               case RGUI_ACTION_OK:
                  g_settings.input.overlay_scale += 0.01f;

                  if (g_settings.input.overlay_scale > 2.0f)
                     g_settings.input.overlay_scale = 2.0f;
                  break;

               case RGUI_ACTION_START:
                  g_settings.input.overlay_scale = DEFAULT_INPUT_OVERLAY_SCALE;
                  break;

               default:
                  changed = false;
                  break;
            }

            if (changed && driver.overlay)
               input_overlay_set_scale_factor(driver.overlay,
                     g_settings.input.overlay_scale);
            break;
         }
#endif
#ifdef HAVE_SCALERS_BUILTIN
      case RGUI_SETTINGS_VIDEO_SOFT_SCALER:
         switch (action)
         {
            case RGUI_ACTION_LEFT:
               if (g_settings.video.filter_idx > 0)
                  g_settings.video.filter_idx--;
               break;
            case RGUI_ACTION_RIGHT:
               if ((g_settings.video.filter_idx + 1) != softfilter_get_last_idx())
                  g_settings.video.filter_idx++;
               break;
            case RGUI_ACTION_START: /* this falls thru OK action */
               g_settings.video.filter_idx = DEFAULT_VIDEO_FILTER_IDX;
            case RGUI_ACTION_OK:
            {
               rarch_reset_drivers();
               
               /* needed to get new frame size after filter so we refresh size. */
               driver.video_poke->set_texture_enable(video_data, true, false);             
               rarch_render_cached_frame();
               
               gfx_match_resolution_auto();
               
               char msg[48] = "Soft scaler removed";
               const char *filter_name = rarch_softfilter_get_name(g_settings.video.filter_idx);
               if (filter_name)
                  snprintf(msg, sizeof(msg), "%s applied", filter_name);
               msg_queue_push(g_extern.msg_queue, msg, 1, 90);
               break;
            }
         }
         break;
#endif        
      /* controllers */
      case RGUI_SETTINGS_BIND_PLAYER:
      {
         unsigned o_player = rgui->c_player;
         if (action == RGUI_ACTION_LEFT)
            rgui->c_player = (rgui->c_player + MAX_PLAYERS - 1) % MAX_PLAYERS;
         else if (action == RGUI_ACTION_RIGHT || action == RGUI_ACTION_OK)
            rgui->c_player = (rgui->c_player + 1) % MAX_PLAYERS;

         if (o_player != rgui->c_player)
         {
            /* set the selected device to the selected player´s device */
            rgui->s_device = g_settings.input.device_port[rgui->c_player];
            rgui->need_refresh = true;
         }
         break;
      }
      case RGUI_SETTINGS_BIND_DEVICE:
         if (action == RGUI_ACTION_START)
            rgui->s_device = g_settings.input.device_port[rgui->c_player];
         else if (action == RGUI_ACTION_LEFT)
            rgui->s_device = (rgui->s_device + MAX_PLAYERS - 1) % MAX_PLAYERS;
         else if (action == RGUI_ACTION_RIGHT)
            rgui->s_device = (rgui->s_device + 1) % MAX_PLAYERS;
         else if (action == RGUI_ACTION_OK)
         {
            unsigned o_device = g_settings.input.device_port[rgui->c_player];
            if (o_device != rgui->s_device)
            {
               unsigned p;
               for (p = 0; rgui->s_device != g_settings.input.device_port[p] && p < MAX_PLAYERS; p++);
               if (p != rgui->c_player && p < MAX_PLAYERS)
               {
                  g_settings.input.device_port[rgui->c_player] = rgui->s_device;
                  g_settings.input.device_port[p] = o_device;
                  
                  char msg[50];
                  snprintf(msg, sizeof(msg), "Player %d and Player %d controllers swapped", rgui->c_player+1, p+1);
                  msg_queue_push(g_extern.msg_queue, msg, 1, 90);                  
               }
            }
         }
         break;
      case RGUI_SETTINGS_BIND_ANALOG_MODE:
      {
         unsigned port = g_settings.input.device_port[rgui->c_player];
         switch (action)
         {
            case RGUI_ACTION_START:
               g_settings.input.analog_dpad_mode[port] = ANALOG_DPAD_NONE;
               break;

            case RGUI_ACTION_OK:
            case RGUI_ACTION_RIGHT:
               g_settings.input.analog_dpad_mode[port] = (g_settings.input.analog_dpad_mode[port] + 1) % ANALOG_DPAD_LAST;
               break;

            case RGUI_ACTION_LEFT:
               g_settings.input.analog_dpad_mode[port] = (g_settings.input.analog_dpad_mode[port] + ANALOG_DPAD_LAST - 1) % ANALOG_DPAD_LAST;
               break;

            default:
               break;
         }
         break;
      }
      case RGUI_SETTINGS_BIND_DEVICE_TYPE:
      {
         unsigned current_device, current_index, i;
         unsigned types = 0;
         unsigned devices[128];

         devices[types++] = RETRO_DEVICE_NONE;
         devices[types++] = RETRO_DEVICE_JOYPAD;
         devices[types++] = RETRO_DEVICE_ANALOG;

         const struct retro_controller_info *desc = rgui->c_player < g_extern.system.num_ports ? &g_extern.system.ports[rgui->c_player] : NULL;
         if (desc)
         {
            for (i = 0; i < desc->num_types; i++)
            {
               unsigned id = desc->types[i].id;
               if (types < ARRAY_SIZE(devices) && id != RETRO_DEVICE_NONE && id != RETRO_DEVICE_JOYPAD && id != RETRO_DEVICE_ANALOG)
                  devices[types++] = id;
            }
         }

         current_device = g_settings.input.libretro_device[rgui->c_player];
         current_index = 0;
         for (i = 0; i < types; i++)
         {
            if (current_device == devices[i])
            {
               current_index = i;
               break;
            }
         }

         bool updated = true;
         switch (action)
         {
            case RGUI_ACTION_START:
               current_device = RETRO_DEVICE_JOYPAD;
               break;

            case RGUI_ACTION_LEFT:
               current_device = devices[(current_index + types - 1) % types];
               break;

            case RGUI_ACTION_RIGHT:
            case RGUI_ACTION_OK:
               current_device = devices[(current_index + 1) % types];
               break;

            default:
               updated = false;
         }

         if (updated)
         {
            g_settings.input.libretro_device[rgui->c_player] = current_device;
            pretro_set_controller_port_device(rgui->c_player, current_device);
         }

         break;
      }
      case RGUI_SETTINGS_DEVICE_AUTOCONF_BUTTONS:
         if (action == RGUI_ACTION_OK || action == RGUI_ACTION_RIGHT || action == RGUI_ACTION_LEFT)
            g_settings.input.autoconf_buttons = !g_settings.input.autoconf_buttons;
         else if (action == RGUI_ACTION_START)
            g_settings.input.autoconf_buttons = DEFAULT_INPUT_AUTOCONF_BUTTONS;
         break;
      case RGUI_SETTINGS_MENU_ALL_PLAYERS_ENABLE:
         if (action == RGUI_ACTION_OK || action == RGUI_ACTION_RIGHT || action == RGUI_ACTION_LEFT)
            g_settings.input.menu_all_players_enable = !g_settings.input.menu_all_players_enable;
         else if (action == RGUI_ACTION_START)
            g_settings.input.menu_all_players_enable = DEFAULT_MENU_ALL_PLAYERS_ENABLE;
         break;
      case RGUI_SETTINGS_QUICK_SWAP_PLAYERS:
         if (action == RGUI_ACTION_RIGHT || action == RGUI_ACTION_OK)
            g_settings.input.quick_swap_players += g_settings.input.quick_swap_players < MAX_PLAYERS ? 1 : 0;
         else if (action == RGUI_ACTION_LEFT)
            g_settings.input.quick_swap_players -= g_settings.input.quick_swap_players > 1 ? 1 : 0;
         
         /* If we disable it, we force a swap to return to player 1 */
         if (g_settings.input.quick_swap_players == 1) quick_swap_controllers();
         break;
      case RGUI_SETTINGS_BIND_DEFAULT_ALL:
         if (action == RGUI_ACTION_OK)
         {
            unsigned i, port = g_settings.input.device_port[rgui->c_player];

            if (driver.input && driver.input->set_keybinds)
               for (i = 0; i < RARCH_CUSTOM_BIND_LIST_END; i++)
                  driver.input->set_keybinds(NULL, 0, port, i, (1ULL << KEYBINDS_ACTION_SET_DEFAULT_BIND));
         }
         break;
      case RGUI_SETTINGS_BIND_FAST_FORWARD_KEY:
      case RGUI_SETTINGS_BIND_FAST_FORWARD_HOLD_KEY:
      case RGUI_SETTINGS_BIND_LOAD_STATE_KEY:
      case RGUI_SETTINGS_BIND_SAVE_STATE_KEY:
      case RGUI_SETTINGS_BIND_QUIT_KEY:
      case RGUI_SETTINGS_BIND_STATE_SLOT_PLUS:
      case RGUI_SETTINGS_BIND_STATE_SLOT_MINUS:
      case RGUI_SETTINGS_BIND_REWIND:
      case RGUI_SETTINGS_BIND_PAUSE_TOGGLE:
      case RGUI_SETTINGS_BIND_FRAMEADVANCE:
      case RGUI_SETTINGS_BIND_RESET:
      case RGUI_SETTINGS_BIND_SCREENSHOT:
      case RGUI_SETTINGS_BIND_MUTE:
      case RGUI_SETTINGS_BIND_SLOWMOTION:
      case RGUI_SETTINGS_BIND_ENABLE_HOTKEY:
      case RGUI_SETTINGS_BIND_VOLUME_UP:
      case RGUI_SETTINGS_BIND_VOLUME_DOWN:
      case RGUI_SETTINGS_BIND_DISK_EJECT_TOGGLE:
      case RGUI_SETTINGS_BIND_DISK_NEXT:
      case RGUI_SETTINGS_BIND_QUICK_SWAP:
      case RGUI_SETTINGS_BIND_MENU_TOGGLE:
         /* hotkeys use first controller, no matter the player */
         set_custom_bind(0, setting, action);
         break;
      case RGUI_SETTINGS_BIND_UP:
      case RGUI_SETTINGS_BIND_DOWN:
      case RGUI_SETTINGS_BIND_LEFT:
      case RGUI_SETTINGS_BIND_RIGHT:
      case RGUI_SETTINGS_BIND_A:
      case RGUI_SETTINGS_BIND_B:
      case RGUI_SETTINGS_BIND_X:
      case RGUI_SETTINGS_BIND_Y:
      case RGUI_SETTINGS_BIND_START:
      case RGUI_SETTINGS_BIND_SELECT:
      case RGUI_SETTINGS_BIND_L:
      case RGUI_SETTINGS_BIND_R:
      case RGUI_SETTINGS_BIND_L2:
      case RGUI_SETTINGS_BIND_R2:
      case RGUI_SETTINGS_BIND_L3:
      case RGUI_SETTINGS_BIND_R3:
      case RGUI_SETTINGS_BIND_ANALOG_LEFT_X_PLUS:
      case RGUI_SETTINGS_BIND_ANALOG_LEFT_X_MINUS:
      case RGUI_SETTINGS_BIND_ANALOG_LEFT_Y_PLUS:
      case RGUI_SETTINGS_BIND_ANALOG_LEFT_Y_MINUS:
      case RGUI_SETTINGS_BIND_ANALOG_RIGHT_X_PLUS:
      case RGUI_SETTINGS_BIND_ANALOG_RIGHT_X_MINUS:
      case RGUI_SETTINGS_BIND_ANALOG_RIGHT_Y_PLUS:
      case RGUI_SETTINGS_BIND_ANALOG_RIGHT_Y_MINUS:
      case RGUI_SETTINGS_BIND_TURBO_ENABLE:
      {
         unsigned port = g_settings.input.device_port[rgui->c_player];
         set_custom_bind(port, setting, action);
         break;
      }
      case RGUI_GAMES_DIR_PATH:
         if (action == RGUI_ACTION_START)
            *g_settings.games_directory = DEFAULT_DIRECTORY_LOCATION;
         break;
#ifdef HAVE_SCREENSHOTS
      case RGUI_SCREENSHOT_DIR_PATH:
         if (action == RGUI_ACTION_START)
            *g_settings.screenshot_directory = DEFAULT_DIRECTORY_LOCATION;
         break;
#endif
      case RGUI_SAVEFILE_DIR_PATH:
         if (action == RGUI_ACTION_START)
            strlcpy(g_settings.savefile_directory, default_paths.sram_dir, sizeof(g_settings.savefile_directory));
         break;
#ifdef HAVE_OVERLAY
      case RGUI_OVERLAY_DIR_PATH:
         if (action == RGUI_ACTION_START)
            *g_settings.overlay_directory = DEFAULT_DIRECTORY_LOCATION;
         break;
#endif
      case RGUI_SAVESTATE_DIR_PATH:
         if (action == RGUI_ACTION_START)
            strlcpy(g_settings.savestate_directory, default_paths.savestate_dir, sizeof(g_settings.savestate_directory));
         break;
      case RGUI_LIBRETRO_INFO_DIR_PATH:
         if (action == RGUI_ACTION_START)
         {
            *g_settings.libretro_info_directory = DEFAULT_DIRECTORY_LOCATION;
            menu_init_core_info(rgui);
         }
         break;
      case RGUI_CONFIG_DIR_PATH:
         if (action == RGUI_ACTION_START)
            *g_settings.config_directory = DEFAULT_DIRECTORY_LOCATION;
         break;
      case RGUI_SYSTEM_DIR_PATH:
         if (action == RGUI_ACTION_START)
            strlcpy(g_settings.system_directory, default_paths.system_dir, sizeof(g_settings.system_directory));
         break;
      case RGUI_SETTINGS_VIDEO_ROTATION:
         if (action == RGUI_ACTION_START)
            g_settings.video.rotation = DEFAULT_VIDEO_ROTATION;
         else if (action == RGUI_ACTION_LEFT)
         {
            if (g_settings.video.rotation > 0) g_settings.video.rotation--;
         }
         else if (action == RGUI_ACTION_RIGHT || action == RGUI_ACTION_OK)
         {
            if (g_settings.video.rotation < LAST_ORIENTATION) g_settings.video.rotation++;
         }
         break;
      case RGUI_SETTINGS_MENU_THEME:
         if (action == RGUI_ACTION_START)
            g_settings.menu.theme = DEFAULT_MENU_THEME;
         else if (action == RGUI_ACTION_LEFT)
         {
            if (g_settings.menu.theme > 0) g_settings.menu.theme--;
         }
         else if (action == RGUI_ACTION_RIGHT || action == RGUI_ACTION_OK)
         {
            if (g_settings.menu.theme < LAST_THEME) g_settings.menu.theme++;
         }
         break;
      case RGUI_SETTINGS_MENU_ROTATION:
         if (action == RGUI_ACTION_START)
            g_settings.menu.rotation = DEFAULT_MENU_ROTATION;
         else if (action == RGUI_ACTION_LEFT)
         {
            if (g_settings.menu.rotation > 0) g_settings.menu.rotation--;
         }
         else if (action == RGUI_ACTION_RIGHT || action == RGUI_ACTION_OK)
         {
            if (g_settings.menu.rotation < LAST_MENU_ORIENTATION) g_settings.menu.rotation++;
         }
         /* Apply the new orientation */
         menugui_driver->set_size(rgui);
         video_set_rotation_func(g_settings.menu.rotation);
         break;
      case RGUI_SETTINGS_VIDEO_BILINEAR:
         if (action == RGUI_ACTION_START)
            g_settings.video.bilinear_filter = DEFAULT_VIDEO_BILINEAR_FILTER;
         else if (action == RGUI_ACTION_OK || action == RGUI_ACTION_RIGHT
               || action == RGUI_ACTION_LEFT)
            g_settings.video.bilinear_filter = !g_settings.video.bilinear_filter;
         break;
      case RGUI_SETTINGS_MENU_BILINEAR:
         if (action == RGUI_ACTION_START)
            g_settings.menu.bilinear_filter = DEFAULT_MENU_BILINEAR_FILTER;
         else if (action == RGUI_ACTION_OK || action == RGUI_ACTION_RIGHT
               || action == RGUI_ACTION_LEFT)
            g_settings.menu.bilinear_filter = !g_settings.menu.bilinear_filter;
         
         driver.video_poke->force_viewport_refresh(video_data);
         break;
      case RGUI_SETTINGS_INPUT_TYPE:
         if (action == RGUI_ACTION_LEFT)
            find_prev_input_driver();
         else if (action == RGUI_ACTION_RIGHT || action == RGUI_ACTION_OK)
            find_next_input_driver();
         break;
      case RGUI_SETTINGS_VIDEO_GAMMA:
         if (action == RGUI_ACTION_START)
            g_settings.video.gamma_correction = DEFAULT_VIDEO_GAMMA;
         else if (action == RGUI_ACTION_LEFT)
         {
            if (g_settings.video.gamma_correction > 0)
               g_settings.video.gamma_correction--;
         }
         else if (action == RGUI_ACTION_RIGHT || action == RGUI_ACTION_OK)
         {
            if (g_settings.video.gamma_correction < MAX_GAMMA_SETTING)
               g_settings.video.gamma_correction++;
         }
         
         driver.video_poke->apply_state_changes(video_data);   
         break;
      case RGUI_SETTINGS_VIDEO_INTEGER_SCALE:
         if (action == RGUI_ACTION_START)
            g_settings.video.scale_integer = DEFAULT_VIDEO_SCALE_INTEGER;
         else if (action == RGUI_ACTION_LEFT ||
               action == RGUI_ACTION_RIGHT ||
               action == RGUI_ACTION_OK)
            g_settings.video.scale_integer = !g_settings.video.scale_integer;
         break;
      case RGUI_SETTINGS_VIDEO_FORCE_ASPECT:
         if (action == RGUI_ACTION_START)
            g_settings.video.force_aspect = DEFAULT_VIDEO_FORCE_ASPECT;
         else if (action == RGUI_ACTION_LEFT ||
               action == RGUI_ACTION_RIGHT ||
               action == RGUI_ACTION_OK)
            g_settings.video.force_aspect = !g_settings.video.force_aspect;
         
         driver.video_poke->apply_state_changes(video_data);          
         break;
      case RGUI_SETTINGS_VIDEO_ASPECT_RATIO:
      {
         unsigned old_aspect_ratio_idx = g_settings.video.aspect_ratio_idx;
         
         if (action == RGUI_ACTION_START)
            g_settings.video.aspect_ratio_idx = DEFAULT_VIDEO_ASPECT_RATIO_IDX;
         else if (action == RGUI_ACTION_LEFT)
         {
            if (g_settings.video.aspect_ratio_idx > 0)
               g_settings.video.aspect_ratio_idx--;
         }
         else if (action == RGUI_ACTION_RIGHT || action == RGUI_ACTION_OK)
         {
            if (g_settings.video.aspect_ratio_idx < LAST_ASPECT_RATIO)
               g_settings.video.aspect_ratio_idx++;
         }
         
         if (old_aspect_ratio_idx != g_settings.video.aspect_ratio_idx)
            rgui->need_refresh = true;
         break;
      }
      case RGUI_SETTINGS_CUSTOM_VIEWPORT_X:
         if (action == RGUI_ACTION_START)
         {
            unsigned w, h, t;
            driver.video_poke->get_resolution_info(video_data, g_settings.video.resolution_idx, &w, &h, &t);
            g_settings.video.custom_vp.x = (w - g_settings.video.custom_vp.width) / 2;
         }
         else if (action == RGUI_ACTION_LEFT)
               g_settings.video.custom_vp.x--;
         else if (action == RGUI_ACTION_RIGHT || action == RGUI_ACTION_OK)
               g_settings.video.custom_vp.x++;

         break;
      case RGUI_SETTINGS_CUSTOM_VIEWPORT_Y:
         if (action == RGUI_ACTION_START)
         {
            unsigned w, h, t;
            driver.video_poke->get_resolution_info(video_data, g_settings.video.resolution_idx, &w, &h, &t);
            g_settings.video.custom_vp.y = (h - g_settings.video.custom_vp.height) / 2;
         }
         else if (action == RGUI_ACTION_LEFT)
               g_settings.video.custom_vp.y--;
         else if (action == RGUI_ACTION_RIGHT || action == RGUI_ACTION_OK)
               g_settings.video.custom_vp.y++;

         break;
      case RGUI_SETTINGS_CUSTOM_VIEWPORT_WIDTH:
         if (action == RGUI_ACTION_START)
         {
            unsigned w, h, t;
            driver.video_poke->get_resolution_info(video_data, g_settings.video.resolution_idx, &w, &h, &t);
            g_settings.video.custom_vp.width = w;
         }
         else if (action == RGUI_ACTION_LEFT)
               g_settings.video.custom_vp.width--;
         else if (action == RGUI_ACTION_RIGHT || action == RGUI_ACTION_OK)
               g_settings.video.custom_vp.width++;

         break;
      case RGUI_SETTINGS_CUSTOM_VIEWPORT_HEIGHT:
         if (action == RGUI_ACTION_START)
         {
            unsigned w, h, t;
            driver.video_poke->get_resolution_info(video_data, g_settings.video.resolution_idx, &w, &h, &t);
            g_settings.video.custom_vp.height = h;
         }
         else if (action == RGUI_ACTION_LEFT)
               g_settings.video.custom_vp.height--;
         else if (action == RGUI_ACTION_RIGHT || action == RGUI_ACTION_OK)
               g_settings.video.custom_vp.height++;

         break;
      case RGUI_SETTINGS_VIDEO_RESOLUTION:
      {
         unsigned old_index = g_settings.video.resolution_idx;
         if (action == RGUI_ACTION_LEFT)
         {
            unsigned first_res = g_extern.video.using_component || 
                                 g_settings.video.interlaced_resolution_only ? 
                                 g_extern.video.resolution_first_hires : GX_RESOLUTIONS_FIRST;

            if(g_settings.video.resolution_idx > first_res)
               g_settings.video.resolution_idx--;
         }
         else if (action == RGUI_ACTION_RIGHT)
         {
            if (g_settings.video.resolution_idx < GX_RESOLUTIONS_LAST)
               g_settings.video.resolution_idx++;
         }
         else if (action == RGUI_ACTION_START)
         {
            g_settings.video.resolution_idx = DEFAULT_VIDEO_RESOLUTION_IDX;
         }
         else if (action == RGUI_ACTION_OK)
         {
            /* Display current game reported resolution */
            char msg[48];
            
            if (!*g_extern.basename)
               strlcpy(msg, "No game is running", sizeof(msg));
            else
               snprintf(msg, sizeof(msg), "Game internal resolution: %ux%u", g_extern.frame_cache.width, g_extern.frame_cache.height);

            msg_queue_push(g_extern.msg_queue, msg, 0, 80);
         }

         if (old_index != g_settings.video.resolution_idx)
         {
            gfx_match_resolution_auto();

            /* adjust refresh rate accordingly */
            driver.video_poke->set_refresh_rate(video_data, g_settings.video.resolution_idx);
         }
         break;
      }
#ifdef HW_RVL
      case RGUI_SETTINGS_VIDEO_VITRAP_FILTER:
         if (action == RGUI_ACTION_START)
            g_settings.video.vi_trap_filter = DEFAULT_VIDEO_VI_TRAP_FILTER;
         else if (action == RGUI_ACTION_OK || action == RGUI_ACTION_RIGHT
               || action == RGUI_ACTION_LEFT)
            g_settings.video.vi_trap_filter =! g_settings.video.vi_trap_filter;

         driver.video_poke->apply_state_changes(video_data);
         break;
      case RGUI_SETTINGS_VIDEO_INTERLACED_ONLY:
         if (action == RGUI_ACTION_START)
            g_settings.video.interlaced_resolution_only = DEFAULT_VIDEO_INTERLACED_RESOLUTION_ONLY;
         else if (action == RGUI_ACTION_OK || action == RGUI_ACTION_RIGHT
               || action == RGUI_ACTION_LEFT)
            g_settings.video.interlaced_resolution_only =! g_settings.video.interlaced_resolution_only;
         
         gfx_check_valid_resolution();
         gfx_match_resolution_auto();
         break;
         
      case RGUI_SETTINGS_VIDEO_SCREEN_POS_X:
      case RGUI_SETTINGS_VIDEO_SCREEN_POS_Y:
      {
         int *pos = (setting == RGUI_SETTINGS_VIDEO_SCREEN_POS_X) ? &g_settings.video.pos_x : &g_settings.video.pos_y;

         switch (action)
         {
            case RGUI_ACTION_START:
               *pos = (setting == RGUI_SETTINGS_VIDEO_SCREEN_POS_X) ? DEFAULT_VIDEO_POS_X : DEFAULT_VIDEO_POS_Y;
               break;

            case RGUI_ACTION_LEFT:
               *pos -= 1;
               break;

            case RGUI_ACTION_RIGHT:
            case RGUI_ACTION_OK:
               *pos += 1;
               break;

            default:
               break;
         }
         
         if (*pos > DEFAULT_VIDEO_MAX_POS_RANGE) *pos = DEFAULT_VIDEO_MAX_POS_RANGE;
         else if (*pos < -DEFAULT_VIDEO_MAX_POS_RANGE) *pos = -DEFAULT_VIDEO_MAX_POS_RANGE;

         break;
      }         
#endif
      case RGUI_SETTINGS_VIDEO_VSYNC:
         switch (action)
         {
            case RGUI_ACTION_START:
               g_settings.video.vsync = DEFAULT_VIDEO_VSYNC;
               break;

            case RGUI_ACTION_LEFT:
            case RGUI_ACTION_RIGHT:
            case RGUI_ACTION_OK:
               g_settings.video.vsync = !g_settings.video.vsync;
               break;

            default:
               break;
         }
         break;
      case RGUI_SETTINGS_VIDEO_CROP_OVERSCAN:
         switch (action)
         {
            case RGUI_ACTION_START:
               g_settings.video.crop_overscan = DEFAULT_VIDEO_CROP_OVERSCAN;
               break;

            case RGUI_ACTION_LEFT:
            case RGUI_ACTION_RIGHT:
            case RGUI_ACTION_OK:
               g_settings.video.crop_overscan = !g_settings.video.crop_overscan;
               break;

            default:
               break;
         }
         break;
      default:
         break;
   }

   return 0;
}

void menu_set_settings_label(char *type_str, size_t type_str_size, unsigned *w, unsigned type)
{
   switch (type)
   {
      case RGUI_SETTINGS_VIDEO_ROTATION:
         strlcpy(type_str, rotation_lut[g_settings.video.rotation], type_str_size);
         break;
      case RGUI_SETTINGS_MENU_ROTATION:
         strlcpy(type_str, rotation_lut[g_settings.menu.rotation], type_str_size);
         break;
      case RGUI_SETTINGS_MENU_THEME:
         strlcpy(type_str, theme_lut[g_settings.menu.theme].name, type_str_size);
         break;
      case RGUI_SETTINGS_VIDEO_VITRAP_FILTER:
         snprintf(type_str, type_str_size, g_settings.video.vi_trap_filter ? "ON" : "OFF");
         break;
      case RGUI_SETTINGS_VIDEO_INTERLACED_ONLY:
         snprintf(type_str, type_str_size, g_settings.video.interlaced_resolution_only ? "ON" : "OFF");
         break;
      case RGUI_SETTINGS_VIDEO_SCREEN_POS_X:
         snprintf(type_str, type_str_size, "%d", g_settings.video.pos_x);
         break;
      case RGUI_SETTINGS_VIDEO_SCREEN_POS_Y:
         snprintf(type_str, type_str_size, "%d", g_settings.video.pos_y);
         break;         
      case RGUI_SETTINGS_VIDEO_BILINEAR:
         strlcpy(type_str, g_settings.video.bilinear_filter ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_MENU_BILINEAR:
         strlcpy(type_str, g_settings.menu.bilinear_filter ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_VIDEO_GAMMA:
         snprintf(type_str, type_str_size, "%d", g_settings.video.gamma_correction);
         break;
      case RGUI_SETTINGS_VIDEO_VSYNC:
         strlcpy(type_str, g_settings.video.vsync ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_VIDEO_CROP_OVERSCAN:
         strlcpy(type_str, g_settings.video.crop_overscan ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_INPUT_TYPE:
         strlcpy(type_str, g_settings.input.driver, type_str_size);
         /* Convert to uppercase */
         for(int i = 0; type_str[i] != '\0'; i++ ) type_str[i] = toupper(type_str[i]);
         break;
      case RGUI_SETTINGS_VIDEO_INTEGER_SCALE:
         strlcpy(type_str, g_settings.video.scale_integer ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_VIDEO_FORCE_ASPECT:
         strlcpy(type_str, g_settings.video.force_aspect ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_VIDEO_ASPECT_RATIO:
         strlcpy(type_str, aspectratio_lut[g_settings.video.aspect_ratio_idx].name, type_str_size);
         break;
      case RGUI_SETTINGS_CUSTOM_VIEWPORT_X:
         snprintf(type_str, type_str_size, "%d", g_settings.video.custom_vp.x);
         break;
      case RGUI_SETTINGS_CUSTOM_VIEWPORT_Y:
         snprintf(type_str, type_str_size, "%d", g_settings.video.custom_vp.y);
         break;
      case RGUI_SETTINGS_CUSTOM_VIEWPORT_WIDTH:
         snprintf(type_str, type_str_size, "%d", g_settings.video.custom_vp.width);
         break;
      case RGUI_SETTINGS_CUSTOM_VIEWPORT_HEIGHT:
         snprintf(type_str, type_str_size, "%d", g_settings.video.custom_vp.height);
         break;
      case RGUI_SETTINGS_VIDEO_RESOLUTION:
      {
         unsigned w, h, t;
         driver.video_poke->get_resolution_info(driver.video_data, g_settings.video.resolution_idx, &w, &h, &t);
         
         if (g_settings.video.resolution_idx == GX_RESOLUTIONS_AUTO && w == 0)
            strlcpy(type_str, "AUTO", type_str_size);
         else
            snprintf(type_str, type_str_size, g_settings.video.resolution_idx == GX_RESOLUTIONS_AUTO ? 
                     "AUTO (%ux%u%s %3.2fhz)" : "%ux%u%s %3.2fhz",
                     w, h, t ? "p" : "i", g_settings.video.refresh_rate);
         break;
      }
      case RGUI_FILE_PLAIN:
         strlcpy(type_str, "(FILE)", type_str_size);
         *w = 6;
         break;
      case RGUI_FILE_DIRECTORY:
         strlcpy(type_str, "(DIR)", type_str_size);
         *w = 5;
         break;
      case RGUI_SETTINGS_REWIND_ENABLE:
         strlcpy(type_str, g_settings.rewind_enable ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_REWIND_GRANULARITY:
         snprintf(type_str, type_str_size, "%u", g_settings.rewind_granularity);
         break;
      case RGUI_SETTINGS_CONFIG_SAVE_ON_EXIT:
         strlcpy(type_str, g_settings.config_save_on_exit ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_SAVESTATE_AUTO_SAVE:
         strlcpy(type_str, g_settings.savestate_auto_save ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_SAVESTATE_AUTO_LOAD:
         strlcpy(type_str, g_settings.savestate_auto_load ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_BLOCK_SRAM_OVERWRITE:
         strlcpy(type_str, g_settings.block_sram_overwrite ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_CONFIG_TYPE:
         if (g_extern.config_type == CONFIG_PER_GAME)
            strlcpy(type_str, "GAME", type_str_size);
         else
            strlcpy(type_str, "CORE", type_str_size);
         break;
      case RGUI_SETTINGS_CONFIG_SAVE_GAME_SPECIFIC:
         if (g_extern.config_type == CONFIG_PER_GAME)
            strlcpy(type_str, "<START> to Remove", type_str_size);
         else if (*g_extern.basename)
            strlcpy(type_str, "<OK> to Create", type_str_size);
         else
            strlcpy(type_str, "Load Game First...", type_str_size);
         break;
      case RGUI_SETTINGS_SAVESTATE_SAVE:
      case RGUI_SETTINGS_SAVESTATE_LOAD:
         if (g_settings.state_slot < 0)
            strlcpy(type_str, "-1 (auto)", type_str_size);
         else
            snprintf(type_str, type_str_size, "%d", g_settings.state_slot);
         break;
      case RGUI_SETTINGS_AUDIO_MUTE:
         strlcpy(type_str, g_settings.audio.mute ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_AUDIO_SYNC:
         strlcpy(type_str, g_settings.audio.sync ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_AUDIO_CONTROL_RATE_DELTA:
         snprintf(type_str, type_str_size, "%.3f", g_settings.audio.rate_control_delta);
         break;
      case RGUI_SETTINGS_SHOW_FRAMERATE:
         snprintf(type_str, type_str_size, (g_settings.fps_show) ? "ON" : "OFF");
         break;
      case RGUI_GAMES_DIR_PATH:
         strlcpy(type_str, *g_settings.games_directory ? g_settings.games_directory : "<default>", type_str_size);
         break;
#ifdef HAVE_SCREENSHOTS
      case RGUI_SCREENSHOT_DIR_PATH:
         strlcpy(type_str, *g_settings.screenshot_directory ? g_settings.screenshot_directory : "<ROM dir>", type_str_size);
         break;
#endif
      case RGUI_SAVEFILE_DIR_PATH:
         strlcpy(type_str, *g_settings.savefile_directory ? g_settings.savefile_directory : "<default>", type_str_size);
         break;
#ifdef HAVE_OVERLAY
      case RGUI_OVERLAY_DIR_PATH:
         strlcpy(type_str, *g_settings.overlay_directory ? g_settings.overlay_directory : "<default>", type_str_size);
         break;
#endif
      case RGUI_SAVESTATE_DIR_PATH:
         strlcpy(type_str, *g_settings.savestate_directory ? g_settings.savestate_directory : "<default>", type_str_size);
         break;
      case RGUI_LIBRETRO_INFO_DIR_PATH:
         strlcpy(type_str, *g_settings.libretro_info_directory ? g_settings.libretro_info_directory : "<Core dir>", type_str_size);
         break;
      case RGUI_CONFIG_DIR_PATH:
         strlcpy(type_str, *g_settings.config_directory ? g_settings.config_directory : "<default>", type_str_size);
         break;
      case RGUI_SYSTEM_DIR_PATH:
         strlcpy(type_str, *g_settings.system_directory ? g_settings.system_directory : "<default>", type_str_size);
         break;
      case RGUI_SETTINGS_DISK_INDEX:
         {
            const struct retro_disk_control_callback *control = &g_extern.system.disk_control;
            unsigned images = control->get_num_images();
            unsigned current = control->get_image_index();
            if (current >= images)
               strlcpy(type_str, "No Disk", type_str_size);
            else
               snprintf(type_str, type_str_size, "%u", current + 1);
            break;
         }
      case RGUI_SETTINGS_OPEN_FILEBROWSER:
      case RGUI_SETTINGS_OPEN_FILEBROWSER_DEFERRED_CORE:
      case RGUI_SETTINGS_OPEN_HISTORY:
      case RGUI_SETTINGS_CORE_OPTIONS:
      case RGUI_SETTINGS_SYSTEM_INFO:
      case RGUI_SETTINGS_VIDEO_OPTIONS:
      case RGUI_SETTINGS_AUDIO_OPTIONS:
      case RGUI_SETTINGS_DISK_OPTIONS:
      case RGUI_SETTINGS_CONFIG_OPTIONS:
      case RGUI_SETTINGS_SAVE_OPTIONS:
      case RGUI_SETTINGS_CORE:
      case RGUI_SETTINGS_DISK_APPEND:
      case RGUI_SETTINGS_INPUT_OPTIONS:
      case RGUI_SETTINGS_PATH_OPTIONS:
      case RGUI_SETTINGS_OVERLAY_OPTIONS:
      case RGUI_SETTINGS_OPTIONS:
      case RGUI_SETTINGS_BIND_DEFAULT_ALL:
      case RGUI_SETTINGS_BIND_HOTKEYS:
      case RGUI_SETTINGS_BIND_PLAYER_KEYS:
      case RGUI_SETTINGS_MENU_OPTIONS:
         strlcpy(type_str, "...", type_str_size);
         break;
#ifdef HAVE_SCALERS_BUILTIN
      case RGUI_SETTINGS_VIDEO_SOFT_SCALER:
         {
            const char *filter_name = rarch_softfilter_get_name(g_settings.video.filter_idx);
            strlcpy(type_str, filter_name ? filter_name : "OFF", type_str_size);
         }
         break;
#endif
#ifdef HAVE_OVERLAY
      case RGUI_SETTINGS_OVERLAY_PRESET:
         strlcpy(type_str, path_basename(g_settings.input.overlay_path), type_str_size);
         break;
      case RGUI_SETTINGS_OVERLAY_OPACITY:
         snprintf(type_str, type_str_size, "%.2f", g_settings.input.overlay_opacity);
         break;
      case RGUI_SETTINGS_OVERLAY_SCALE:
         snprintf(type_str, type_str_size, "%.2f", g_settings.input.overlay_scale);
         break;
#endif
      case RGUI_SETTINGS_BIND_PLAYER:
         snprintf(type_str, type_str_size, "< #%d >", rgui->c_player + 1);
         break;
      case RGUI_SETTINGS_BIND_DEVICE:
         if (g_settings.input.device_names[rgui->s_device][0] != '\0')
            snprintf(type_str, type_str_size, "%d:%s", rgui->s_device+1, g_settings.input.device_names[rgui->s_device]);
         else
            snprintf(type_str, type_str_size, "%d:No Device", rgui->s_device+1);
         break;
      case RGUI_SETTINGS_BIND_ANALOG_MODE:
      {
         static const char *modes[] = {
            "None",
            "Left Analog",
            "Right Analog",
            "Dual Analog",
         };

         strlcpy(type_str, modes[g_settings.input.analog_dpad_mode[rgui->c_player] % ANALOG_DPAD_LAST], type_str_size);
         break;
      }
      case RGUI_SETTINGS_BIND_DEVICE_TYPE:
      {
         const struct retro_controller_description *desc = NULL;
         if (rgui->c_player < g_extern.system.num_ports)
         {
            desc = libretro_find_controller_description(&g_extern.system.ports[rgui->c_player],
                  g_settings.input.libretro_device[rgui->c_player]);
         }

         const char *name = desc ? desc->desc : NULL;
         if (!name) // Find generic name.
         {
            switch (g_settings.input.libretro_device[rgui->c_player])
            {
               case RETRO_DEVICE_NONE: name = "None"; break;
               case RETRO_DEVICE_JOYPAD: name = "Joypad"; break;
               case RETRO_DEVICE_ANALOG: name = "Joypad w/ Analog"; break;
               default: name = "Unknown"; break;
            }
         }

         strlcpy(type_str, name, type_str_size);
         break;
      }
      case RGUI_SETTINGS_DEVICE_AUTOCONF_BUTTONS:
         strlcpy(type_str, g_settings.input.autoconf_buttons ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_MENU_ALL_PLAYERS_ENABLE:
         strlcpy(type_str, g_settings.input.menu_all_players_enable ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_QUICK_SWAP_PLAYERS:
         if (g_settings.input.quick_swap_players < 2 || g_settings.input.quick_swap_players > MAX_PLAYERS)
            strlcpy(type_str, "Disabled", type_str_size);
         else
            snprintf(type_str, type_str_size, "%d Players", g_settings.input.quick_swap_players);
         break;
      case RGUI_SETTINGS_BIND_FAST_FORWARD_KEY:
      case RGUI_SETTINGS_BIND_FAST_FORWARD_HOLD_KEY:
      case RGUI_SETTINGS_BIND_LOAD_STATE_KEY:
      case RGUI_SETTINGS_BIND_SAVE_STATE_KEY:
      case RGUI_SETTINGS_BIND_QUIT_KEY:
      case RGUI_SETTINGS_BIND_STATE_SLOT_PLUS:
      case RGUI_SETTINGS_BIND_STATE_SLOT_MINUS:
      case RGUI_SETTINGS_BIND_REWIND:
      case RGUI_SETTINGS_BIND_PAUSE_TOGGLE:
      case RGUI_SETTINGS_BIND_FRAMEADVANCE:
      case RGUI_SETTINGS_BIND_RESET:
      case RGUI_SETTINGS_BIND_SCREENSHOT:
      case RGUI_SETTINGS_BIND_MUTE:
      case RGUI_SETTINGS_BIND_SLOWMOTION:
      case RGUI_SETTINGS_BIND_ENABLE_HOTKEY:
      case RGUI_SETTINGS_BIND_VOLUME_UP:
      case RGUI_SETTINGS_BIND_VOLUME_DOWN:
      case RGUI_SETTINGS_BIND_DISK_EJECT_TOGGLE:
      case RGUI_SETTINGS_BIND_DISK_NEXT:
      case RGUI_SETTINGS_BIND_QUICK_SWAP:
      case RGUI_SETTINGS_BIND_MENU_TOGGLE:
         /* always use first controller for hotkey bindings */
         input_get_bind_string(type_str, &g_settings.input.binds[0][type - RGUI_SETTINGS_BIND_BEGIN], 0, type_str_size);      
         break;
      case RGUI_SETTINGS_BIND_UP:
      case RGUI_SETTINGS_BIND_DOWN:
      case RGUI_SETTINGS_BIND_LEFT:
      case RGUI_SETTINGS_BIND_RIGHT:
      case RGUI_SETTINGS_BIND_A:
      case RGUI_SETTINGS_BIND_B:
      case RGUI_SETTINGS_BIND_X:
      case RGUI_SETTINGS_BIND_Y:
      case RGUI_SETTINGS_BIND_START:
      case RGUI_SETTINGS_BIND_SELECT:
      case RGUI_SETTINGS_BIND_L:
      case RGUI_SETTINGS_BIND_R:
      case RGUI_SETTINGS_BIND_L2:
      case RGUI_SETTINGS_BIND_R2:
      case RGUI_SETTINGS_BIND_L3:
      case RGUI_SETTINGS_BIND_R3:
      case RGUI_SETTINGS_BIND_ANALOG_LEFT_X_PLUS:
      case RGUI_SETTINGS_BIND_ANALOG_LEFT_X_MINUS:
      case RGUI_SETTINGS_BIND_ANALOG_LEFT_Y_PLUS:
      case RGUI_SETTINGS_BIND_ANALOG_LEFT_Y_MINUS:
      case RGUI_SETTINGS_BIND_ANALOG_RIGHT_X_PLUS:
      case RGUI_SETTINGS_BIND_ANALOG_RIGHT_X_MINUS:
      case RGUI_SETTINGS_BIND_ANALOG_RIGHT_Y_PLUS:
      case RGUI_SETTINGS_BIND_ANALOG_RIGHT_Y_MINUS:
      case RGUI_SETTINGS_BIND_TURBO_ENABLE:
      {
         unsigned port = g_settings.input.device_port[rgui->c_player];
         input_get_bind_string(type_str, &g_settings.input.binds[port][type - RGUI_SETTINGS_BIND_BEGIN], port, type_str_size);
         break;
      }
      case RGUI_SETTINGS_AUDIO_VOLUME:
         snprintf(type_str, type_str_size, "%.1f dB", g_extern.audio_data.volume_db);
         break;
      default:
         *type_str = '\0';
         *w = 0;
         break;
   }
}
