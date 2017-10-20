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

#include "general.h"
#include "conf/config_file.h"
#include "conf/config_file_macros.h"
#include "compat/strl.h"
#include "config.def.h"
#include "file.h"
#include "compat/posix_string.h"
#include "input/input_common.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>

struct settings g_settings;
struct global g_extern;

const char *config_get_default_audio(void)
{
   switch (AUDIO_DEFAULT_DRIVER)
   {
      case AUDIO_OSS:
         return "oss";
      case AUDIO_ALSA:
         return "alsa";
      case AUDIO_ALSATHREAD:
         return "alsathread";
      case AUDIO_ROAR:
         return "roar";
      case AUDIO_COREAUDIO:
         return "coreaudio";
      case AUDIO_AL:
         return "openal";
      case AUDIO_SL:
         return "opensl";
      case AUDIO_SDL:
         return "sdl";
      case AUDIO_DSOUND:
         return "dsound";
      case AUDIO_XAUDIO:
         return "xaudio";
      case AUDIO_PULSE:
         return "pulse";
      case AUDIO_EXT:
         return "ext";
      case AUDIO_WII:
         return "gx";
      case AUDIO_NULL:
         return "null";
      default:
         return NULL;
   }
}

const char *config_get_default_video(void)
{
   switch (VIDEO_DEFAULT_DRIVER)
   {
      case VIDEO_GL:
         return "gl";
      case VIDEO_WII:
         return "gx";
      case VIDEO_D3D9:
         return "d3d";
      case VIDEO_XVIDEO:
         return "xvideo";
      case VIDEO_SDL:
         return "sdl";
      case VIDEO_EXT:
         return "ext";
      case VIDEO_VG:
         return "vg";
      case VIDEO_NULL:
         return "null";
      default:
         return NULL;
   }
}

const char *config_get_default_input(void)
{
   switch (INPUT_DEFAULT_DRIVER)
   {
      case INPUT_ANDROID:
         return "android_input";
      case INPUT_SDL:
         return "sdl";
      case INPUT_DINPUT:
         return "dinput";
      case INPUT_X:
         return "x";
      case INPUT_XINPUT:
         return "xinput";
      case INPUT_WII:
         return "gx";
      case INPUT_LINUXRAW:
         return "linuxraw";
      case INPUT_UDEV:
         return "udev";
      case INPUT_APPLE:
         return "apple_input";
      case INPUT_NULL:
         return "null";
      default:
         return NULL;
   }
}

