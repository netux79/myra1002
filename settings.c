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
#include "input/input_common.h"
#include <ctype.h>

struct settings g_settings ATTRIBUTE_ALIGN(32);
struct global g_extern ATTRIBUTE_ALIGN(32);

void config_set_defaults(void)
{
   unsigned i, j;
      
   strlcpy(g_settings.video.driver, DEFAULT_VIDEO_DRIVER, sizeof(g_settings.video.driver));
   strlcpy(g_settings.audio.driver, DEFAULT_AUDIO_DRIVER, sizeof(g_settings.audio.driver));
   strlcpy(g_settings.input.driver, DEFAULT_INPUT_DRIVER, sizeof(g_settings.input.driver));
   strlcpy(g_settings.audio.resampler, DEFAULT_RESAMPLER_DRIVER, sizeof(g_settings.audio.resampler));   

   g_settings.video.vsync = DEFAULT_VIDEO_VSYNC;
   g_settings.video.bilinear_filter = DEFAULT_VIDEO_BILINEAR_FILTER;
   g_settings.video.force_aspect = DEFAULT_VIDEO_FORCE_ASPECT;
   g_settings.video.scale_integer = DEFAULT_VIDEO_SCALE_INTEGER;
   g_settings.video.crop_overscan = DEFAULT_VIDEO_CROP_OVERSCAN;
   g_settings.video.manual_aspect_ratio = DEFAULT_VIDEO_ASPECT_RATIO;
   g_settings.video.aspect_ratio_idx = DEFAULT_VIDEO_ASPECT_RATIO_IDX;
   g_settings.video.refresh_rate = DEFAULT_VIDEO_REFRESH_RATE;
   g_settings.video.gamma_correction = DEFAULT_VIDEO_GAMMA;
   g_settings.video.rotation = DEFAULT_VIDEO_ROTATION;
   g_settings.video.menu_rotation = DEFAULT_VIDEO_ROTATION;
   g_settings.video.vi_trap_filter = DEFAULT_VIDEO_VI_TRAP_FILTER;
   g_settings.video.resolution_idx = DEFAULT_VIDEO_RESOLUTION_IDX;
   g_settings.video.interlaced_resolution_only = DEFAULT_VIDEO_INTERLACED_RESOLUTION_ONLY;
   g_settings.video.pos_x = DEFAULT_VIDEO_POS_X;
   g_settings.video.pos_y = DEFAULT_VIDEO_POS_Y;
   g_settings.video.custom_vp.width = DEFAULT_VIDEO_CUSTOM_VP_WIDTH;
   g_settings.video.custom_vp.height = DEFAULT_VIDEO_CUSTOM_VP_HEIGHT;
   g_settings.video.custom_vp.x = DEFAULT_VIDEO_CUSTOM_VP_X;
   g_settings.video.custom_vp.y = DEFAULT_VIDEO_CUSTOM_VP_Y;
   g_settings.video.filter_idx = DEFAULT_VIDEO_FILTER_IDX;
   g_extern.video.resolution_first_hires = DEFAULT_VIDEO_RESOLUTION_HIRES;

   g_settings.audio.enable = DEFAULT_AUDIO_ENABLE;
   g_settings.audio.out_rate = DEFAULT_AUDIO_OUT_RATE;
   g_settings.audio.latency = DEFAULT_AUDIO_OUT_LATENCY;
   g_settings.audio.sync = DEFAULT_AUDIO_AUDIO_SYNC;
   g_settings.audio.rate_control = DEFAULT_AUDIO_RATE_CONTROL;
   g_settings.audio.rate_control_delta = DEFAULT_AUDIO_RATE_CONTROL_DELTA;
   g_settings.audio.volume = DEFAULT_AUDIO_VOLUME;
   g_settings.audio.mute = DEFAULT_AUDIO_MUTE;
   g_extern.audio_data.volume_db   = DEFAULT_AUDIO_VOLUME;
   g_extern.audio_data.volume_gain = db_to_gain(DEFAULT_AUDIO_VOLUME);
   g_extern.audio_data.in_rate = DEFAULT_AUDIO_OUT_RATE;

   g_settings.rewind_enable = DEFAULT_REWIND_ENABLE;
   g_settings.rewind_buffer_size = DEFAULT_REWIND_BUFFER_SIZE;
   g_settings.rewind_granularity = DEFAULT_REWIND_GRANULARITY;

   g_settings.block_sram_overwrite = DEFAULT_BLOCK_SRAM_OVERWRITE;
   g_settings.savestate_auto_save  = DEFAULT_SAVESTATE_AUTO_SAVE;
   g_settings.savestate_auto_load  = DEFAULT_SAVESTATE_AUTO_LOAD;
   g_settings.state_slot = DEFAULT_SAVESTATE_SLOT;

   g_settings.slowmotion_ratio = DEFAULT_SLOWMOTION_RATIO;
   g_settings.game_history_size = DEFAULT_GAME_HISTORY_SIZE;
   g_settings.config_save_on_exit = DEFAULT_CONFIG_SAVE_ON_EXIT;
   g_settings.fps_show = DEFAULT_VIDEO_SHOW_FRAMERATE;   
   g_extern.lifecycle_state |= (1ULL << MODE_MENU_PREINIT);
   g_extern.config_type = CONFIG_PER_CORE;

   *g_settings.libretro_info_directory = DEFAULT_DIRECTORY_LOCATION;
   *g_settings.screenshot_directory = DEFAULT_DIRECTORY_LOCATION;
   *g_settings.games_directory = DEFAULT_DIRECTORY_LOCATION;
   *g_settings.overlay_directory = DEFAULT_DIRECTORY_LOCATION;
   *g_settings.config_directory = DEFAULT_DIRECTORY_LOCATION;
   strlcpy(g_settings.system_directory, default_paths.system_dir, sizeof(g_settings.system_directory));
   strlcpy(g_settings.savefile_directory, default_paths.sram_dir, sizeof(g_settings.savefile_directory));
   strlcpy(g_settings.savestate_directory, default_paths.savestate_dir, sizeof(g_settings.savestate_directory));

   g_settings.input.axis_threshold = DEFAULT_AXIS_THRESHOLD;
   g_settings.input.turbo_period = DEFAULT_TURBO_PERIOD;
   g_settings.input.turbo_duty_cycle = DEFAULT_TURBO_DUTY_CYCLE;
   g_settings.input.autoconf_buttons = DEFAULT_INPUT_AUTOCONF_BUTTONS;
   g_settings.input.menu_all_players_enable = DEFAULT_MENU_ALL_PLAYERS_ENABLE;
   g_settings.input.quick_swap_players = DEFAULT_QUICK_SWAP_PLAYERS;
   *g_settings.input.overlay_path = DEFAULT_INPUT_OVERLAY_PATH;
   g_settings.input.overlay_opacity = DEFAULT_INPUT_OVERLAY_OPACITY;
   g_settings.input.overlay_scale = DEFAULT_INPUT_OVERLAY_SCALE;

   rarch_assert(sizeof(g_settings.input.binds[0]) >= sizeof(retro_keybinds_1));
   rarch_assert(sizeof(g_settings.input.binds[1]) >= sizeof(retro_keybinds_rest));
   memcpy(g_settings.input.binds[0], retro_keybinds_1, sizeof(retro_keybinds_1));
   memcpy(g_settings.input.menu_binds, retro_keybinds_menu, sizeof(retro_keybinds_menu));
   for (i = 1; i < MAX_PLAYERS; i++)
      memcpy(g_settings.input.binds[i], retro_keybinds_rest, sizeof(retro_keybinds_rest));

   // Verify that binds are in proper order.
   for (i = 0; i < MAX_PLAYERS; i++)
      for (j = 0; j < RARCH_BIND_LIST_END; j++)
         if (g_settings.input.binds[i][j].valid)
            rarch_assert(j == g_settings.input.binds[i][j].id);

   for (i = 0; i < MAX_PLAYERS; i++)
   {
      g_settings.input.device_port[i] = i;
      g_settings.input.analog_dpad_mode[i] = ANALOG_DPAD_NONE;
      if (!g_extern.has_set_libretro_device[i])
         g_settings.input.libretro_device[i] = RETRO_DEVICE_JOYPAD;
   }

   rarch_init_msg_queue();
}

