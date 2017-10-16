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
#include "menu_input_line_cb.h"
#include "../../gfx/gfx_common.h"
#include "../../input/input_common.h"
#include "../../config.def.h"
#include "../../input/keyboard_line.h"

#ifdef HAVE_CONFIG_H
#include "../../config.h"
#endif

#ifdef GEKKO
#define MAX_GAMMA_SETTING 2
#else
#define MAX_GAMMA_SETTING 1
#endif

unsigned menu_type_is(unsigned type)
{
   unsigned ret = 0;
   bool type_found;

   type_found =
      type == RGUI_SETTINGS ||
      type == RGUI_SETTINGS_CONFIG_OPTIONS ||
      type == RGUI_SETTINGS_SAVE_OPTIONS ||
      type == RGUI_SETTINGS_CORE_OPTIONS ||
      type == RGUI_SETTINGS_CORE_INFO ||
      type == RGUI_SETTINGS_VIDEO_OPTIONS ||
      type == RGUI_SETTINGS_SHADER_OPTIONS ||
      type == RGUI_SETTINGS_AUDIO_OPTIONS ||
      type == RGUI_SETTINGS_DISK_OPTIONS ||
      type == RGUI_SETTINGS_PATH_OPTIONS ||
      type == RGUI_SETTINGS_OVERLAY_OPTIONS ||
      type == RGUI_SETTINGS_OPTIONS ||
      type == RGUI_SETTINGS_DRIVERS ||
      type == RGUI_SETTINGS_BIND_PLAYER_KEYS ||
      type == RGUI_SETTINGS_BIND_HOTKEYS ||
      type == RGUI_SETTINGS_INPUT_OPTIONS;

   if (type_found)
   {
      ret = RGUI_SETTINGS;
      return ret;
   }

   type_found = (type >= RGUI_SETTINGS_SHADER_0 &&
         type <= RGUI_SETTINGS_SHADER_LAST &&
         ((type - RGUI_SETTINGS_SHADER_0) % 3) == 0) ||
      type == RGUI_SETTINGS_SHADER_PRESET;

   if (type_found)
   {
      ret = RGUI_SETTINGS_SHADER_OPTIONS;
      return ret;
   }

   type_found = type == RGUI_BROWSER_DIR_PATH ||
      type == RGUI_SHADER_DIR_PATH ||
      type == RGUI_SAVESTATE_DIR_PATH ||
      type == RGUI_LIBRETRO_DIR_PATH ||
      type == RGUI_LIBRETRO_INFO_DIR_PATH ||
      type == RGUI_CONFIG_DIR_PATH ||
      type == RGUI_SAVEFILE_DIR_PATH ||
      type == RGUI_OVERLAY_DIR_PATH ||
#ifndef RARCH_CONSOLE
      type == RGUI_AUTOCONF_DIR_PATH ||
#endif
      type == RGUI_SCREENSHOT_DIR_PATH ||
      type == RGUI_SYSTEM_DIR_PATH;

   if (type_found)
   {
      ret = RGUI_FILE_DIRECTORY;
      return ret;
   }

   return ret;
}

#ifdef HAVE_SHADER_MANAGER
static enum rarch_shader_type shader_manager_get_type(const struct gfx_shader *shader)
{
   unsigned i;
   // All shader types must be the same, or we cannot use it.
   enum rarch_shader_type type = RARCH_SHADER_NONE;

   for (i = 0; i < shader->passes; i++)
   {
      enum rarch_shader_type pass_type = gfx_shader_parse_type(shader->pass[i].source.cg,
            RARCH_SHADER_NONE);

      switch (pass_type)
      {
         case RARCH_SHADER_CG:
         case RARCH_SHADER_GLSL:
            if (type == RARCH_SHADER_NONE)
               type = pass_type;
            else if (type != pass_type)
               return RARCH_SHADER_NONE;
            break;

         default:
            return RARCH_SHADER_NONE;
      }
   }

   return type;
}

void shader_manager_save_preset(void *data, const char *basename, bool apply)
{
   rgui_handle_t *rgui = (rgui_handle_t*)data;
   enum rarch_shader_type type = shader_manager_get_type(&rgui->shader);
   if (type == RARCH_SHADER_NONE)
      return;

   const char *conf_path = NULL;
   char buffer[PATH_MAX];
   if (basename)
   {
      strlcpy(buffer, basename, sizeof(buffer));
      // Append extension automatically as appropriate.
      if (!strstr(basename, ".cgp") && !strstr(basename, ".glslp"))
      {
         if (type == RARCH_SHADER_GLSL)
            strlcat(buffer, ".glslp", sizeof(buffer));
         else if (type == RARCH_SHADER_CG)
            strlcat(buffer, ".cgp", sizeof(buffer));
      }
      conf_path = buffer;
   }
   else
      conf_path = type == RARCH_SHADER_GLSL ? rgui->default_glslp : rgui->default_cgp;

   char config_directory[PATH_MAX];
   if (*g_extern.config_path)
      fill_pathname_basedir(config_directory, g_extern.config_path, sizeof(config_directory));
   else
      *config_directory = '\0';

   char cgp_path[PATH_MAX];
   const char *dirs[] = {
      g_settings.video.shader_dir,
      g_settings.rgui_config_directory,
      config_directory,
   };

   config_file_t *conf = config_file_new(NULL);
   if (!conf)
      return;
   gfx_shader_write_conf_cgp(conf, &rgui->shader);

   bool ret = false;
   unsigned d;
   for (d = 0; d < ARRAY_SIZE(dirs); d++)
   {
      if (!*dirs[d])
         continue;

      fill_pathname_join(cgp_path, dirs[d], conf_path, sizeof(cgp_path));
      if (config_file_write(conf, cgp_path))
      {
         RARCH_LOG("Saved shader preset to %s.\n", cgp_path);
         if (apply)
            shader_manager_set_preset(NULL, type, cgp_path);
         ret = true;
         break;
      }
      else
         RARCH_LOG("Failed writing shader preset to %s.\n", cgp_path);
   }

   config_file_free(conf);
   if (!ret)
      RARCH_ERR("Failed to save shader preset. Make sure config directory and/or shader dir are writable.\n");
}

