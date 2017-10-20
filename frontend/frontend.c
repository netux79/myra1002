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

#if defined(HAVE_MENU)
#include "menu/menu_input_line_cb.h"
#include "menu/menu_common.h"
#endif

#include "../file_ext.h"

#ifdef RARCH_CONSOLE
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
#endif

#define returntype int
#define signature_expand() argc, argv
#define returnfunc() return 0
#define return_negative() return 1
#define return_var(var) return var
#define declare_argc()
#define declare_argv()
#define args_initial_ptr() NULL

#define ra_preinited false

#ifdef RARCH_CONSOLE
#define attempt_load_game false
#else
#define attempt_load_game true
#endif

#if !defined(RARCH_CONSOLE)
#define attempt_load_game_push_history true
#else
#define attempt_load_game_push_history false
#endif

#if defined(RARCH_CONSOLE)
#define load_dummy_on_core_shutdown false
#else
#define load_dummy_on_core_shutdown true
#endif

int main_entry_iterate(int argc, char *argv[], void* args)
{
   int i;
   static retro_keyboard_event_t key_event;

   if (g_extern.system.core_shutdown)
   {
#ifdef HAVE_MENU
      // Load dummy core instead of exiting RetroArch completely.
      if (load_dummy_on_core_shutdown)
         load_menu_game_prepare_dummy();
      else
#endif
         return 1;
   }
#ifndef RARCH_CONSOLE
   else if (g_extern.lifecycle_state & (1ULL << MODE_EXIT))
   {
      return 1;
   }
#endif
   else if (g_extern.lifecycle_state & (1ULL << MODE_LOAD_GAME))
   {
      load_menu_game_prepare(driver.video_data);

      if (load_menu_game())
      {
         g_extern.lifecycle_state |= (1ULL << MODE_GAME);
#ifndef GEKKO
         if (driver.video_poke && driver.video_poke->set_aspect_ratio)
            driver.video_poke->set_aspect_ratio(driver.video_data, g_settings.video.aspect_ratio_idx);
#endif
      }
      else
      {
         /* If ROM load fails, we exit RetroArch. On console it 
          * makes more sense to go back to menu though ... */
#ifdef RARCH_CONSOLE        
         g_extern.lifecycle_state = (1ULL << MODE_MENU_PREINIT);
#else
         g_extern.lifecycle_state = (1ULL << MODE_EXIT);
#endif         
      }

      g_extern.lifecycle_state &= ~(1ULL << MODE_LOAD_GAME);
   }
   else if (g_extern.lifecycle_state & (1ULL << MODE_GAME))
#ifdef GEKKO
   {
      g_extern.lifecycle_state |= (1ULL << MODE_GAME_RUN);
      // setup the screen for the current core
      if (driver.video_poke && driver.video_poke->update_screen_config)
         driver.video_poke->update_screen_config(driver.video_data,
                                                 g_extern.console_screen.resolution_idx,
                                                 g_settings.video.aspect_ratio_idx,
                                                 g_settings.video.scale_integer,
                                                 g_settings.video.rotation,
                                                 true);

      g_extern.lifecycle_state &= ~(1ULL << MODE_GAME);
   }
   else if (g_extern.lifecycle_state & (1ULL << MODE_GAME_RUN))
#endif
   {
      bool r;
      if (g_extern.is_paused && !g_extern.is_oneshot)
         r = rarch_main_idle_iterate();
      else
         r = rarch_main_iterate();

      if (r)
      {
         if (frontend_ctx && frontend_ctx->process_events)
            frontend_ctx->process_events(args);
      }
      else
#ifdef GEKKO
         g_extern.lifecycle_state &= ~(1ULL << MODE_GAME_RUN);
#else
         g_extern.lifecycle_state &= ~(1ULL << MODE_GAME);
#endif
   }
#ifdef HAVE_MENU
   else if (g_extern.lifecycle_state & (1ULL << MODE_MENU_PREINIT))
   {
      // Menu should always run with vsync on.
      video_set_nonblock_state_func(false);
#ifdef GEKKO
      // Change video resolution to the preferred mode
      if (driver.video_poke && driver.video_poke->update_screen_config)
         driver.video_poke->update_screen_config(driver.video_data, GX_RESOLUTIONS_AUTO, ASPECT_RATIO_4_3,
                                                 false, ORIENTATION_NORMAL, false);
#endif
      // Stop all rumbling when entering RGUI.
      for (i = 0; i < MAX_PLAYERS; i++)
      {
         driver_set_rumble_state(i, RETRO_RUMBLE_STRONG, 0);
         driver_set_rumble_state(i, RETRO_RUMBLE_WEAK, 0);
      }

      // Override keyboard callback to redirect to menu instead.
      // We'll use this later for something ...
      // FIXME: This should probably be moved to menu_common somehow.
      key_event = g_extern.system.key_event;
      g_extern.system.key_event = menu_key_event;

      if (driver.audio_data)
         audio_stop_func();

      rgui->need_refresh= true;
      rgui->old_input_state |= 1ULL << RARCH_MENU_TOGGLE;

      g_extern.lifecycle_state &= ~(1ULL << MODE_MENU_PREINIT);
      g_extern.lifecycle_state |= (1ULL << MODE_MENU);
   }
   else if (g_extern.lifecycle_state & (1ULL << MODE_MENU))
   {
      if (menu_iterate(driver.video_data))
      {
         if (frontend_ctx && frontend_ctx->process_events)
            frontend_ctx->process_events(args);
      }
      else
      {
         g_extern.lifecycle_state &= ~(1ULL << MODE_MENU);
         driver_set_nonblock_state(driver.nonblock_state);

         if (driver.audio_data && !g_extern.audio_data.mute && !audio_start_func())
         {
            RARCH_ERR("Failed to resume audio driver. Will continue without audio.\n");
            g_extern.audio_active = false;
         }

         /* Restore libretro keyboard callback. */
         g_extern.system.key_event = key_event;
      }
   }
#endif
   else
      return 1;

   return 0;
}