static void calculate_specific_config_path(const char *in_basename)
{
   if (*g_settings.config_directory)
      strlcpy(g_extern.specific_config_path, g_settings.config_directory, sizeof(g_extern.specific_config_path));
   else
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
   else if (g_settings.config_save_on_exit)
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
   
   return conf;
}

static void config_read_keybinds_conf(config_file_t *conf);

bool global_config_load_file(const char *path)
{
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

   CONFIG_GET_STRING(input.driver, "input_driver");
   CONFIG_GET_BOOL(config_save_on_exit, "config_save_on_exit");
   CONFIG_GET_BOOL(input.menu_all_players_enable, "menu_all_players_enable");
   CONFIG_GET_BOOL(fps_show, "fps_show");
   CONFIG_GET_BOOL(audio.mute, "audio_mute");
   CONFIG_GET_INT(game_history_size, "game_history_size");
   CONFIG_GET_INT(video.menu_rotation, "video_menu_rotation");
   CONFIG_GET_PATH(libretro_info_directory, "libretro_info_directory");
   if (!strcmp(g_settings.libretro_info_directory, "default"))
      *g_settings.libretro_info_directory = DEFAULT_DIRECTORY_LOCATION;
   CONFIG_GET_PATH(screenshot_directory, "screenshot_directory");
   if (!strcmp(g_settings.screenshot_directory, "default"))
      *g_settings.screenshot_directory = DEFAULT_DIRECTORY_LOCATION;
   CONFIG_GET_PATH(games_directory, "games_directory");
   if (!strcmp(g_settings.games_directory, "default"))
      *g_settings.games_directory = DEFAULT_DIRECTORY_LOCATION;
#ifdef HAVE_OVERLAY
   CONFIG_GET_PATH(overlay_directory, "overlay_directory");
   if (!strcmp(g_settings.overlay_directory, "default"))
      *g_settings.overlay_directory = DEFAULT_DIRECTORY_LOCATION;
#endif
   CONFIG_GET_PATH(config_directory, "config_directory");
   if (!strcmp(g_settings.config_directory, "default"))
      *g_settings.config_directory = DEFAULT_DIRECTORY_LOCATION;
   CONFIG_GET_PATH(system_directory, "system_directory");
   if (!strcmp(g_settings.system_directory, "default"))
      strlcpy(g_settings.system_directory, default_paths.system_dir, sizeof(g_settings.system_directory));
   CONFIG_GET_PATH(savefile_directory, "savefile_directory");
   if (!strcmp(g_settings.savefile_directory, "default"))
      strlcpy(g_settings.savefile_directory, default_paths.sram_dir, sizeof(g_settings.savefile_directory));
   CONFIG_GET_PATH(savestate_directory, "savestate_directory");
   if (!strcmp(g_settings.savestate_directory, "default"))
      strlcpy(g_settings.savestate_directory, default_paths.savestate_dir, sizeof(g_settings.savestate_directory));

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

   CONFIG_GET_BOOL(video.vsync, "video_vsync");
   CONFIG_GET_BOOL(video.bilinear_filter, "video_bilinear_filter");
   CONFIG_GET_BOOL(video.force_aspect, "video_force_aspect");
   CONFIG_GET_BOOL(video.scale_integer, "video_scale_integer");
   CONFIG_GET_BOOL(video.crop_overscan, "video_crop_overscan");
   CONFIG_GET_FLOAT(video.manual_aspect_ratio, "video_manual_aspect_ratio");
   CONFIG_GET_INT(video.aspect_ratio_idx, "aspect_ratio_index");
   CONFIG_GET_FLOAT(video.refresh_rate, "video_refresh_rate");
   CONFIG_GET_INT(video.rotation, "video_rotation");
#ifdef HAVE_SCALERS_BUILTIN
   CONFIG_GET_INT(video.filter_idx, "filter_index");
#endif
   CONFIG_GET_INT(video.gamma_correction, "gamma_correction");
   CONFIG_GET_BOOL(video.vi_trap_filter, "vi_trap_filter");
   CONFIG_GET_INT(video.resolution_idx, "current_resolution_id");
   CONFIG_GET_BOOL(video.interlaced_resolution_only, "interlaced_resolution_only");
   CONFIG_GET_INT(video.pos_x, "screen_pos_x");
   CONFIG_GET_INT(video.pos_y, "screen_pos_y");
   CONFIG_GET_INT(state_slot, "state_slot");
   CONFIG_GET_INT(video.custom_vp.x, "custom_viewport_x");
   CONFIG_GET_INT(video.custom_vp.y, "custom_viewport_y");
   CONFIG_GET_INT(video.custom_vp.width, "custom_viewport_width");
   CONFIG_GET_INT(video.custom_vp.height, "custom_viewport_height");
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
   CONFIG_GET_INT(audio.latency, "audio_latency");
   CONFIG_GET_BOOL(audio.sync, "audio_sync");
   CONFIG_GET_BOOL(audio.rate_control, "audio_rate_control");
   CONFIG_GET_FLOAT(audio.rate_control_delta, "audio_rate_control_delta");
   CONFIG_GET_FLOAT(audio.volume, "audio_volume");
   g_extern.audio_data.volume_db   = g_settings.audio.volume;
   g_extern.audio_data.volume_gain = db_to_gain(g_settings.audio.volume);
#ifdef HAVE_OVERLAY
   CONFIG_GET_PATH(input.overlay_path, "input_overlay_path");
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
      g_settings.slowmotion_ratio = DEFAULT_SLOWMOTION_RATIO;
   CONFIG_GET_BOOL(block_sram_overwrite, "block_sram_overwrite");
   CONFIG_GET_BOOL(savestate_auto_save, "savestate_auto_save");
   CONFIG_GET_BOOL(savestate_auto_load, "savestate_auto_load");
   CONFIG_GET_INT(input.turbo_period, "input_turbo_period");
   CONFIG_GET_INT(input.turbo_duty_cycle, "input_duty_cycle");
   CONFIG_GET_BOOL(input.autoconf_buttons, "input_autoconf_buttons");
   CONFIG_GET_INT(input.quick_swap_players, "quick_swap_players");

   config_read_keybinds_conf(conf);

   config_file_free(conf);
   return true;
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

static void save_keybind_joykey(config_file_t *conf, const char *prefix, const char *base,
      const struct retro_keybind *bind)
{
   char key[64];
   snprintf(key, sizeof(key), "%s_%s_btn", prefix, base);

   if (bind->joykey == NO_BTN)
      config_set_string(conf, key, "nul");
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
   config_set_string(conf, "input_driver", g_settings.input.driver);
   config_set_bool(conf, "config_save_on_exit", g_settings.config_save_on_exit);
   config_set_bool(conf, "fps_show", g_settings.fps_show);
   config_set_bool(conf, "audio_mute", g_settings.audio.mute);
   config_set_int(conf, "game_history_size", g_settings.game_history_size);
   config_set_int(conf, "video_menu_rotation", g_settings.video.menu_rotation);
   config_set_bool(conf, "menu_all_players_enable", g_settings.input.menu_all_players_enable);
   config_set_path(conf, "libretro_info_directory", *g_settings.libretro_info_directory ? g_settings.libretro_info_directory : "default");
   config_set_path(conf, "screenshot_directory", *g_settings.screenshot_directory ? g_settings.screenshot_directory : "default");
   config_set_path(conf, "games_directory", *g_settings.games_directory ? g_settings.games_directory : "default");
#ifdef HAVE_OVERLAY
   config_set_path(conf, "overlay_directory", *g_settings.overlay_directory ? g_settings.overlay_directory : "default");
#endif
   config_set_path(conf, "config_directory", *g_settings.config_directory ? g_settings.config_directory : "default");
   config_set_path(conf, "system_directory", g_settings.system_directory);
   config_set_path(conf, "savefile_directory", g_settings.savefile_directory);
   config_set_path(conf, "savestate_directory", g_settings.savestate_directory);

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
   config_set_bool(conf, "video_crop_overscan", g_settings.video.crop_overscan);
   config_set_bool(conf, "video_scale_integer", g_settings.video.scale_integer);
   config_set_bool(conf, "video_force_aspect", g_settings.video.force_aspect);
   config_set_bool(conf, "video_bilinear_filter", g_settings.video.bilinear_filter);
   config_set_float(conf, "video_refresh_rate", g_settings.video.refresh_rate);
   config_set_bool(conf, "video_vsync", g_settings.video.vsync);
   config_set_int(conf, "video_rotation", g_settings.video.rotation);
   config_set_int(conf, "aspect_ratio_index", g_settings.video.aspect_ratio_idx);
   config_set_bool(conf, "audio_rate_control", g_settings.audio.rate_control);
   config_set_float(conf, "audio_rate_control_delta", g_settings.audio.rate_control_delta);
   config_set_int(conf, "audio_out_rate", g_settings.audio.out_rate);
   g_settings.audio.volume = g_extern.audio_data.volume_db;
   config_set_float(conf, "audio_volume", g_settings.audio.volume);
   config_set_bool(conf, "audio_sync", g_settings.audio.sync);
#ifdef HAVE_OVERLAY
   config_set_path(conf, "input_overlay_path", g_settings.input.overlay_path);
   config_set_float(conf, "input_overlay_opacity", g_settings.input.overlay_opacity);
   config_set_float(conf, "input_overlay_scale", g_settings.input.overlay_scale);
#endif
   config_set_int(conf, "gamma_correction", g_settings.video.gamma_correction);
   config_set_bool(conf, "vi_trap_filter", g_settings.video.vi_trap_filter);
   config_set_int(conf, "current_resolution_id", g_settings.video.resolution_idx);
   config_set_bool(conf, "interlaced_resolution_only", g_settings.video.interlaced_resolution_only);
   config_set_int(conf, "screen_pos_x", g_settings.video.pos_x);
   config_set_int(conf, "screen_pos_y", g_settings.video.pos_y);
   config_set_int(conf, "custom_viewport_width", g_settings.video.custom_vp.width);
   config_set_int(conf, "custom_viewport_height", g_settings.video.custom_vp.height);
   config_set_int(conf, "custom_viewport_x", g_settings.video.custom_vp.x);
   config_set_int(conf, "custom_viewport_y", g_settings.video.custom_vp.y);
   config_set_bool(conf, "block_sram_overwrite", g_settings.block_sram_overwrite);
   config_set_bool(conf, "savestate_auto_save", g_settings.savestate_auto_save);
   config_set_bool(conf, "savestate_auto_load", g_settings.savestate_auto_load);
   config_set_int(conf, "state_slot", g_settings.state_slot);
   config_set_bool(conf, "input_autoconf_buttons", g_settings.input.autoconf_buttons);
   config_set_int(conf, "quick_swap_players", g_settings.input.quick_swap_players);
   
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