void config_set_defaults(void)
{
   unsigned i, j;
   const char *def_video = config_get_default_video();
   const char *def_audio = config_get_default_audio();
   const char *def_input = config_get_default_input();

   if (def_video)
      strlcpy(g_settings.video.driver, def_video, sizeof(g_settings.video.driver));
   if (def_audio)
      strlcpy(g_settings.audio.driver, def_audio, sizeof(g_settings.audio.driver));
   if (def_input)
      strlcpy(g_settings.input.driver, def_input, sizeof(g_settings.input.driver));

   g_settings.video.xscale = xscale;
   g_settings.video.yscale = yscale;
   g_settings.video.fullscreen = g_extern.force_fullscreen ? true : fullscreen;
   g_settings.video.windowed_fullscreen = windowed_fullscreen;
   g_settings.video.monitor_index = monitor_index;
   g_settings.video.fullscreen_x = fullscreen_x;
   g_settings.video.fullscreen_y = fullscreen_y;
   g_settings.video.disable_composition = disable_composition;
   g_settings.video.vsync = vsync;
   g_settings.video.swap_interval = swap_interval;
   g_settings.video.threaded = video_threaded;
   g_settings.video.smooth = video_smooth;
   g_settings.video.force_aspect = force_aspect;
   g_settings.video.scale_integer = scale_integer;
   g_settings.video.crop_overscan = crop_overscan;
   g_settings.video.aspect_ratio = aspect_ratio;
   g_settings.video.aspect_ratio_auto = aspect_ratio_auto; // Let implementation decide if automatic, or 1:1 PAR.
   g_settings.video.aspect_ratio_idx = aspect_ratio_idx;
   g_settings.video.shader_enable = shader_enable;
   g_settings.video.allow_rotate = allow_rotate;

   g_settings.video.font_enable = font_enable;
   g_settings.video.font_size = font_size;
   g_settings.video.font_scale = font_scale;
   g_settings.video.msg_pos_x = message_pos_offset_x;
   g_settings.video.msg_pos_y = message_pos_offset_y;
   
   g_settings.video.msg_color_r = ((message_color >> 16) & 0xff) / 255.0f;
   g_settings.video.msg_color_g = ((message_color >>  8) & 0xff) / 255.0f;
   g_settings.video.msg_color_b = ((message_color >>  0) & 0xff) / 255.0f;

   g_settings.video.refresh_rate = refresh_rate;
   g_settings.video.gpu_screenshot = gpu_screenshot;
   g_settings.video.rotation = ORIENTATION_NORMAL;

   g_settings.audio.enable = audio_enable;
   g_settings.audio.out_rate = out_rate;
   g_settings.audio.block_frames = 0;
   g_settings.audio.in_rate = out_rate;
   if (audio_device)
      strlcpy(g_settings.audio.device, audio_device, sizeof(g_settings.audio.device));
   g_settings.audio.latency = out_latency;
   g_settings.audio.sync = audio_sync;
   g_settings.audio.rate_control = rate_control;
   g_settings.audio.rate_control_delta = rate_control_delta;
   g_settings.audio.volume = audio_volume;
   g_extern.audio_data.volume_db   = g_settings.audio.volume;
   g_extern.audio_data.volume_gain = db_to_gain(g_settings.audio.volume);
   strlcpy(g_settings.audio.resampler, audio_resampler, sizeof(g_settings.audio.resampler));

   g_settings.rewind_enable = rewind_enable;
   g_settings.rewind_buffer_size = rewind_buffer_size;
   g_settings.rewind_granularity = rewind_granularity;
   g_settings.slowmotion_ratio = slowmotion_ratio;
   g_settings.fastforward_ratio = fastforward_ratio;
   g_settings.pause_nonactive = pause_nonactive;
   g_settings.autosave_interval = autosave_interval;

   g_settings.block_sram_overwrite = block_sram_overwrite;
   g_settings.savestate_auto_index = savestate_auto_index;
   g_settings.savestate_auto_save  = savestate_auto_save;
   g_settings.savestate_auto_load  = savestate_auto_load;
   g_settings.stdin_cmd_enable     = stdin_cmd_enable;
   g_settings.game_history_size    = game_history_size;
   g_settings.libretro_log_level   = libretro_log_level;

   rarch_assert(sizeof(g_settings.input.binds[0]) >= sizeof(retro_keybinds_1));
   rarch_assert(sizeof(g_settings.input.binds[1]) >= sizeof(retro_keybinds_rest));
   memcpy(g_settings.input.binds[0], retro_keybinds_1, sizeof(retro_keybinds_1));
#ifdef RARCH_CONSOLE
   memcpy(g_settings.input.menu_binds, retro_keybinds_menu, sizeof(retro_keybinds_menu));
#endif
   for (i = 1; i < MAX_PLAYERS; i++)
      memcpy(g_settings.input.binds[i], retro_keybinds_rest, sizeof(retro_keybinds_rest));

   for (i = 0; i < MAX_PLAYERS; i++)
   {
      for (j = 0; j < RARCH_BIND_LIST_END; j++)
      {
         g_settings.input.autoconf_binds[i][j].joykey = NO_BTN;
         g_settings.input.autoconf_binds[i][j].joyaxis = AXIS_NONE;
      }
   }
   memset(g_settings.input.autoconfigured, 0, sizeof(g_settings.input.autoconfigured));

   // Verify that binds are in proper order.
   for (i = 0; i < MAX_PLAYERS; i++)
      for (j = 0; j < RARCH_BIND_LIST_END; j++)
         if (g_settings.input.binds[i][j].valid)
            rarch_assert(j == g_settings.input.binds[i][j].id);

   g_settings.input.axis_threshold = axis_threshold;
   g_settings.input.turbo_period = turbo_period;
   g_settings.input.turbo_duty_cycle = turbo_duty_cycle;
   g_settings.input.overlay_opacity = 0.7f;
   g_settings.input.overlay_scale = 1.0f;
   g_settings.input.debug_enable = input_debug_enable;
   g_settings.input.autodetect_enable = input_autodetect_enable;
   g_settings.input.menu_all_players_enable = menu_all_players_enable;   
   *g_settings.input.keyboard_layout = '\0';
#ifdef ANDROID
   g_settings.input.back_behavior = BACK_BUTTON_QUIT;
#endif

   for (i = 0; i < MAX_PLAYERS; i++)
   {
      g_settings.input.device_port[i] = i;
      g_settings.input.analog_dpad_mode[i] = ANALOG_DPAD_NONE;
      if (!g_extern.has_set_libretro_device[i])
         g_settings.input.libretro_device[i] = RETRO_DEVICE_JOYPAD;
   }

   g_extern.console_screen.custom_vp.width = 0;
   g_extern.console_screen.custom_vp.height = 0;
   g_extern.console_screen.custom_vp.x = 0;
   g_extern.console_screen.custom_vp.y = 0;

   // Make sure settings from other configs carry over into defaults for another config.
   if (!g_extern.has_set_save_path)
      *g_extern.savefile_dir = '\0';
   if (!g_extern.has_set_state_path)
      *g_extern.savestate_dir = '\0';
   *g_settings.libretro_info_path = '\0';
   *g_settings.core_options_path = '\0';
   *g_settings.game_history_path = '\0';
   *g_settings.screenshot_directory = '\0';
   *g_settings.system_directory = '\0';
#ifndef RARCH_CONSOLE
   *g_settings.input.autoconfig_dir = '\0';
#endif
   *g_settings.input.overlay = '\0';
   *g_settings.content_directory = '\0';
   *g_settings.video.shader_path = '\0';
   *g_settings.video.shader_dir = '\0';
#ifdef HAVE_MENU
   *g_settings.rgui_content_directory = '\0';
   *g_settings.rgui_config_directory = '\0';
#endif

#ifdef RARCH_CONSOLE
   g_extern.lifecycle_state |= (1ULL << MODE_MENU_PREINIT);

   strlcpy(g_settings.system_directory, default_paths.system_dir, sizeof(g_settings.system_directory));

   g_settings.video.msg_pos_x = 0.05f;
   g_settings.video.msg_pos_y = 0.90f;
   g_settings.video.aspect_ratio = -1.0f;

   g_extern.config_type = CONFIG_PER_CORE;

   // g_extern
   strlcpy(g_extern.savefile_dir, default_paths.sram_dir, sizeof(g_extern.savefile_dir));
   g_extern.console_screen.gamma_correction = DEFAULT_GAMMA;
   g_extern.console_screen.soft_filter_enable = true;

   g_extern.console_screen.resolution_idx = GX_RESOLUTIONS_AUTO;
   strlcpy(g_extern.savestate_dir, default_paths.savestate_dir, sizeof(g_extern.savestate_dir));

   g_extern.state_slot = 0;
   g_extern.audio_data.mute = 0;
   g_extern.verbose = true;
#endif
   
#ifdef HAVE_OVERLAY
   if (default_overlay_dir)
   {
      fill_pathname_expand_special(g_extern.overlay_dir, default_overlay_dir, sizeof(g_extern.overlay_dir));
#ifdef IOS
      fill_pathname_join(g_settings.input.overlay, g_extern.overlay_dir, "gamepads/snes/snes.cfg", sizeof(g_settings.input.overlay));
#endif
   }
#endif

   if (default_shader_dir)
      fill_pathname_expand_special(g_settings.video.shader_dir, default_shader_dir, sizeof(g_settings.video.shader_dir));

   if (default_libretro_info_path)
      fill_pathname_expand_special(g_settings.libretro_info_path, default_libretro_info_path, sizeof(g_settings.libretro_info_path));

   if (default_config_path)
      fill_pathname_expand_special(g_extern.config_path, default_config_path, sizeof(g_extern.config_path));

   g_extern.config_save_on_exit = config_save_on_exit;

   rarch_init_msg_queue();
}

