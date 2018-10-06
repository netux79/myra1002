/* RetroArch - A frontend for libretro.
 * Copyright (C) 2010-2014 - Hans-Kristian Arntzen
 * Copyright (C) 2011-2014 - Daniel De Matteis
 * Copyright (C) 2012-2014 - Michael Lelli
 *
 * RetroArch is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Found-
 * ation, either version 3 of the License, or (at your option) any later version.
 *
 * RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with RetroArch.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "frontend.h"
#include "../general.h"
#include "../conf/config_file.h"
#include "../file.h"

#include "frontend_context.h"
frontend_ctx_driver_t *frontend_ctx;

#include "menu/menu_common.h"
#include "../file_ext.h"
#include "../config.def.h"

default_paths_t default_paths;

static void rarch_get_environment_console(char *path)
{
   path_mkdir(default_paths.port_dir);
   path_mkdir(default_paths.system_dir);
   path_mkdir(default_paths.savestate_dir);
   path_mkdir(default_paths.sram_dir);

   /* Initially set the libretro path to load
    * the correct configuration. */
   strlcpy(g_settings.libretro, path, sizeof(g_settings.libretro));

   config_load();

   init_libretro_sym(false);
   rarch_init_system_info();

   global_init_drivers();
}

int main_entry_iterate(int argc, char *argv[], void* args)
{
   bool gamerun;

   if (g_extern.system.core_shutdown)
      return 1; /* Exit Retroarch */
   else if (g_extern.lifecycle_state & (1ULL << MODE_LOAD_GAME))
   {
      load_menu_game_prepare(driver.video_data);

      if (load_menu_game())
         g_extern.lifecycle_state |= (1ULL << MODE_GAME);
      else
      {
         /* If ROM load fails, we exit RetroArch. On console it 
          * makes more sense to go back to menu though ... */
         g_extern.lifecycle_state = (1ULL << MODE_MENU_PREINIT);
      }

      g_extern.lifecycle_state &= ~(1ULL << MODE_LOAD_GAME);
   }
   else if (g_extern.lifecycle_state & (1ULL << MODE_GAME))
   {
      g_extern.lifecycle_state |= (1ULL << MODE_GAME_RUN);
      // setup the screen for the current core
      if (driver.video_poke && driver.video_poke->update_screen_config)
         driver.video_poke->update_screen_config(driver.video_data,
                                                 g_settings.video.resolution_idx,
                                                 g_settings.video.aspect_ratio_idx,
                                                 g_settings.video.scale_integer,
                                                 g_settings.video.rotation);

      g_extern.lifecycle_state &= ~(1ULL << MODE_GAME);
   }
   else if (g_extern.lifecycle_state & (1ULL << MODE_GAME_RUN))
   {
      gamerun = (g_extern.is_paused && !g_extern.is_oneshot) ? rarch_main_idle_iterate() : rarch_main_iterate();
      
      if (!gamerun) g_extern.lifecycle_state &= ~(1ULL << MODE_GAME_RUN);
   }
   else if (g_extern.lifecycle_state & (1ULL << MODE_MENU_PREINIT))
   {
      // Menu should always run with vsync on.
      video_set_nonblock_state_func(false);
      // Change video resolution to the preferred mode
      if (driver.video_poke && driver.video_poke->update_screen_config)
         driver.video_poke->update_screen_config(driver.video_data, GX_RESOLUTIONS_RGUI, ASPECT_RATIO_4_3,
                                                 false, ORIENTATION_NORMAL);
      // Stop all rumbling when entering RGUI.
      for (int i = 0; i < MAX_PLAYERS; i++)
      {
         driver_set_rumble_state(i, RETRO_RUMBLE_STRONG, 0);
         driver_set_rumble_state(i, RETRO_RUMBLE_WEAK, 0);
      }

      if (driver.audio_data)
         audio_stop_func();

      rgui->need_refresh = true;

      g_extern.lifecycle_state &= ~(1ULL << MODE_MENU_PREINIT);
      g_extern.lifecycle_state |= (1ULL << MODE_MENU);
   }
   else if (g_extern.lifecycle_state & (1ULL << MODE_MENU))
   {
      if (!menu_iterate(driver.video_data))
      {
         g_extern.lifecycle_state &= ~(1ULL << MODE_MENU);
         driver_set_nonblock_state(driver.nonblock_state);

         if (driver.audio_data && !g_settings.audio.mute && !audio_start_func())
         {
            RARCH_ERR("Failed to resume audio driver. Will continue without audio.\n");
            g_extern.audio_active = false;
         }
      }
   }
   else
      return 1;

   return 0;
}

void main_exit(void)
{
   g_extern.system.core_shutdown = false;

   menu_free(driver.video_data);

   if (g_settings.config_save_on_exit)
   {
      /* Flush out the specific config. */
      if (*g_extern.specific_config_path)
         config_save_file(g_extern.specific_config_path);

      /* save the global configurations aswell */
      if(*g_extern.config_path)
         global_config_save_file(g_extern.config_path);
   }

   if (g_extern.main_is_init)
      rarch_main_deinit();
   rarch_deinit_msg_queue();
   global_uninit_drivers();

#ifdef PERF_TEST
   rarch_perf_log();
#endif

   if (g_extern.lifecycle_state & (1ULL << MODE_EXITSPAWN) && frontend_ctx
         && frontend_ctx->exitspawn)
      frontend_ctx->exitspawn();

   rarch_main_clear_state();
}

int main(int argc, char *argv[])
{
   void* args = NULL;

   frontend_ctx = (frontend_ctx_driver_t*)frontend_ctx_init_first();
   if (frontend_ctx && frontend_ctx->init)
      frontend_ctx->init(args);

   rarch_main_clear_state();
   rarch_init_msg_queue();

   if (frontend_ctx && frontend_ctx->environment_get)
   {
      frontend_ctx->environment_get(argc, argv, args);
      rarch_get_environment_console(argv[0]);
   }

   menu_init(driver.video_data);

   if (frontend_ctx && frontend_ctx->process_args)
      frontend_ctx->process_args(argc, argv, args);

   /* Attempt to load a game if a path is provided */
   if (*g_extern.fullpath)
      g_extern.lifecycle_state |= (1ULL << MODE_LOAD_GAME);

   while (!main_entry_iterate(argc, argv, args));

   main_exit();

   return 0;
}