static int shader_manager_toggle_setting(void *data, void *video_data, unsigned setting, unsigned action)
{
   unsigned dist_shader, dist_filter, dist_scale;
   rgui_handle_t *rgui = (rgui_handle_t*)data;
   dist_shader = setting - RGUI_SETTINGS_SHADER_0;
   dist_filter = setting - RGUI_SETTINGS_SHADER_0_FILTER;
   dist_scale  = setting - RGUI_SETTINGS_SHADER_0_SCALE;

   if (setting == RGUI_SETTINGS_SHADER_FILTER)
   {
      switch (action)
      {
         case RGUI_ACTION_START:
            g_settings.video.smooth = true;
            break;

         case RGUI_ACTION_LEFT:
         case RGUI_ACTION_RIGHT:
         case RGUI_ACTION_OK:
            g_settings.video.smooth = !g_settings.video.smooth;
            break;

         default:
            break;
      }
   }
   else if (setting == RGUI_SETTINGS_SHADER_APPLY || setting == RGUI_SETTINGS_SHADER_PASSES)
      return menu_set_settings(rgui, video_data, setting, action);
   else if ((dist_shader % 3) == 0 || setting == RGUI_SETTINGS_SHADER_PRESET)
   {
      dist_shader /= 3;
      struct gfx_shader_pass *pass = setting == RGUI_SETTINGS_SHADER_PRESET ?
         &rgui->shader.pass[dist_shader] : NULL;
      switch (action)
      {
         case RGUI_ACTION_OK:
            file_list_push(rgui->menu_stack, g_settings.video.shader_dir, setting, rgui->selection_ptr);
            menu_clear_navigation(rgui);
            rgui->need_refresh = true;
            break;

         case RGUI_ACTION_START:
            if (pass)
               *pass->source.cg = '\0';
            break;

         default:
            break;
      }
   }
   else if ((dist_filter % 3) == 0)
   {
      dist_filter /= 3;
      struct gfx_shader_pass *pass = &rgui->shader.pass[dist_filter];
      switch (action)
      {
         case RGUI_ACTION_START:
            rgui->shader.pass[dist_filter].filter = RARCH_FILTER_UNSPEC;
            break;

         case RGUI_ACTION_LEFT:
         case RGUI_ACTION_RIGHT:
         case RGUI_ACTION_OK:
         {
            unsigned delta = action == RGUI_ACTION_LEFT ? 2 : 1;
            pass->filter = (enum gfx_filter_type)((pass->filter + delta) % 3);
            break;
         }

         default:
         break;
      }
   }
   else if ((dist_scale % 3) == 0)
   {
      dist_scale /= 3;
      struct gfx_shader_pass *pass = &rgui->shader.pass[dist_scale];
      switch (action)
      {
         case RGUI_ACTION_START:
            pass->fbo.scale_x = pass->fbo.scale_y = 0;
            pass->fbo.valid = false;
            break;

         case RGUI_ACTION_LEFT:
         case RGUI_ACTION_RIGHT:
         case RGUI_ACTION_OK:
         {
            unsigned current_scale = pass->fbo.scale_x;
            unsigned delta = action == RGUI_ACTION_LEFT ? 5 : 1;
            current_scale = (current_scale + delta) % 6;
            pass->fbo.valid = current_scale;
            pass->fbo.scale_x = pass->fbo.scale_y = current_scale;
            break;
         }

         default:
         break;
      }
   }

   return 0;
}
#endif

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
#ifdef HAVE_SHADER_MANAGER
   if (setting >= RGUI_SETTINGS_SHADER_FILTER && setting <= RGUI_SETTINGS_SHADER_LAST)
      return shader_manager_toggle_setting(rgui, video_data, setting, action);
#endif
   if (setting >= RGUI_SETTINGS_CORE_OPTION_START)
      return menu_core_setting_toggle(setting, action);

   return menu_set_settings(rgui, video_data, setting, action);
}

void update_config_params()
{
   config_load();
   
   rarch_reset_drivers();
      
#ifndef GEKKO
   if (driver.video_poke && driver.video_poke->set_aspect_ratio)
      driver.video_poke->set_aspect_ratio(driver.video_data, g_settings.video.aspect_ratio_idx);
#endif
}