static void calculate_specific_config_path(const char *in_basename)
{
#ifdef HAVE_MENU
   if (*g_settings.rgui_config_directory)
   {
      path_resolve_realpath(g_settings.rgui_config_directory, sizeof(g_settings.rgui_config_directory));
      strlcpy(g_extern.specific_config_path, g_settings.rgui_config_directory, sizeof(g_extern.specific_config_path));
   }
   else
#endif
   {
      // Use original config file's directory as a fallback.
      fill_pathname_basedir(g_extern.specific_config_path, g_extern.config_path, sizeof(g_extern.specific_config_path));
   }
   
   fill_pathname_dir(g_extern.specific_config_path, in_basename, ".cfg", sizeof(g_extern.specific_config_path));
}

void config_load(void)
{
   static bool first_time = true;
       
   if (first_time)
   {
       config_set_defaults();
       
       /* try loading the global configuration */
       RARCH_LOG("Loading config from: %s.\n", g_extern.config_path);
       if (!global_config_load_file(g_extern.config_path))
          RARCH_ERR("Couldn't find config at path: \"%s\"\n", g_extern.config_path);
       
       first_time = false;
   }
   else if (g_extern.config_save_on_exit)
   {
      /* Flush out configs before loading a new one. */
      if (*g_extern.specific_config_path)
         config_save_file(g_extern.specific_config_path);
      
      /* save global config aswell */
      if (*g_extern.config_path)
         global_config_save_file(g_extern.config_path);
   }

   /* Reset to default values */
   *g_extern.specific_config_path = '\0';
    g_extern.config_type = CONFIG_PER_CORE;
   
   /* First try per-game config (requires game already loaded) */
   if (*g_extern.basename)
   {
      calculate_specific_config_path(g_extern.basename);
      RARCH_LOG("Loading game-specific config from: %s.\n", g_extern.specific_config_path);
      if (config_load_file(g_extern.specific_config_path))
         g_extern.config_type = CONFIG_PER_GAME;
      else
         RARCH_WARN("Game-specific config not found, trying per-core config.\n");
   }
   
   /* Try the per-core config if the per-game was unsuccessful */
   if (*g_settings.libretro && g_extern.config_type != CONFIG_PER_GAME)
   {
      calculate_specific_config_path(g_settings.libretro);
      RARCH_LOG("Loading core-specific config from: %s.\n", g_extern.specific_config_path);
      if (!config_load_file(g_extern.specific_config_path))
         RARCH_WARN("Core-specific config not found, using defaults config.\n");
   }
   
   /* if reached this point and no config was loaded, defaults are used. 
    * also the per-core setup is left so next time it gets saved as is */
}

static config_file_t *open_default_config_file(void)
{
   config_file_t *conf = NULL;

#if defined(_WIN32)
   char conf_path[PATH_MAX];

   char app_path[PATH_MAX];
   fill_pathname_application_path(app_path, sizeof(app_path));
   fill_pathname_resolve_relative(conf_path, app_path, "retroarch.cfg", sizeof(conf_path));

   conf = config_file_new(conf_path);
   if (!conf)
   {
      const char *appdata = getenv("APPDATA");
      if (appdata)
      {
         fill_pathname_join(conf_path, appdata, "retroarch.cfg", sizeof(conf_path));
         conf = config_file_new(conf_path);
      }
   }

   // Try to create a new config file.
   if (!conf)
   {
      conf = config_file_new(NULL);
      bool saved = false;
      if (conf) // Since this is a clean config file, we can safely use config_save_on_exit.
      {
         fill_pathname_resolve_relative(conf_path, app_path, "retroarch.cfg", sizeof(conf_path));
         config_set_bool(conf, "config_save_on_exit", true);
         saved = config_file_write(conf, conf_path);
      }

      if (saved)
         RARCH_WARN("Created new config file in: \"%s\".\n", conf_path); // WARN here to make sure user has a good chance of seeing it.
      else
      {
         RARCH_ERR("Failed to create new config file in: \"%s\".\n", conf_path);
         config_file_free(conf);
         conf = NULL;
      }
   }

   if (conf)
      strlcpy(g_extern.config_path, conf_path, sizeof(g_extern.config_path));
#elif defined(OSX)
   char conf_path[PATH_MAX];
   const char *home = getenv("HOME");

   if (!home)
      return NULL;

   fill_pathname_join(conf_path, home, "Library/Application Support/RetroArch", sizeof(conf_path));
   path_mkdir(conf_path);
      
   fill_pathname_join(conf_path, conf_path, "retroarch.cfg", sizeof(conf_path));
   conf = config_file_new(conf_path);

   if (!conf)
   {
      conf = config_file_new(NULL);
      bool saved = false;
      if (conf)
      {
         config_set_bool(conf, "config_save_on_exit", true);
         saved = config_file_write(conf, conf_path);
      }
      
      if (saved)
         RARCH_WARN("Created new config file in: \"%s\".\n", conf_path); // WARN here to make sure user has a good chance of seeing it.
      else
      {
         RARCH_ERR("Failed to create new config file in: \"%s\".\n", conf_path);
         config_file_free(conf);
         conf = NULL;
      }
   }

   if (conf)
      strlcpy(g_extern.config_path, conf_path, sizeof(g_extern.config_path));

#else
   char conf_path[PATH_MAX];
   const char *xdg  = getenv("XDG_CONFIG_HOME");
   const char *home = getenv("HOME");

   // XDG_CONFIG_HOME falls back to $HOME/.config.
   if (xdg)
      fill_pathname_join(conf_path, xdg, "retroarch/retroarch.cfg", sizeof(conf_path));
   else if (home)
      fill_pathname_join(conf_path, home, ".config/retroarch/retroarch.cfg", sizeof(conf_path));

   if (xdg || home)
   {
      RARCH_LOG("Looking for config in: \"%s\".\n", conf_path);
      conf = config_file_new(conf_path);
   }

   // Fallback to $HOME/.retroarch.cfg.
   if (!conf && home)
   {
      fill_pathname_join(conf_path, home, ".retroarch.cfg", sizeof(conf_path));
      RARCH_LOG("Looking for config in: \"%s\".\n", conf_path);
      conf = config_file_new(conf_path);
   }

   // Try to create a new config file.
   if (!conf && (home || xdg))
   {
      // XDG_CONFIG_HOME falls back to $HOME/.config.
      if (xdg)
         fill_pathname_join(conf_path, xdg, "retroarch/retroarch.cfg", sizeof(conf_path));
      else if (home)
         fill_pathname_join(conf_path, home, ".config/retroarch/retroarch.cfg", sizeof(conf_path));

      char basedir[PATH_MAX];
      fill_pathname_basedir(basedir, conf_path, sizeof(basedir));

      if (path_mkdir(basedir))
      {
#ifndef GLOBAL_CONFIG_DIR
#define GLOBAL_CONFIG_DIR "/etc"
#endif
         char skeleton_conf[PATH_MAX];
         fill_pathname_join(skeleton_conf, GLOBAL_CONFIG_DIR, "retroarch.cfg", sizeof(skeleton_conf));
         conf = config_file_new(skeleton_conf);
         if (conf)
            RARCH_WARN("Using skeleton config \"%s\" as base for a new config file.\n", skeleton_conf);
         else
            conf = config_file_new(NULL);

         bool saved = false;
         if (conf)
         {
            config_set_bool(conf, "config_save_on_exit", true); // Since this is a clean config file, we can safely use config_save_on_exit.
            saved = config_file_write(conf, conf_path);
         }

         if (saved)
            RARCH_WARN("Created new config file in: \"%s\".\n", conf_path); // WARN here to make sure user has a good chance of seeing it.
         else
         {
            RARCH_ERR("Failed to create new config file in: \"%s\".\n", conf_path);
            config_file_free(conf);
            conf = NULL;
         }
      }
   }

   if (conf)
      strlcpy(g_extern.config_path, conf_path, sizeof(g_extern.config_path));
#endif
   
   return conf;
}