void main_exit(void* args)
{
#ifdef HAVE_MENU
   g_extern.system.core_shutdown = false;

   menu_free(driver.video_data);

   if (g_extern.config_save_on_exit)
   {
      /* Flush out the specific config. */
      if (*g_extern.specific_config_path)
         config_save_file(g_extern.specific_config_path);

      /* save the global configurations aswell */
      if(*g_extern.config_path)
         global_config_save_file(g_extern.config_path);
   }
#endif

   if (g_extern.main_is_init)
      rarch_main_deinit();
   rarch_deinit_msg_queue();
   global_uninit_drivers();

#ifdef PERF_TEST
   rarch_perf_log();
#endif

   if (frontend_ctx && frontend_ctx->deinit)
      frontend_ctx->deinit(args);

   if (g_extern.lifecycle_state & (1ULL << MODE_EXITSPAWN) && frontend_ctx
         && frontend_ctx->exitspawn)
      frontend_ctx->exitspawn();

   rarch_main_clear_state();

   if (frontend_ctx && frontend_ctx->shutdown)
      frontend_ctx->shutdown(false);
}

returntype main(int argc, char *argv[])
{
   declare_argc();
   declare_argv();
   void* args = (void*)args_initial_ptr();


   frontend_ctx = (frontend_ctx_driver_t*)frontend_ctx_init_first();
   if (frontend_ctx && frontend_ctx->init)
      frontend_ctx->init(args);

   if (!ra_preinited)
   {
      rarch_main_clear_state();
      rarch_init_msg_queue();
   }

   if (frontend_ctx && frontend_ctx->environment_get)
   {
      frontend_ctx->environment_get(argc, argv, args);
#ifdef RARCH_CONSOLE
      rarch_get_environment_console(argv[0]);
#endif
   }

   if (attempt_load_game)
   {
      int init_ret;
      if ((init_ret = rarch_main_init(argc, argv))) return_var(init_ret);
   }

#if defined(HAVE_MENU)
   menu_init(driver.video_data);

   if (frontend_ctx && frontend_ctx->process_args)
      frontend_ctx->process_args(argc, argv, args);

   /* Attempt to load a game if a path is provided */
   if (*g_extern.fullpath)
      g_extern.lifecycle_state |= (1ULL << MODE_LOAD_GAME);

   if (attempt_load_game_push_history)
   {
      // If we started a ROM directly from command line,
      // push it to ROM history.
      if (!g_extern.libretro_dummy)
         menu_rom_history_push_current();
   }

   while (!main_entry_iterate(signature_expand(), args));
#else
   while ((g_extern.is_paused && !g_extern.is_oneshot) ? rarch_main_idle_iterate() : rarch_main_iterate());
#endif

   main_exit(args);

   returnfunc();
}