int menu_set_settings(void *data, void *video_data, unsigned setting, unsigned action)
{
   rgui_handle_t *rgui = (rgui_handle_t*)data;
   unsigned port = rgui->current_pad;

   switch (setting)
   {
      case RGUI_HELP_SCREEN:
         if (action == RGUI_ACTION_OK)
            file_list_push(rgui->menu_stack, "", RGUI_HELP_SCREEN, 0);
         break;
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
            g_settings.rewind_enable = false;
            rarch_deinit_rewind();
         }
         break;
#ifdef HAVE_SCREENSHOTS
      case RGUI_SETTINGS_GPU_SCREENSHOT:
         if (action == RGUI_ACTION_OK ||
               action == RGUI_ACTION_LEFT ||
               action == RGUI_ACTION_RIGHT)
            g_settings.video.gpu_screenshot = !g_settings.video.gpu_screenshot;
         else if (action == RGUI_ACTION_START)
            g_settings.video.gpu_screenshot = true;
         break;
#endif
      case RGUI_SETTINGS_REWIND_GRANULARITY:
         if (action == RGUI_ACTION_OK || action == RGUI_ACTION_RIGHT)
            g_settings.rewind_granularity++;
         else if (action == RGUI_ACTION_LEFT)
         {
            if (g_settings.rewind_granularity > 1)
               g_settings.rewind_granularity--;
         }
         else if (action == RGUI_ACTION_START)
            g_settings.rewind_granularity = 1;
         break;
      case RGUI_SETTINGS_CONFIG_SAVE_ON_EXIT:
         if (action == RGUI_ACTION_OK || action == RGUI_ACTION_RIGHT
               || action == RGUI_ACTION_LEFT)
            g_extern.config_save_on_exit = !g_extern.config_save_on_exit;
         else if (action == RGUI_ACTION_START)
            g_extern.config_save_on_exit = true;
         break;
      case RGUI_SETTINGS_SAVESTATE_AUTO_SAVE:
         if (action == RGUI_ACTION_OK || action == RGUI_ACTION_RIGHT
               || action == RGUI_ACTION_LEFT)
            g_settings.savestate_auto_save = !g_settings.savestate_auto_save;
         else if (action == RGUI_ACTION_START)
            g_settings.savestate_auto_save = false;
         break;
      case RGUI_SETTINGS_SAVESTATE_AUTO_LOAD:
         if (action == RGUI_ACTION_OK || action == RGUI_ACTION_RIGHT
               || action == RGUI_ACTION_LEFT)
            g_settings.savestate_auto_load = !g_settings.savestate_auto_load;
         else if (action == RGUI_ACTION_START)
            g_settings.savestate_auto_load = true;
         break;
      case RGUI_SETTINGS_BLOCK_SRAM_OVERWRITE:
         if (action == RGUI_ACTION_OK || action == RGUI_ACTION_RIGHT
               || action == RGUI_ACTION_LEFT)
            g_settings.block_sram_overwrite = !g_settings.block_sram_overwrite;
         else if (action == RGUI_ACTION_START)
            g_settings.block_sram_overwrite = false;
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
#if defined(HAVE_THREADS)
      case RGUI_SETTINGS_SRAM_AUTOSAVE:
         if (action == RGUI_ACTION_OK || action == RGUI_ACTION_RIGHT)
         {
            rarch_deinit_autosave();
            g_settings.autosave_interval += 10;
            if (g_settings.autosave_interval)
               rarch_init_autosave();
         }
         else if (action == RGUI_ACTION_LEFT)
         {
            if (g_settings.autosave_interval)
            {
               rarch_deinit_autosave();
               g_settings.autosave_interval -= min(10, g_settings.autosave_interval);
               if (g_settings.autosave_interval)
                  rarch_init_autosave();
            }
         }
         else if (action == RGUI_ACTION_START)
         {
            rarch_deinit_autosave();
            g_settings.autosave_interval = 0;
         }
         break;
#endif
      case RGUI_SETTINGS_SAVESTATE_SAVE:
      case RGUI_SETTINGS_SAVESTATE_LOAD:
         if (action == RGUI_ACTION_OK)
         {
            if (setting == RGUI_SETTINGS_SAVESTATE_SAVE)
               rarch_save_state();
            else
            {
               // Disallow savestate load when we absoluetely cannot change game state.

               rarch_load_state();
            }
            g_extern.lifecycle_state |= (1ULL << MODE_GAME);
            return -1;
         }
         else if (action == RGUI_ACTION_START)
            g_extern.state_slot = 0;
         else if (action == RGUI_ACTION_LEFT)
         {
            // Slot -1 is (auto) slot.
            if (g_extern.state_slot >= 0)
               g_extern.state_slot--;
         }
         else if (action == RGUI_ACTION_RIGHT)
            g_extern.state_slot++;
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
            g_extern.audio_data.mute = false;
         else
            g_extern.audio_data.mute = !g_extern.audio_data.mute;
         break;
      case RGUI_SETTINGS_AUDIO_SYNC:
         if (action == RGUI_ACTION_START)
            g_settings.audio.sync = false;
         else
            g_settings.audio.sync = !g_settings.audio.sync;
         break;
      case RGUI_SETTINGS_AUDIO_CONTROL_RATE_DELTA:
         if (action == RGUI_ACTION_START)
         {
            g_settings.audio.rate_control_delta = rate_control_delta;
            g_settings.audio.rate_control = rate_control;
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
         else if (action == RGUI_ACTION_RIGHT)
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
            g_extern.audio_data.volume_db = 0.0f;
            g_extern.audio_data.volume_gain = 1.0f;
         }
         else if (action == RGUI_ACTION_LEFT)
            db_delta -= 1.0f;
         else if (action == RGUI_ACTION_RIGHT)
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
      case RGUI_SETTINGS_DEBUG_TEXT:
         if (action == RGUI_ACTION_START)
            g_settings.fps_show = false;
         else if (action == RGUI_ACTION_LEFT || action == RGUI_ACTION_RIGHT)
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
      case RGUI_SETTINGS_RESTART_EMULATOR:
         if (action == RGUI_ACTION_OK)
         {
#if defined(GEKKO) && defined(HW_RVL)
            fill_pathname_join(g_extern.fullpath, default_paths.core_dir, SALAMANDER_FILE,
                  sizeof(g_extern.fullpath));
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
               file_list_push(rgui->menu_stack, g_extern.overlay_dir, setting, rgui->selection_ptr);
               menu_clear_navigation(rgui);
               rgui->need_refresh = true;
               break;

            case RGUI_ACTION_START:
               if (driver.overlay)
                  input_overlay_free(driver.overlay);
               driver.overlay = NULL;
               *g_settings.input.overlay = '\0';
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
                  g_settings.input.overlay_opacity = 0.7f;
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
                  g_settings.input.overlay_scale = 1.0f;
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
            case RGUI_ACTION_OK:
               rarch_reset_drivers();
               rgui->need_refresh = true;
               break;
            case RGUI_ACTION_START:
               g_settings.video.filter_idx = 0;
               rarch_reset_drivers();
               break;
         }
         
         if (action == RGUI_ACTION_OK || action == RGUI_ACTION_START)
         {
            char msg[48] = "Soft scaler removed";
            const char *filter_name = rarch_softfilter_get_name(g_settings.video.filter_idx);
            if (filter_name)
               snprintf(msg, sizeof(msg), "%s applied", filter_name);
            msg_queue_push(g_extern.msg_queue, msg, 1, 90);
         }    
         break;
#endif        
         // controllers
      case RGUI_SETTINGS_BIND_PLAYER:
         if (action == RGUI_ACTION_START)
            rgui->current_pad = 0;
         else if (action == RGUI_ACTION_LEFT)
         {
            if (rgui->current_pad != 0)
               rgui->current_pad--;
         }
         else if (action == RGUI_ACTION_RIGHT)
         {
            if (rgui->current_pad < MAX_PLAYERS - 1)
               rgui->current_pad++;
         }
#ifdef HAVE_MENU
         if (port != rgui->current_pad)
            rgui->need_refresh = true;
#endif
         port = rgui->current_pad;
         break;
      case RGUI_SETTINGS_BIND_DEVICE:
         // If set_keybinds is supported, we do it more fancy, and scroll through
         // a list of supported devices directly.
         if (driver.input->set_keybinds)
         {
            g_settings.input.device[port] += DEVICE_LAST;
            if (action == RGUI_ACTION_START)
               g_settings.input.device[port] = 0;
            else if (action == RGUI_ACTION_LEFT)
               g_settings.input.device[port]--;
            else if (action == RGUI_ACTION_RIGHT)
               g_settings.input.device[port]++;

            // DEVICE_LAST can be 0, avoid modulo.
            if (g_settings.input.device[port] >= DEVICE_LAST)
               g_settings.input.device[port] -= DEVICE_LAST;
            // needs to be checked twice, in case we go right past the end of the list
            if (g_settings.input.device[port] >= DEVICE_LAST)
               g_settings.input.device[port] -= DEVICE_LAST;

            unsigned keybind_action = (1ULL << KEYBINDS_ACTION_SET_DEFAULT_BINDS);

            driver.input->set_keybinds(driver.input_data, g_settings.input.device[port], port, 0,
                  keybind_action);
         }
         else
         {
            // When only straight g_settings.input.joypad_map[] style
            // mapping is supported.
            int *p = &g_settings.input.joypad_map[port];
            if (action == RGUI_ACTION_START)
               *p = port;
            else if (action == RGUI_ACTION_LEFT)
               (*p)--;
            else if (action == RGUI_ACTION_RIGHT)
               (*p)++;

            if (*p < -1)
               *p = -1;
            else if (*p >= MAX_PLAYERS)
               *p = MAX_PLAYERS - 1;
         }
         break;
      case RGUI_SETTINGS_BIND_ANALOG_MODE:
         switch (action)
         {
            case RGUI_ACTION_START:
               g_settings.input.analog_dpad_mode[port] = 0;
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
      case RGUI_SETTINGS_BIND_DEVICE_TYPE:
         {
            unsigned current_device, current_index, i;
            unsigned types = 0;
            unsigned devices[128];

            devices[types++] = RETRO_DEVICE_NONE;
            devices[types++] = RETRO_DEVICE_JOYPAD;
            devices[types++] = RETRO_DEVICE_ANALOG;

            const struct retro_controller_info *desc = port < g_extern.system.num_ports ? &g_extern.system.ports[port] : NULL;
            if (desc)
            {
               for (i = 0; i < desc->num_types; i++)
               {
                  unsigned id = desc->types[i].id;
                  if (types < ARRAY_SIZE(devices) && id != RETRO_DEVICE_NONE && id != RETRO_DEVICE_JOYPAD && id != RETRO_DEVICE_ANALOG)
                     devices[types++] = id;
               }
            }

            current_device = g_settings.input.libretro_device[port];
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
               g_settings.input.libretro_device[port] = current_device;
               pretro_set_controller_port_device(port, current_device);
            }

            break;
         }
      case RGUI_SETTINGS_DEVICE_AUTODETECT_ENABLE:
         if (action == RGUI_ACTION_OK || action == RGUI_ACTION_RIGHT || action == RGUI_ACTION_LEFT)
            g_settings.input.autodetect_enable = !g_settings.input.autodetect_enable;
         else
            g_settings.input.autodetect_enable = input_autodetect_enable;
         break;
      case RGUI_SETTINGS_MENU_ALL_PLAYERS_ENABLE:
         if (action == RGUI_ACTION_OK || action == RGUI_ACTION_RIGHT || action == RGUI_ACTION_LEFT)
            g_settings.input.menu_all_players_enable = !g_settings.input.menu_all_players_enable;
         else
            g_settings.input.menu_all_players_enable = menu_all_players_enable;
         break;                  
      case RGUI_SETTINGS_CUSTOM_BIND_ALL:
         if (action == RGUI_ACTION_OK)
         {
            rgui->binds.target = &g_settings.input.binds[port][0];
            rgui->binds.begin = RGUI_SETTINGS_BIND_BEGIN;
            rgui->binds.last = RGUI_SETTINGS_BIND_LAST;
            file_list_push(rgui->menu_stack, "", RGUI_SETTINGS_CUSTOM_BIND, rgui->selection_ptr);
            menu_poll_bind_get_rested_axes(&rgui->binds);
            menu_poll_bind_state(&rgui->binds);
         }
         break;
      case RGUI_SETTINGS_CUSTOM_BIND_DEFAULT_ALL:
         if (action == RGUI_ACTION_OK)
         {
            unsigned i;
            struct retro_keybind *target = &g_settings.input.binds[port][0];
            rgui->binds.begin = RGUI_SETTINGS_BIND_BEGIN;
            rgui->binds.last = RGUI_SETTINGS_BIND_LAST;
            for (i = RGUI_SETTINGS_BIND_BEGIN; i <= RGUI_SETTINGS_BIND_LAST; i++, target++)
            {
               target->joykey = NO_BTN;
               target->joyaxis = AXIS_NONE;
            }
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
#ifndef RARCH_CONSOLE
      case RGUI_SETTINGS_BIND_FULLSCREEN_TOGGLE_KEY:
      case RGUI_SETTINGS_BIND_SHADER_NEXT:
      case RGUI_SETTINGS_BIND_SHADER_PREV:
      case RGUI_SETTINGS_BIND_DSP_CONFIG:
      case RGUI_SETTINGS_BIND_OVERLAY_NEXT:
      case RGUI_SETTINGS_BIND_GRAB_MOUSE_TOGGLE:
#endif
#ifdef HAVE_MENU
      case RGUI_SETTINGS_BIND_MENU_TOGGLE:
#endif
         port = 0; /* all the hotkeys always mapped to first port */
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
         if (driver.input->set_keybinds && !driver.input->get_joypad_driver)
         {
            unsigned keybind_action = KEYBINDS_ACTION_NONE;

            if (action == RGUI_ACTION_START)
               keybind_action = (1ULL << KEYBINDS_ACTION_SET_DEFAULT_BIND);

            if (keybind_action != KEYBINDS_ACTION_NONE)
               driver.input->set_keybinds(driver.input_data, g_settings.input.device[port], port,
                     setting - RGUI_SETTINGS_BIND_BEGIN, keybind_action);
         }
         else
         {
            struct retro_keybind *bind = &g_settings.input.binds[port][setting - RGUI_SETTINGS_BIND_BEGIN];
            if (action == RGUI_ACTION_OK)
            {
               rgui->binds.begin = setting;
               rgui->binds.last = setting;
               rgui->binds.target = bind;
               rgui->binds.player = port;
               file_list_push(rgui->menu_stack, "", RGUI_SETTINGS_CUSTOM_BIND, rgui->selection_ptr);
               menu_poll_bind_get_rested_axes(&rgui->binds);
               menu_poll_bind_state(&rgui->binds);
            }
            else if (action == RGUI_ACTION_START)
            {
               bind->joykey = NO_BTN;
               bind->joyaxis = AXIS_NONE;
            }
         }
         break;
      case RGUI_BROWSER_DIR_PATH:
         if (action == RGUI_ACTION_START)
            *g_settings.rgui_content_directory = '\0';
         break;
#ifdef HAVE_SCREENSHOTS
      case RGUI_SCREENSHOT_DIR_PATH:
         if (action == RGUI_ACTION_START)
            *g_settings.screenshot_directory = '\0';
         break;
#endif
      case RGUI_SAVEFILE_DIR_PATH:
         if (action == RGUI_ACTION_START)
            *g_extern.savefile_dir = '\0';
         break;
#ifdef HAVE_OVERLAY
      case RGUI_OVERLAY_DIR_PATH:
         if (action == RGUI_ACTION_START)
            *g_extern.overlay_dir = '\0';
         break;
#endif
#ifndef RARCH_CONSOLE
      case RGUI_AUTOCONF_DIR_PATH:
         if (action == RGUI_ACTION_START)
            *g_extern.overlay_dir = '\0';
         break;
#endif
      case RGUI_SAVESTATE_DIR_PATH:
         if (action == RGUI_ACTION_START)
            *g_extern.savestate_dir = '\0';
         break;
      case RGUI_LIBRETRO_DIR_PATH:
         if (action == RGUI_ACTION_START)
         {
            *rgui->libretro_dir = '\0';
            menu_init_core_info(rgui);
         }
         break;
      case RGUI_LIBRETRO_INFO_DIR_PATH:
         if (action == RGUI_ACTION_START)
         {
            *g_settings.libretro_info_path = '\0';
            menu_init_core_info(rgui);
         }
         break;
      case RGUI_CONFIG_DIR_PATH:
         if (action == RGUI_ACTION_START)
            *g_settings.rgui_config_directory = '\0';
         break;
      case RGUI_SHADER_DIR_PATH:
         if (action == RGUI_ACTION_START)
            *g_settings.video.shader_dir = '\0';
         break;
      case RGUI_SYSTEM_DIR_PATH:
         if (action == RGUI_ACTION_START)
            *g_settings.system_directory = '\0';
         break;
      case RGUI_SETTINGS_VIDEO_ROTATION:
         if (action == RGUI_ACTION_START)
            g_settings.video.rotation = ORIENTATION_NORMAL;
         else if (action == RGUI_ACTION_LEFT)
         {
            if (g_settings.video.rotation > 0) g_settings.video.rotation--;
         }
         else if (action == RGUI_ACTION_RIGHT)
         {
            if (g_settings.video.rotation < LAST_ORIENTATION) g_settings.video.rotation++;
         }
#ifndef GEKKO
         video_set_rotation_func((g_settings.video.rotation + g_extern.system.rotation) % 4);
#endif
         break;

      case RGUI_SETTINGS_VIDEO_BILINEAR:
         if (action == RGUI_ACTION_START)
            g_settings.video.smooth = video_smooth;
         else
            g_settings.video.smooth = !g_settings.video.smooth;

         if (driver.video_poke && driver.video_poke->set_filtering)
            driver.video_poke->set_filtering(video_data, 1, g_settings.video.smooth);
         break;

      case RGUI_SETTINGS_DRIVER_VIDEO:
         if (action == RGUI_ACTION_LEFT)
            find_prev_video_driver();
         else if (action == RGUI_ACTION_RIGHT)
            find_next_video_driver();
         break;
      case RGUI_SETTINGS_DRIVER_AUDIO:
         if (action == RGUI_ACTION_LEFT)
            find_prev_audio_driver();
         else if (action == RGUI_ACTION_RIGHT)
            find_next_audio_driver();
         break;
      case RGUI_SETTINGS_DRIVER_AUDIO_DEVICE:
         if (action == RGUI_ACTION_OK)
         {
               menu_key_start_line(rgui, "Audio Device Name / IP: ", audio_device_callback);
         }
         else if (action == RGUI_ACTION_START)
            *g_settings.audio.device = '\0';
         break;
      case RGUI_SETTINGS_DRIVER_AUDIO_RESAMPLER:
         if (action == RGUI_ACTION_LEFT)
            find_prev_resampler_driver();
         else if (action == RGUI_ACTION_RIGHT)
            find_next_resampler_driver();
         break;
      case RGUI_SETTINGS_DRIVER_INPUT:
         if (action == RGUI_ACTION_LEFT)
            find_prev_input_driver();
         else if (action == RGUI_ACTION_RIGHT)
            find_next_input_driver();
         break;

      case RGUI_SETTINGS_VIDEO_GAMMA:
         if (action == RGUI_ACTION_START)
            g_extern.console_screen.gamma_correction = 0;
         else if (action == RGUI_ACTION_LEFT)
         {
            if (g_extern.console_screen.gamma_correction > 0)
               g_extern.console_screen.gamma_correction--;
         }
         else if (action == RGUI_ACTION_RIGHT)
         {
            if (g_extern.console_screen.gamma_correction < MAX_GAMMA_SETTING)
               g_extern.console_screen.gamma_correction++;
         }
         
         if (driver.video_poke && driver.video_poke->apply_state_changes)
            driver.video_poke->apply_state_changes(video_data);   
         break;

      case RGUI_SETTINGS_VIDEO_INTEGER_SCALE:
         if (action == RGUI_ACTION_START)
            g_settings.video.scale_integer = scale_integer;
         else if (action == RGUI_ACTION_LEFT ||
               action == RGUI_ACTION_RIGHT ||
               action == RGUI_ACTION_OK)
            g_settings.video.scale_integer = !g_settings.video.scale_integer;
#ifndef GEKKO
         if (driver.video_poke && driver.video_poke->apply_state_changes)
            driver.video_poke->apply_state_changes(video_data);
#endif            
         break;

      case RGUI_SETTINGS_VIDEO_FORCE_ASPECT:
         if (action == RGUI_ACTION_START)
            g_settings.video.force_aspect = force_aspect;
         else if (action == RGUI_ACTION_LEFT ||
               action == RGUI_ACTION_RIGHT ||
               action == RGUI_ACTION_OK)
            g_settings.video.force_aspect = !g_settings.video.force_aspect;
         
         if (driver.video_poke && driver.video_poke->apply_state_changes)
            driver.video_poke->apply_state_changes(video_data);          
         break;

      case RGUI_SETTINGS_VIDEO_ASPECT_RATIO:
         if (action == RGUI_ACTION_START)
            g_settings.video.aspect_ratio_idx = aspect_ratio_idx;
         else if (action == RGUI_ACTION_LEFT)
         {
            if (g_settings.video.aspect_ratio_idx > 0)
               g_settings.video.aspect_ratio_idx--;
         }
         else if (action == RGUI_ACTION_RIGHT)
         {
            if (g_settings.video.aspect_ratio_idx < LAST_ASPECT_RATIO)
               g_settings.video.aspect_ratio_idx++;
         }
#ifndef GEKKO
         if (driver.video_poke && driver.video_poke->set_aspect_ratio)
            driver.video_poke->set_aspect_ratio(video_data, g_settings.video.aspect_ratio_idx);
#endif
         break;

      case RGUI_SETTINGS_CUSTOM_VIEWPORT_X:
         if (action == RGUI_ACTION_START)
            g_extern.console_screen.custom_vp.x = 0;
         else if (action == RGUI_ACTION_LEFT)
               g_extern.console_screen.custom_vp.x--;
         else if (action == RGUI_ACTION_RIGHT)
               g_extern.console_screen.custom_vp.x++;

         break;

      case RGUI_SETTINGS_CUSTOM_VIEWPORT_Y:
         if (action == RGUI_ACTION_START)
            g_extern.console_screen.custom_vp.y = 0;
         else if (action == RGUI_ACTION_LEFT)
               g_extern.console_screen.custom_vp.y--;
         else if (action == RGUI_ACTION_RIGHT)
               g_extern.console_screen.custom_vp.y++;

         break;

      case RGUI_SETTINGS_CUSTOM_VIEWPORT_WIDTH:
         if (action == RGUI_ACTION_START)
         {
#ifdef GEKKO
            unsigned w, h;
            if (driver.video_poke && driver.video_poke->get_resolution_size)
               driver.video_poke->get_resolution_size(g_extern.console_screen.resolution_idx, &w, &h);
            g_extern.console_screen.custom_vp.width = w;
#else
            rarch_viewport_t vp;
            if (video_data && driver.video && driver.video->viewport_info)
               driver.video->viewport_info(video_data, &vp);
            g_extern.console_screen.custom_vp.width = vp.full_width;
#endif
         }
         else if (action == RGUI_ACTION_LEFT)
               g_extern.console_screen.custom_vp.width--;
         else if (action == RGUI_ACTION_RIGHT)
               g_extern.console_screen.custom_vp.width++;

         break;

      case RGUI_SETTINGS_CUSTOM_VIEWPORT_HEIGHT:
         if (action == RGUI_ACTION_START)
         {
#ifdef GEKKO
            unsigned w, h;
            if (driver.video_poke && driver.video_poke->get_resolution_size)
               driver.video_poke->get_resolution_size(g_extern.console_screen.resolution_idx, &w, &h);
            g_extern.console_screen.custom_vp.height = h;
#else
            rarch_viewport_t vp;
            if (video_data && driver.video && driver.video->viewport_info)
               driver.video->viewport_info(video_data, &vp);
            g_extern.console_screen.custom_vp.height = vp.full_height;
#endif
         }
         else if (action == RGUI_ACTION_LEFT)
               g_extern.console_screen.custom_vp.height--;
         else if (action == RGUI_ACTION_RIGHT)
               g_extern.console_screen.custom_vp.height++;

         break;

      case RGUI_SETTINGS_TOGGLE_FULLSCREEN:
         if (action == RGUI_ACTION_OK)
         {
            g_settings.video.fullscreen = !g_settings.video.fullscreen;
            rarch_reset_drivers();
         }
         break;

      case RGUI_SETTINGS_WINDOWED_FULLSCREEN:
         if (action == RGUI_ACTION_OK || action == RGUI_ACTION_LEFT || action == RGUI_ACTION_RIGHT)
         {
            g_settings.video.windowed_fullscreen = !g_settings.video.windowed_fullscreen;
            rarch_reset_drivers();
         }
         break;

#if defined(GEKKO)
      case RGUI_SETTINGS_VIDEO_RESOLUTION:
      {
         unsigned old_index = g_extern.console_screen.resolution_idx;
         if (action == RGUI_ACTION_LEFT)
         {
            if(g_extern.console_screen.resolution_idx > 0)
            {
               g_extern.console_screen.resolution_idx--;
            }
         }
         else if (action == RGUI_ACTION_RIGHT)
         {
            if (g_extern.console_screen.resolution_idx < GX_RESOLUTIONS_LAST)
            {
               g_extern.console_screen.resolution_idx++;
            }
         }
         else if (action == RGUI_ACTION_START)
         {
            g_extern.console_screen.resolution_idx = GX_RESOLUTIONS_AUTO;
         }
         else if (action == RGUI_ACTION_OK)
         {
            /* Display current game reported resolution */
            char msg[64];
            snprintf(msg, sizeof(msg), "Game internal resolution: %ux%u", g_extern.frame_cache.width, g_extern.frame_cache.height);
            msg_queue_push(g_extern.msg_queue, msg, 0, 80);
         }
         /* adjust refresh rate accordingly */
         if (old_index != g_extern.console_screen.resolution_idx)
            if (driver.video_poke && driver.video_poke->set_refresh_rate)
                  driver.video_poke->set_refresh_rate(video_data, g_extern.console_screen.resolution_idx);            
         break;
      }
#endif
#ifdef HW_RVL
      case RGUI_SETTINGS_VIDEO_VITRAP_FILTER:
         g_extern.console_screen.soft_filter_enable=!g_extern.console_screen.soft_filter_enable;

         if (driver.video_poke && driver.video_poke->apply_state_changes)
            driver.video_poke->apply_state_changes(video_data);
         break;
#endif

      case RGUI_SETTINGS_VIDEO_VSYNC:
         switch (action)
         {
            case RGUI_ACTION_START:
               g_settings.video.vsync = true;
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
               g_settings.video.crop_overscan = true;
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

      case RGUI_SETTINGS_VIDEO_WINDOW_SCALE_X:
      case RGUI_SETTINGS_VIDEO_WINDOW_SCALE_Y:
      {
         float *scale = setting == RGUI_SETTINGS_VIDEO_WINDOW_SCALE_X ? &g_settings.video.xscale : &g_settings.video.yscale;
         float old_scale = *scale;

         switch (action)
         {
            case RGUI_ACTION_START:
               *scale = 3.0f;
               break;

            case RGUI_ACTION_LEFT:
               *scale -= 1.0f;
               break;

            case RGUI_ACTION_RIGHT:
               *scale += 1.0f;
               break;

            default:
               break;
         }

         *scale = roundf(*scale);
         *scale = max(*scale, 1.0f);

         if (old_scale != *scale && !g_settings.video.fullscreen)
            rarch_reset_drivers();

         break;
      }

#ifdef HAVE_THREADS
      case RGUI_SETTINGS_VIDEO_THREADED:
      {
         bool old = g_settings.video.threaded;
         if (action == RGUI_ACTION_OK ||
               action == RGUI_ACTION_LEFT ||
               action == RGUI_ACTION_RIGHT)
            g_settings.video.threaded = !g_settings.video.threaded;
         else if (action == RGUI_ACTION_START)
            g_settings.video.threaded = false;

         if (g_settings.video.threaded != old)
            rarch_reset_drivers();
         break;
      }
#endif

      case RGUI_SETTINGS_VIDEO_SWAP_INTERVAL:
      {
         unsigned old = g_settings.video.swap_interval;
         switch (action)
         {
            case RGUI_ACTION_START:
               g_settings.video.swap_interval = 1;
               break;

            case RGUI_ACTION_LEFT:
               g_settings.video.swap_interval--;
               break;

            case RGUI_ACTION_RIGHT:
            case RGUI_ACTION_OK:
               g_settings.video.swap_interval++;
               break;

            default:
               break;
         }

         g_settings.video.swap_interval = min(g_settings.video.swap_interval, 4);
         g_settings.video.swap_interval = max(g_settings.video.swap_interval, 1);
         if (old != g_settings.video.swap_interval && driver.video && video_data)
            video_set_nonblock_state_func(false); // This will update the current swap interval. Since we're in RGUI now, always apply VSync.

         break;
      }
#ifdef HAVE_SHADER_MANAGER
      case RGUI_SETTINGS_SHADER_PASSES:
         switch (action)
         {
            case RGUI_ACTION_START:
               rgui->shader.passes = 0;
               break;

            case RGUI_ACTION_LEFT:
               if (rgui->shader.passes)
               {
                  rgui->shader.passes--;
                  rgui->need_refresh = true;
               }
               break;

            case RGUI_ACTION_RIGHT:
            case RGUI_ACTION_OK:
               if (rgui->shader.passes < RGUI_MAX_SHADERS)
               {
                  rgui->shader.passes++;
                  rgui->need_refresh = true;
               }
               break;

            default:
               break;
         }

         rgui->need_refresh = true;
         
         break;
      case RGUI_SETTINGS_SHADER_APPLY:
      {
         if (!driver.video || !driver.video->set_shader || action != RGUI_ACTION_OK)
            return 0;

         RARCH_LOG("Applying shader ...\n");

         enum rarch_shader_type type = shader_manager_get_type(&rgui->shader);

         if (rgui->shader.passes && type != RARCH_SHADER_NONE)
            shader_manager_save_preset(rgui, NULL, true);
         else
         {
            type = gfx_shader_parse_type("", DEFAULT_SHADER_TYPE);
            if (type == RARCH_SHADER_NONE)
            {
#if defined(HAVE_GLSL)
               type = RARCH_SHADER_GLSL;
#elif defined(HAVE_CG) || defined(HAVE_HLSL)
               type = RARCH_SHADER_CG;
#endif
            }
            shader_manager_set_preset(NULL, type, NULL);
         }
         break;
      }
      case RGUI_SETTINGS_SHADER_PRESET_SAVE:
         if (action == RGUI_ACTION_OK)
         {
               menu_key_start_line(rgui, "Preset Filename: ", preset_filename_callback);
         }
         break;
#endif
      case RGUI_SETTINGS_PAUSE_IF_WINDOW_FOCUS_LOST:
         if (action == RGUI_ACTION_OK || action == RGUI_ACTION_LEFT || action == RGUI_ACTION_RIGHT)
            g_settings.pause_nonactive = !g_settings.pause_nonactive;
         else if (action == RGUI_ACTION_START)
            g_settings.pause_nonactive = false;
         break;
      case RGUI_SETTINGS_WINDOW_COMPOSITING_ENABLE:
         if (action == RGUI_ACTION_OK || action == RGUI_ACTION_LEFT || action == RGUI_ACTION_RIGHT)
         {
            g_settings.video.disable_composition = !g_settings.video.disable_composition;
            rarch_reset_drivers();
         }
         else if (action == RGUI_ACTION_START)
         {
            g_settings.video.disable_composition = false;
            rarch_reset_drivers();
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
         strlcpy(type_str, rotation_lut[g_settings.video.rotation],
               type_str_size);
         break;
      case RGUI_SETTINGS_VIDEO_VITRAP_FILTER:
         snprintf(type_str, type_str_size, g_extern.console_screen.soft_filter_enable ? "ON" : "OFF");
         break;
      case RGUI_SETTINGS_VIDEO_BILINEAR:
         strlcpy(type_str, g_settings.video.smooth ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_VIDEO_GAMMA:
         snprintf(type_str, type_str_size, "%d", g_extern.console_screen.gamma_correction);
         break;
      case RGUI_SETTINGS_VIDEO_VSYNC:
         strlcpy(type_str, g_settings.video.vsync ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_VIDEO_SWAP_INTERVAL:
         snprintf(type_str, type_str_size, "%u", g_settings.video.swap_interval);
         break;
      case RGUI_SETTINGS_VIDEO_THREADED:
         strlcpy(type_str, g_settings.video.threaded ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_VIDEO_WINDOW_SCALE_X:
         snprintf(type_str, type_str_size, "%.1fx", g_settings.video.xscale);
         break;
      case RGUI_SETTINGS_VIDEO_WINDOW_SCALE_Y:
         snprintf(type_str, type_str_size, "%.1fx", g_settings.video.yscale);
         break;
      case RGUI_SETTINGS_VIDEO_CROP_OVERSCAN:
         strlcpy(type_str, g_settings.video.crop_overscan ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_DRIVER_VIDEO:
         strlcpy(type_str, g_settings.video.driver, type_str_size);
         break;
      case RGUI_SETTINGS_DRIVER_AUDIO:
         strlcpy(type_str, g_settings.audio.driver, type_str_size);
         break;
      case RGUI_SETTINGS_DRIVER_AUDIO_DEVICE:
         strlcpy(type_str, g_settings.audio.device, type_str_size);
         break;
      case RGUI_SETTINGS_DRIVER_AUDIO_RESAMPLER:
         strlcpy(type_str, g_settings.audio.resampler, type_str_size);
         break;
      case RGUI_SETTINGS_DRIVER_INPUT:
         strlcpy(type_str, g_settings.input.driver, type_str_size);
         break;
      case RGUI_SETTINGS_VIDEO_REFRESH_RATE:
            snprintf(type_str, type_str_size, "%3.2f", g_settings.video.refresh_rate);
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
         snprintf(type_str, type_str_size, "%d", g_extern.console_screen.custom_vp.x);
         break;
      case RGUI_SETTINGS_CUSTOM_VIEWPORT_Y:
         snprintf(type_str, type_str_size, "%d", g_extern.console_screen.custom_vp.y);
         break;
      case RGUI_SETTINGS_CUSTOM_VIEWPORT_WIDTH:
         snprintf(type_str, type_str_size, "%d", g_extern.console_screen.custom_vp.width);
         break;
      case RGUI_SETTINGS_CUSTOM_VIEWPORT_HEIGHT:
         snprintf(type_str, type_str_size, "%d", g_extern.console_screen.custom_vp.height);
         break;
#if defined(GEKKO)
      case RGUI_SETTINGS_VIDEO_RESOLUTION:
         if (driver.video_poke && driver.video_poke->get_resolution)
            snprintf(type_str, type_str_size, "%s %3.2fhz", driver.video_poke->get_resolution(g_extern.console_screen.resolution_idx), 
                                                           g_settings.video.refresh_rate);
         break;
#endif
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
#ifdef HAVE_SCREENSHOTS
      case RGUI_SETTINGS_GPU_SCREENSHOT:
         strlcpy(type_str, g_settings.video.gpu_screenshot ? "ON" : "OFF", type_str_size);
         break;
#endif
      case RGUI_SETTINGS_REWIND_GRANULARITY:
         snprintf(type_str, type_str_size, "%u", g_settings.rewind_granularity);
         break;
      case RGUI_SETTINGS_CONFIG_SAVE_ON_EXIT:
         strlcpy(type_str, g_extern.config_save_on_exit ? "ON" : "OFF", type_str_size);
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
      case RGUI_SETTINGS_SRAM_AUTOSAVE:
         if (g_settings.autosave_interval)
            snprintf(type_str, type_str_size, "%u seconds", g_settings.autosave_interval);
         else
            strlcpy(type_str, "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_SAVESTATE_SAVE:
      case RGUI_SETTINGS_SAVESTATE_LOAD:
         if (g_extern.state_slot < 0)
            strlcpy(type_str, "-1 (auto)", type_str_size);
         else
            snprintf(type_str, type_str_size, "%d", g_extern.state_slot);
         break;
      case RGUI_SETTINGS_AUDIO_MUTE:
         strlcpy(type_str, g_extern.audio_data.mute ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_AUDIO_SYNC:
         strlcpy(type_str, g_settings.audio.sync ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_AUDIO_CONTROL_RATE_DELTA:
         snprintf(type_str, type_str_size, "%.3f", g_settings.audio.rate_control_delta);
         break;
      case RGUI_SETTINGS_DEBUG_TEXT:
         snprintf(type_str, type_str_size, (g_settings.fps_show) ? "ON" : "OFF");
         break;
      case RGUI_BROWSER_DIR_PATH:
         strlcpy(type_str, *g_settings.rgui_content_directory ? g_settings.rgui_content_directory : "<default>", type_str_size);
         break;
#ifdef HAVE_SCREENSHOTS
      case RGUI_SCREENSHOT_DIR_PATH:
         strlcpy(type_str, *g_settings.screenshot_directory ? g_settings.screenshot_directory : "<ROM dir>", type_str_size);
         break;
#endif
      case RGUI_SAVEFILE_DIR_PATH:
         strlcpy(type_str, *g_extern.savefile_dir ? g_extern.savefile_dir : "<ROM dir>", type_str_size);
         break;
#ifdef HAVE_OVERLAY
      case RGUI_OVERLAY_DIR_PATH:
         strlcpy(type_str, *g_extern.overlay_dir ? g_extern.overlay_dir : "<default>", type_str_size);
         break;
#endif
#ifndef RARCH_CONSOLE
      case RGUI_AUTOCONF_DIR_PATH:
         strlcpy(type_str, *g_settings.input.autoconfig_dir ? g_settings.input.autoconfig_dir : "<None>", type_str_size);
         break;
#endif
      case RGUI_SAVESTATE_DIR_PATH:
         strlcpy(type_str, *g_extern.savestate_dir ? g_extern.savestate_dir : "<ROM dir>", type_str_size);
         break;
      case RGUI_LIBRETRO_DIR_PATH:
         strlcpy(type_str, *rgui->libretro_dir ? rgui->libretro_dir : "<None>", type_str_size);
         break;
      case RGUI_LIBRETRO_INFO_DIR_PATH:
         strlcpy(type_str, *g_settings.libretro_info_path ? g_settings.libretro_info_path : "<Core dir>", type_str_size);
         break;
      case RGUI_CONFIG_DIR_PATH:
         strlcpy(type_str, *g_settings.rgui_config_directory ? g_settings.rgui_config_directory : "<default>", type_str_size);
         break;
      case RGUI_SHADER_DIR_PATH:
         strlcpy(type_str, *g_settings.video.shader_dir ? g_settings.video.shader_dir : "<default>", type_str_size);
         break;
      case RGUI_SYSTEM_DIR_PATH:
         strlcpy(type_str, *g_settings.system_directory ? g_settings.system_directory : "<ROM dir>", type_str_size);
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
      case RGUI_SETTINGS_CORE_INFO:
      case RGUI_SETTINGS_TOGGLE_FULLSCREEN:
      case RGUI_SETTINGS_VIDEO_OPTIONS:
      case RGUI_SETTINGS_AUDIO_OPTIONS:
      case RGUI_SETTINGS_DISK_OPTIONS:
#ifdef HAVE_SHADER_MANAGER
      case RGUI_SETTINGS_SHADER_OPTIONS:
      case RGUI_SETTINGS_SHADER_PRESET:
#endif
      case RGUI_SETTINGS_CONFIG_OPTIONS:
      case RGUI_SETTINGS_SAVE_OPTIONS:
      case RGUI_SETTINGS_SHADER_PRESET_SAVE:
      case RGUI_SETTINGS_CORE:
      case RGUI_SETTINGS_DISK_APPEND:
      case RGUI_SETTINGS_INPUT_OPTIONS:
      case RGUI_SETTINGS_PATH_OPTIONS:
      case RGUI_SETTINGS_OVERLAY_OPTIONS:
      case RGUI_SETTINGS_OPTIONS:
      case RGUI_SETTINGS_DRIVERS:
      case RGUI_SETTINGS_CUSTOM_BIND_ALL:
      case RGUI_SETTINGS_CUSTOM_BIND_DEFAULT_ALL:
      case RGUI_SETTINGS_BIND_HOTKEYS:
      case RGUI_SETTINGS_BIND_PLAYER_KEYS:
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
         strlcpy(type_str, path_basename(g_settings.input.overlay), type_str_size);
         break;
      case RGUI_SETTINGS_OVERLAY_OPACITY:
         snprintf(type_str, type_str_size, "%.2f", g_settings.input.overlay_opacity);
         break;
      case RGUI_SETTINGS_OVERLAY_SCALE:
         snprintf(type_str, type_str_size, "%.2f", g_settings.input.overlay_scale);
         break;
#endif
      case RGUI_SETTINGS_BIND_PLAYER:
         snprintf(type_str, type_str_size, "#%d", rgui->current_pad + 1);
         break;
      case RGUI_SETTINGS_BIND_DEVICE:
      {
         int map = g_settings.input.joypad_map[rgui->current_pad];
         if (map >= 0 && map < MAX_PLAYERS)
         {
            const char *device_name = g_settings.input.device_names[map];
            if (*device_name)
               strlcpy(type_str, device_name, type_str_size);
            else
               strlcpy(type_str, "N/A", type_str_size);
         }
         else
            strlcpy(type_str, "Disabled", type_str_size);
         break;
      }
      case RGUI_SETTINGS_BIND_ANALOG_MODE:
      {
         static const char *modes[] = {
            "None",
            "Left Analog",
            "Right Analog",
            "Dual Analog",
         };

         strlcpy(type_str, modes[g_settings.input.analog_dpad_mode[rgui->current_pad] % ANALOG_DPAD_LAST], type_str_size);
         break;
      }
      case RGUI_SETTINGS_BIND_DEVICE_TYPE:
      {
         const struct retro_controller_description *desc = NULL;
         if (rgui->current_pad < g_extern.system.num_ports)
         {
            desc = libretro_find_controller_description(&g_extern.system.ports[rgui->current_pad],
                  g_settings.input.libretro_device[rgui->current_pad]);
         }

         const char *name = desc ? desc->desc : NULL;
         if (!name) // Find generic name.
         {
            switch (g_settings.input.libretro_device[rgui->current_pad])
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
      case RGUI_SETTINGS_DEVICE_AUTODETECT_ENABLE:
         strlcpy(type_str, g_settings.input.autodetect_enable ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_MENU_ALL_PLAYERS_ENABLE:
         strlcpy(type_str, g_settings.input.menu_all_players_enable ? "ON" : "OFF", type_str_size);
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
#ifndef RARCH_CONSOLE
      case RGUI_SETTINGS_BIND_FULLSCREEN_TOGGLE_KEY:
      case RGUI_SETTINGS_BIND_SHADER_NEXT:
      case RGUI_SETTINGS_BIND_SHADER_PREV:
      case RGUI_SETTINGS_BIND_DSP_CONFIG:
      case RGUI_SETTINGS_BIND_OVERLAY_NEXT:
      case RGUI_SETTINGS_BIND_GRAB_MOUSE_TOGGLE:
#endif
#ifdef HAVE_MENU
      case RGUI_SETTINGS_BIND_MENU_TOGGLE:
#endif
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
         input_get_bind_string(type_str, &g_settings.input.binds[rgui->current_pad][type - RGUI_SETTINGS_BIND_BEGIN], 
               rgui->current_pad, type_str_size);
         break;
      case RGUI_SETTINGS_AUDIO_VOLUME:
         snprintf(type_str, type_str_size, "%.1f dB", g_extern.audio_data.volume_db);
         break;
      case RGUI_SETTINGS_PAUSE_IF_WINDOW_FOCUS_LOST:
         strlcpy(type_str, g_settings.pause_nonactive ? "ON" : "OFF", type_str_size);
         break;
      case RGUI_SETTINGS_WINDOW_COMPOSITING_ENABLE:
         strlcpy(type_str, g_settings.video.disable_composition ? "OFF" : "ON", type_str_size);
         break;
      case RGUI_SETTINGS_WINDOWED_FULLSCREEN:
         strlcpy(type_str, g_settings.video.windowed_fullscreen ? "ON" : "OFF", type_str_size);
         break;
      default:
         *type_str = '\0';
         *w = 0;
         break;
   }
}