static void config_read_keybinds_conf(config_file_t *conf);

bool global_config_load_file(const char *path)
{
   char tmp_str[PATH_MAX];
   config_file_t *conf = NULL;

   if (*path)
   {
      conf = config_file_new(path);
      if (!conf)
         return false;
   }
   else
      conf = open_default_config_file();

   if (!conf)
      return false;

   /*if (g_extern.verbose)
   {
      RARCH_LOG_OUTPUT("=== Global Config ===\n");
      config_file_dump_all(conf, LOG_FILE);
      RARCH_LOG_OUTPUT("=== Global Config end ===\n");
   }*/

   CONFIG_GET_STRING(video.driver, "video_driver");
   CONFIG_GET_STRING(video.gl_context, "video_gl_context");
   CONFIG_GET_STRING(audio.driver, "audio_driver");
   CONFIG_GET_PATH(audio.dsp_plugin, "audio_dsp_plugin");
   CONFIG_GET_STRING(audio.device, "audio_device");
   CONFIG_GET_STRING(audio.resampler, "audio_resampler");
   CONFIG_GET_STRING(input.driver, "input_driver");
   CONFIG_GET_STRING(input.joypad_driver, "input_joypad_driver");
   CONFIG_GET_STRING(input.keyboard_layout, "input_keyboard_layout");

   CONFIG_GET_INT(libretro_log_level, "libretro_log_level");
   CONFIG_GET_BOOL_EXTERN(config_save_on_exit, "config_save_on_exit");
   CONFIG_GET_BOOL(video.gpu_screenshot, "video_gpu_screenshot");
   CONFIG_GET_BOOL(input.menu_all_players_enable, "menu_all_players_enable");
   CONFIG_GET_BOOL(input.debug_enable, "input_debug_enable");
   CONFIG_GET_BOOL(fps_show, "fps_show");
   CONFIG_GET_INT(game_history_size, "game_history_size");
   CONFIG_GET_INT(video.fullscreen_x, "video_fullscreen_x");
   CONFIG_GET_INT(video.fullscreen_y, "video_fullscreen_y");
   if (!g_extern.force_fullscreen)
      CONFIG_GET_BOOL(video.fullscreen, "video_fullscreen");
   CONFIG_GET_BOOL(video.windowed_fullscreen, "video_windowed_fullscreen");
   CONFIG_GET_INT(video.monitor_index, "video_monitor_index");
   CONFIG_GET_BOOL(video.disable_composition, "video_disable_composition");
   CONFIG_GET_BOOL(pause_nonactive, "pause_nonactive");
   CONFIG_GET_BOOL(video.threaded, "video_threaded");
   CONFIG_GET_PATH(video.font_path, "video_font_path");
   CONFIG_GET_FLOAT(video.font_size, "video_font_size");
   CONFIG_GET_BOOL(video.font_enable, "video_font_enable");
   CONFIG_GET_BOOL(video.font_scale, "video_font_scale");
   CONFIG_GET_BOOL(stdin_cmd_enable, "stdin_cmd_enable");

   CONFIG_GET_PATH(video.shader_dir, "video_shader_dir");
   if (!strcmp(g_settings.video.shader_dir, "default"))
      *g_settings.video.shader_dir = '\0';
   CONFIG_GET_PATH(libretro_info_path, "libretro_info_path");
   CONFIG_GET_PATH(core_options_path, "core_options_path");
   CONFIG_GET_PATH(screenshot_directory, "screenshot_directory");
   if (*g_settings.screenshot_directory)
   {
      if (!strcmp(g_settings.screenshot_directory, "default"))
         *g_settings.screenshot_directory = '\0';
      else if (!path_is_directory(g_settings.screenshot_directory))
      {
         RARCH_WARN("screenshot_directory is not an existing directory, ignoring ...\n");
         *g_settings.screenshot_directory = '\0';
      }
   }
   CONFIG_GET_PATH(content_directory, "content_directory");
   if (!strcmp(g_settings.content_directory, "default"))
      *g_settings.content_directory = '\0';
#ifdef HAVE_MENU
   CONFIG_GET_PATH(rgui_content_directory, "rgui_browser_directory");
   if (!strcmp(g_settings.rgui_content_directory, "default"))
      *g_settings.rgui_content_directory = '\0';
   CONFIG_GET_PATH(rgui_config_directory, "rgui_config_directory");
   if (!strcmp(g_settings.rgui_config_directory, "default"))
      *g_settings.rgui_config_directory = '\0';
#endif
#ifdef HAVE_OVERLAY
   CONFIG_GET_PATH_EXTERN(overlay_dir, "overlay_directory");
   if (!strcmp(g_extern.overlay_dir, "default"))
      *g_extern.overlay_dir = '\0';
#endif
   CONFIG_GET_PATH(game_history_path, "game_history_path");
#ifndef RARCH_CONSOLE
   CONFIG_GET_PATH(input.autoconfig_dir, "joypad_autoconfig_dir");
#endif

   if (!g_extern.has_set_save_path && config_get_path(conf, "savefile_directory", tmp_str, sizeof(tmp_str)))
   {
      if (!strcmp(tmp_str, "default"))
         *g_extern.savefile_dir = '\0';
      else if (path_is_directory(tmp_str))
      {
         strlcpy(g_extern.savefile_dir, tmp_str, sizeof(g_extern.savefile_dir));
         strlcpy(g_extern.savefile_name_srm, tmp_str, sizeof(g_extern.savefile_name_srm));
         fill_pathname_dir(g_extern.savefile_name_srm, g_extern.basename, ".srm", sizeof(g_extern.savefile_name_srm));
      }
      else
         RARCH_WARN("savefile_directory is not a directory, ignoring ...\n");
   }

   if (!g_extern.has_set_state_path && config_get_path(conf, "savestate_directory", tmp_str, sizeof(tmp_str)))
   {
      if (!strcmp(tmp_str, "default"))
         *g_extern.savestate_dir = '\0';
      else if (path_is_directory(tmp_str))
      {
         strlcpy(g_extern.savestate_dir, tmp_str, sizeof(g_extern.savestate_dir));
         strlcpy(g_extern.savestate_name, tmp_str, sizeof(g_extern.savestate_name));
         fill_pathname_dir(g_extern.savestate_name, g_extern.basename, ".state", sizeof(g_extern.savestate_name));
      }
      else
         RARCH_WARN("savestate_directory is not a directory, ignoring ...\n");
   }

   if (!config_get_path(conf, "system_directory", g_settings.system_directory, sizeof(g_settings.system_directory)))
   {
      RARCH_WARN("system_directory is not set in config. Assuming system directory is same folder as game: \"%s\".\n",
            g_settings.system_directory);
   }

   if (!strcmp(g_settings.system_directory, "default"))
      *g_settings.system_directory = '\0';

   config_file_free(conf);
   return true;
}

bool config_load_file(const char *path)
{
   unsigned i;
   config_file_t *conf = NULL;

   if (*path)
   {
      conf = config_file_new(path);
      if (!conf)
         return false;
   }
   else
      return false;

   if (g_extern.verbose)
   {
      RARCH_LOG_OUTPUT("=== Config ===\n");
      config_file_dump_all(conf, LOG_FILE);
      RARCH_LOG_OUTPUT("=== Config end ===\n");
   }

   CONFIG_GET_FLOAT(video.xscale, "video_xscale");
   CONFIG_GET_FLOAT(video.yscale, "video_yscale");
   CONFIG_GET_BOOL(video.vsync, "video_vsync");
   CONFIG_GET_INT(video.swap_interval, "video_swap_interval");
   g_settings.video.swap_interval = max(g_settings.video.swap_interval, 1);
   g_settings.video.swap_interval = min(g_settings.video.swap_interval, 4);
   CONFIG_GET_BOOL(video.smooth, "video_smooth");
   CONFIG_GET_BOOL(video.force_aspect, "video_force_aspect");
   CONFIG_GET_BOOL(video.scale_integer, "video_scale_integer");
   CONFIG_GET_BOOL(video.crop_overscan, "video_crop_overscan");
   CONFIG_GET_FLOAT(video.aspect_ratio, "video_aspect_ratio");
   CONFIG_GET_INT(video.aspect_ratio_idx, "aspect_ratio_index");
   CONFIG_GET_BOOL(video.aspect_ratio_auto, "video_aspect_ratio_auto");
   CONFIG_GET_FLOAT(video.refresh_rate, "video_refresh_rate");
   CONFIG_GET_PATH(video.shader_path, "video_shader");
   CONFIG_GET_BOOL(video.shader_enable, "video_shader_enable");
   CONFIG_GET_BOOL(video.allow_rotate, "video_allow_rotate");
   CONFIG_GET_FLOAT(video.msg_pos_x, "video_message_pos_x");
   CONFIG_GET_FLOAT(video.msg_pos_y, "video_message_pos_y");
   CONFIG_GET_INT(video.rotation, "video_rotation");

#ifdef HAVE_SCALERS_BUILTIN
   CONFIG_GET_INT(video.filter_idx, "filter_index");
#endif

#ifdef RARCH_CONSOLE
   CONFIG_GET_INT_EXTERN(console_screen.gamma_correction, "gamma_correction");
   CONFIG_GET_BOOL_EXTERN(console_screen.soft_filter_enable, "soft_filter_enable");
   CONFIG_GET_INT_EXTERN(console_screen.resolution_idx, "current_resolution_id");
#endif
   CONFIG_GET_INT_EXTERN(state_slot, "state_slot");

   CONFIG_GET_INT_EXTERN(console_screen.custom_vp.x, "custom_viewport_x");
   CONFIG_GET_INT_EXTERN(console_screen.custom_vp.y, "custom_viewport_y");
   CONFIG_GET_INT_EXTERN(console_screen.custom_vp.width, "custom_viewport_width");
   CONFIG_GET_INT_EXTERN(console_screen.custom_vp.height, "custom_viewport_height");

   unsigned msg_color = 0;
   if (config_get_hex(conf, "video_message_color", &msg_color))
   {
      g_settings.video.msg_color_r = ((msg_color >> 16) & 0xff) / 255.0f;
      g_settings.video.msg_color_g = ((msg_color >>  8) & 0xff) / 255.0f;
      g_settings.video.msg_color_b = ((msg_color >>  0) & 0xff) / 255.0f;
   }

   CONFIG_GET_FLOAT(input.axis_threshold, "input_axis_threshold");

   for (i = 0; i < MAX_PLAYERS; i++)
   {
      char buf[64];
      snprintf(buf, sizeof(buf), "input_player%u_analog_dpad_mode", i + 1);
      CONFIG_GET_INT(input.analog_dpad_mode[i], buf);

      if (!g_extern.has_set_libretro_device[i])
      {
         snprintf(buf, sizeof(buf), "input_libretro_device_p%u", i + 1);
         CONFIG_GET_INT(input.libretro_device[i], buf);
      }
   }

   // Audio settings.
   CONFIG_GET_BOOL(audio.enable, "audio_enable");
   CONFIG_GET_INT(audio.out_rate, "audio_out_rate");
   CONFIG_GET_INT(audio.block_frames, "audio_block_frames");
   CONFIG_GET_INT(audio.latency, "audio_latency");
   CONFIG_GET_BOOL(audio.sync, "audio_sync");
   CONFIG_GET_BOOL(audio.rate_control, "audio_rate_control");
   CONFIG_GET_FLOAT(audio.rate_control_delta, "audio_rate_control_delta");
   CONFIG_GET_FLOAT(audio.volume, "audio_volume");
   g_extern.audio_data.volume_db   = g_settings.audio.volume;
   g_extern.audio_data.volume_gain = db_to_gain(g_settings.audio.volume);
#ifdef HAVE_OVERLAY
   CONFIG_GET_PATH(input.overlay, "input_overlay");
   CONFIG_GET_FLOAT(input.overlay_opacity, "input_overlay_opacity");
   CONFIG_GET_FLOAT(input.overlay_scale, "input_overlay_scale");
#endif
   CONFIG_GET_BOOL(rewind_enable, "rewind_enable");
   int buffer_size = 0;
   if (config_get_int(conf, "rewind_buffer_size", &buffer_size))
      g_settings.rewind_buffer_size = buffer_size * UINT64_C(1000000);
   CONFIG_GET_INT(rewind_granularity, "rewind_granularity");
   CONFIG_GET_FLOAT(slowmotion_ratio, "slowmotion_ratio");
   if (g_settings.slowmotion_ratio < 1.0f)
      g_settings.slowmotion_ratio = 1.0f;
   CONFIG_GET_FLOAT(fastforward_ratio, "fastforward_ratio");
   CONFIG_GET_INT(autosave_interval, "autosave_interval");
   CONFIG_GET_BOOL(block_sram_overwrite, "block_sram_overwrite");
   CONFIG_GET_BOOL(savestate_auto_index, "savestate_auto_index");
   CONFIG_GET_BOOL(savestate_auto_save, "savestate_auto_save");
   CONFIG_GET_BOOL(savestate_auto_load, "savestate_auto_load");
   CONFIG_GET_INT(input.turbo_period, "input_turbo_period");
   CONFIG_GET_INT(input.turbo_duty_cycle, "input_duty_cycle");
   CONFIG_GET_BOOL(input.autodetect_enable, "input_autodetect_enable");

#ifdef ANDROID
   CONFIG_GET_INT(input.back_behavior, "input_back_behavior");
   CONFIG_GET_INT(input.icade_profile[0], "input_autodetect_icade_profile_pad1");
   CONFIG_GET_INT(input.icade_profile[1], "input_autodetect_icade_profile_pad2");
   CONFIG_GET_INT(input.icade_profile[2], "input_autodetect_icade_profile_pad3");
   CONFIG_GET_INT(input.icade_profile[3], "input_autodetect_icade_profile_pad4");
#endif

   config_read_keybinds_conf(conf);

   config_file_free(conf);
   return true;
}

static void read_keybinds_keyboard(config_file_t *conf, unsigned player, unsigned index,
      struct retro_keybind *bind)
{
   if (input_config_bind_map[index].valid && input_config_bind_map[index].base)
   {
      const char *prefix = input_config_get_prefix(player, input_config_bind_map[index].meta);
      if (prefix)
         input_config_parse_key(conf, prefix, input_config_bind_map[index].base, bind);
   }
}

static void read_keybinds_button(config_file_t *conf, unsigned player, unsigned index,
      struct retro_keybind *bind)
{
   if (input_config_bind_map[index].valid && input_config_bind_map[index].base)
   {
      const char *prefix = input_config_get_prefix(player, input_config_bind_map[index].meta);
      if (prefix)
         input_config_parse_joy_button(conf, prefix, input_config_bind_map[index].base, bind);
   }
}

static void read_keybinds_axis(config_file_t *conf, unsigned player, unsigned index,
      struct retro_keybind *bind)
{
   if (input_config_bind_map[index].valid && input_config_bind_map[index].base)
   {
      const char *prefix = input_config_get_prefix(player, input_config_bind_map[index].meta);
      if (prefix)
         input_config_parse_joy_axis(conf, prefix, input_config_bind_map[index].base, bind);
   }
}

static void read_keybinds_player(config_file_t *conf, unsigned player)
{
   unsigned i;
   for (i = 0; input_config_bind_map[i].valid; i++)
   {
      struct retro_keybind *bind = &g_settings.input.binds[player][i];
      if (!bind->valid)
         continue;

      read_keybinds_keyboard(conf, player, i, bind);
      read_keybinds_button(conf, player, i, bind);
      read_keybinds_axis(conf, player, i, bind);
   }
}

static void config_read_keybinds_conf(config_file_t *conf)
{
   unsigned i;
   for (i = 0; i < MAX_PLAYERS; i++)
      read_keybinds_player(conf, i);
}

static void save_keybind_key(config_file_t *conf, const char *prefix, const char *base,
      const struct retro_keybind *bind)
{
   char key[64];
   snprintf(key, sizeof(key), "%s_%s", prefix, base);

   char btn[64];
   input_translate_rk_to_str(bind->key, btn, sizeof(btn));
   config_set_string(conf, key, btn);
}

#ifndef RARCH_CONSOLE
static void save_keybind_hat(config_file_t *conf, const char *key, const struct retro_keybind *bind)
{
   unsigned hat = GET_HAT(bind->joykey);
   const char *dir = NULL;

   switch (GET_HAT_DIR(bind->joykey))
   {
      case HAT_UP_MASK:
         dir = "up";
         break;

      case HAT_DOWN_MASK:
         dir = "down";
         break;

      case HAT_LEFT_MASK:
         dir = "left";
         break;

      case HAT_RIGHT_MASK:
         dir = "right";
         break;

      default:
         rarch_assert(0);
   }

   char config[16];
   snprintf(config, sizeof(config), "h%u%s", hat, dir);
   config_set_string(conf, key, config);
}
#endif

static void save_keybind_joykey(config_file_t *conf, const char *prefix, const char *base,
      const struct retro_keybind *bind)
{
   char key[64];
   snprintf(key, sizeof(key), "%s_%s_btn", prefix, base);

   if (bind->joykey == NO_BTN)
      config_set_string(conf, key, "nul");
#ifndef RARCH_CONSOLE // Consoles don't understand hats.
   else if (GET_HAT_DIR(bind->joykey))
      save_keybind_hat(conf, key, bind);
#endif
   else
      config_set_uint64(conf, key, bind->joykey);
}

static void save_keybind_axis(config_file_t *conf, const char *prefix, const char *base,
      const struct retro_keybind *bind)
{
   char key[64];
   snprintf(key, sizeof(key), "%s_%s_axis", prefix, base);

   unsigned axis = 0;
   char dir = '\0';

   if (bind->joyaxis == AXIS_NONE)
      config_set_string(conf, key, "nul");
   else if (AXIS_NEG_GET(bind->joyaxis) != AXIS_DIR_NONE)
   {
      dir = '-';
      axis = AXIS_NEG_GET(bind->joyaxis);
   }
   else if (AXIS_POS_GET(bind->joyaxis) != AXIS_DIR_NONE)
   {
      dir = '+';
      axis = AXIS_POS_GET(bind->joyaxis);
   }

   if (dir)
   {
      char config[16];
      snprintf(config, sizeof(config), "%c%u", dir, axis);
      config_set_string(conf, key, config);
   }
}

static void save_keybind(config_file_t *conf, const char *prefix, const char *base,
      const struct retro_keybind *bind)
{
   if (!bind->valid)
      return;

   save_keybind_key(conf, prefix, base, bind);
   save_keybind_joykey(conf, prefix, base, bind);
   save_keybind_axis(conf, prefix, base, bind);
}

static void save_keybinds_player(config_file_t *conf, unsigned player)
{
   unsigned i = 0;
   for (i = 0; input_config_bind_map[i].valid; i++)
   {
      const char *prefix = input_config_get_prefix(player, input_config_bind_map[i].meta);
      if (prefix)
         save_keybind(conf, prefix, input_config_bind_map[i].base, &g_settings.input.binds[player][i]);
   }
}

bool global_config_save_file(const char *path)
{
   config_file_t *conf = config_file_new(path);
   if (!conf)
      conf = config_file_new(NULL);
   if (!conf)
      return false;

   RARCH_LOG("Saving global config at path: \"%s\"\n", path);

   config_set_path(conf, "libretro_path", g_settings.libretro);
   config_set_path(conf, "libretro_info_path", g_settings.libretro_info_path);
   config_set_bool(conf, "config_save_on_exit", g_extern.config_save_on_exit);
   config_set_int(conf, "libretro_log_level", g_settings.libretro_log_level);
   config_set_bool(conf, "fps_show", g_settings.fps_show);
   config_set_bool(conf, "video_threaded", g_settings.video.threaded);
   config_set_bool(conf, "video_fullscreen", g_settings.video.fullscreen);
   config_set_bool(conf, "video_windowed_fullscreen", g_settings.video.windowed_fullscreen);
   config_set_bool(conf, "video_gpu_screenshot", g_settings.video.gpu_screenshot);
   config_set_int(conf, "game_history_size", g_settings.game_history_size);
   config_set_bool(conf, "menu_all_players_enable", g_settings.input.menu_all_players_enable);
   config_set_float(conf, "video_font_size", g_settings.video.font_size);

   config_set_string(conf, "video_driver", g_settings.video.driver);
   config_set_string(conf, "audio_device", g_settings.audio.device);
   config_set_string(conf, "audio_driver", g_settings.audio.driver);
   config_set_string(conf, "audio_resampler", g_settings.audio.resampler);
   config_set_string(conf, "input_driver", g_settings.input.driver);
   config_set_string(conf, "input_joypad_driver", g_settings.input.joypad_driver);
   config_set_string(conf, "input_keyboard_layout", g_settings.input.keyboard_layout);
   
   config_set_path(conf, "screenshot_directory", *g_settings.screenshot_directory ? g_settings.screenshot_directory : "default");
   config_set_path(conf, "system_directory", *g_settings.system_directory ? g_settings.system_directory : "default");
   config_set_path(conf, "savefile_directory", *g_extern.savefile_dir ? g_extern.savefile_dir : "default");
   config_set_path(conf, "savestate_directory", *g_extern.savestate_dir ? g_extern.savestate_dir : "default");
   config_set_path(conf, "video_shader_dir", *g_settings.video.shader_dir ? g_settings.video.shader_dir : "default");
   config_set_path(conf, "content_directory", *g_settings.content_directory ? g_settings.content_directory : "default");
#ifdef HAVE_MENU
   config_set_path(conf, "rgui_browser_directory", *g_settings.rgui_content_directory ? g_settings.rgui_content_directory : "default");
   config_set_path(conf, "rgui_config_directory", *g_settings.rgui_config_directory ? g_settings.rgui_config_directory : "default");
#endif
   config_set_path(conf, "game_history_path", g_settings.game_history_path);
#ifndef RARCH_CONSOLE
   config_set_path(conf, "joypad_autoconfig_dir", g_settings.input.autoconfig_dir);
#endif
#ifdef HAVE_OVERLAY
   config_set_path(conf, "overlay_directory", *g_extern.overlay_dir ? g_extern.overlay_dir : "default");
#endif

   bool ret = config_file_write(conf, path);
   config_file_free(conf);
   return ret;
}

bool config_save_file(const char *path)
{
   unsigned i = 0;
   config_file_t *conf = config_file_new(path);
   if (!conf)
      conf = config_file_new(NULL);
   if (!conf)
      return false;

   RARCH_LOG("Saving config at path: \"%s\"\n", path);

   config_set_bool(conf, "rewind_enable", g_settings.rewind_enable);
#ifdef HAVE_SCALERS_BUILTIN
   config_set_int(conf,   "filter_index",  g_settings.video.filter_idx);
#endif
   config_set_int(conf, "rewind_granularity", g_settings.rewind_granularity);
   config_set_path(conf, "video_shader", g_settings.video.shader_path);
   config_set_bool(conf, "video_shader_enable", g_settings.video.shader_enable);
   config_set_float(conf, "video_aspect_ratio", g_settings.video.aspect_ratio);
   config_set_float(conf, "video_xscale", g_settings.video.xscale);
   config_set_float(conf, "video_yscale", g_settings.video.yscale);
   config_set_int(conf, "autosave_interval", g_settings.autosave_interval);
   config_set_bool(conf, "video_crop_overscan", g_settings.video.crop_overscan);
   config_set_bool(conf, "video_scale_integer", g_settings.video.scale_integer);
   config_set_bool(conf, "video_force_aspect", g_settings.video.force_aspect);
   config_set_bool(conf, "video_smooth", g_settings.video.smooth);
   config_set_float(conf, "video_refresh_rate", g_settings.video.refresh_rate);
   config_set_bool(conf, "video_vsync", g_settings.video.vsync);
   config_set_int(conf, "video_swap_interval", g_settings.video.swap_interval);
   config_set_int(conf, "video_rotation", g_settings.video.rotation);
   config_set_int(conf, "aspect_ratio_index", g_settings.video.aspect_ratio_idx);
   config_set_bool(conf, "audio_rate_control", g_settings.audio.rate_control);
   config_set_float(conf, "audio_rate_control_delta", g_settings.audio.rate_control_delta);
   config_set_int(conf, "audio_out_rate", g_settings.audio.out_rate);
   g_settings.audio.volume = g_extern.audio_data.volume_db;
   config_set_float(conf, "audio_volume", g_settings.audio.volume);
   config_set_bool(conf, "audio_sync", g_settings.audio.sync);
#ifdef HAVE_OVERLAY
   config_set_path(conf, "input_overlay", g_settings.input.overlay);
   config_set_float(conf, "input_overlay_opacity", g_settings.input.overlay_opacity);
   config_set_float(conf, "input_overlay_scale", g_settings.input.overlay_scale);
#endif
#ifdef ANDROID
   config_set_int(conf, "input_back_behavior", g_settings.input.back_behavior);
   config_set_int(conf, "input_autodetect_icade_profile_pad1", g_settings.input.icade_profile[0]);
   config_set_int(conf, "input_autodetect_icade_profile_pad2", g_settings.input.icade_profile[1]);
   config_set_int(conf, "input_autodetect_icade_profile_pad3", g_settings.input.icade_profile[2]);
   config_set_int(conf, "input_autodetect_icade_profile_pad4", g_settings.input.icade_profile[3]);
#endif
#ifdef RARCH_CONSOLE
   config_set_int(conf, "gamma_correction", g_extern.console_screen.gamma_correction);
   config_set_bool(conf, "soft_filter_enable", g_extern.console_screen.soft_filter_enable);
   config_set_int(conf, "current_resolution_id", g_extern.console_screen.resolution_idx);
#endif
   config_set_int(conf, "custom_viewport_width", g_extern.console_screen.custom_vp.width);
   config_set_int(conf, "custom_viewport_height", g_extern.console_screen.custom_vp.height);
   config_set_int(conf, "custom_viewport_x", g_extern.console_screen.custom_vp.x);
   config_set_int(conf, "custom_viewport_y", g_extern.console_screen.custom_vp.y);
   config_set_bool(conf, "block_sram_overwrite", g_settings.block_sram_overwrite);
   config_set_bool(conf, "savestate_auto_index", g_settings.savestate_auto_index);
   config_set_bool(conf, "savestate_auto_save", g_settings.savestate_auto_save);
   config_set_bool(conf, "savestate_auto_load", g_settings.savestate_auto_load);
   config_set_int(conf, "state_slot", g_extern.state_slot);
   config_set_bool(conf, "input_autodetect_enable", g_settings.input.autodetect_enable);
   
   for (i = 0; i < MAX_PLAYERS; i++)
   {
      char cfg[64];
      snprintf(cfg, sizeof(cfg), "input_libretro_device_p%u", i + 1);
      config_set_int(conf, cfg, g_settings.input.libretro_device[i]);
      snprintf(cfg, sizeof(cfg), "input_player%u_analog_dpad_mode", i + 1);
      config_set_int(conf, cfg, g_settings.input.analog_dpad_mode[i]);
   }

   for (i = 0; i < MAX_PLAYERS; i++)
      save_keybinds_player(conf, i);

   bool ret = config_file_write(conf, path);
   config_file_free(conf);
   return ret;
}
