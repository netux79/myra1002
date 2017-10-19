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

#ifndef __RARCH_GENERAL_H
#define __RARCH_GENERAL_H

#include "boolean.h"
#include <stdio.h>
#include <limits.h>
#include <setjmp.h>
#include "driver.h"
#include "message_queue.h"
#include "rewind.h"
#include "autosave.h"
#include "dynamic.h"
#include "audio/ext/rarch_dsp.h"
#include "compat/strl.h"
#include "performance.h"
#include "core_options.h"
#include "miscellaneous.h"

#ifdef HAVE_SCALERS_BUILTIN
#include "gfx/filter.h"
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "MyRA1002"
#endif

// Platform-specific headers
// Windows
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

// Wii - for usleep (among others)
#ifdef GEKKO
#include <unistd.h>
#endif

#ifdef HAVE_COMMAND
#include "command.h"
#endif

#include "audio/resampler.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PLAYERS 4

enum menu_enums
{
   MODE_GAME = 0,
#ifdef GEKKO
   MODE_GAME_RUN,
#endif
   MODE_LOAD_GAME,
   MODE_MENU,
#if !(defined(RARCH_CONSOLE) || defined(ANDROID))   
   MODE_EXIT,
#endif
   MODE_MENU_PREINIT,
   MODE_EXITSPAWN,
   MODE_EXITSPAWN_START_GAME,
};

enum sound_mode_enums
{
   SOUND_MODE_NORMAL = 0,
#ifdef HAVE_HEADSET
   SOUND_MODE_HEADSET,
#endif
   SOUND_MODE_LAST
};

enum config_type_enums
{
   CONFIG_PER_CORE = 0,
   CONFIG_PER_GAME,
};


// All config related settings go here.
struct settings
{
   struct
   {
      char driver[32];
      char gl_context[32];
      float xscale;
      float yscale;
      bool fullscreen;
      bool windowed_fullscreen;
      unsigned monitor_index;
      unsigned fullscreen_x;
      unsigned fullscreen_y;
      bool vsync;
      unsigned swap_interval;
      bool smooth;
      bool force_aspect;
      bool crop_overscan;
      float aspect_ratio;
      bool aspect_ratio_auto;
      bool scale_integer;
      unsigned aspect_ratio_idx;
      unsigned rotation;

      char shader_path[PATH_MAX];
      bool shader_enable;

#ifdef HAVE_SCALERS_BUILTIN
      unsigned filter_idx;
#endif
      float refresh_rate;
      bool threaded;

      char shader_dir[PATH_MAX];
      char font_path[PATH_MAX];
      float font_size;
      bool font_enable;
      bool font_scale;
      float msg_pos_x;
      float msg_pos_y;
      float msg_color_r;
      float msg_color_g;
      float msg_color_b;

      bool disable_composition;
      bool gpu_screenshot;
      bool allow_rotate;
   } video;

   struct
   {
      char driver[32];
      bool enable;
      unsigned out_rate;
      unsigned block_frames;
      float in_rate;
      char device[PATH_MAX];
      unsigned latency;
      bool sync;

      char dsp_plugin[PATH_MAX];

      bool rate_control;
      float rate_control_delta;
      float volume; // dB scale
      char resampler[32];
   } audio;

   struct
   {
      char driver[32];
      char joypad_driver[32];
      char keyboard_layout[64];
      struct retro_keybind binds[MAX_PLAYERS][RARCH_BIND_LIST_END];
#ifdef RARCH_CONSOLE
      struct retro_keybind menu_binds[RARCH_BIND_LIST_END];
#endif

      // Set by autoconfiguration in joypad_autoconfig_dir. Does not override main binds.
      struct retro_keybind autoconf_binds[MAX_PLAYERS][RARCH_BIND_LIST_END];
      bool autoconfigured[MAX_PLAYERS];

      unsigned libretro_device[MAX_PLAYERS];
      unsigned analog_dpad_mode[MAX_PLAYERS];

      float axis_threshold;
      int device_port[MAX_PLAYERS];
      unsigned device[MAX_PLAYERS];
      char device_names[MAX_PLAYERS][64];
      bool debug_enable;
      bool autodetect_enable;
      bool menu_all_players_enable;
#ifdef ANDROID
      unsigned back_behavior;
      unsigned icade_profile[MAX_PLAYERS];
      unsigned icade_count;
#endif

      unsigned turbo_period;
      unsigned turbo_duty_cycle;

      char overlay[PATH_MAX];
      float overlay_opacity;
      float overlay_scale;
#ifndef RARCH_CONSOLE
      char autoconfig_dir[PATH_MAX];
#endif
   } input;

   char core_options_path[PATH_MAX];
   char game_history_path[PATH_MAX];
   unsigned game_history_size;

   char libretro[PATH_MAX];
   unsigned libretro_log_level;
   char libretro_info_path[PATH_MAX];

   char screenshot_directory[PATH_MAX];
   char system_directory[PATH_MAX];

   bool rewind_enable;
   size_t rewind_buffer_size;
   unsigned rewind_granularity;

   float slowmotion_ratio;
   float fastforward_ratio;

   bool pause_nonactive;
   unsigned autosave_interval;

   bool block_sram_overwrite;
   bool savestate_auto_index;
   bool savestate_auto_save;
   bool savestate_auto_load;

   bool stdin_cmd_enable;

   char content_directory[PATH_MAX];
#if defined(HAVE_MENU)
   char rgui_content_directory[PATH_MAX];
   char rgui_config_directory[PATH_MAX];
#endif
   bool fps_show;
};

enum rarch_game_type
{
   RARCH_CART_NORMAL = 0,
   RARCH_CART_SGB,
   RARCH_CART_BSX,
   RARCH_CART_BSX_SLOTTED,
   RARCH_CART_SUFAMI
};

typedef struct rarch_viewport
{
   int x;
   int y;
   unsigned width;
   unsigned height;
   unsigned full_width;
   unsigned full_height;
} rarch_viewport_t;

// All run-time- / command line flag-related globals go here.
struct global
{
   bool verbose;
   bool audio_active;
   bool video_active;
   bool force_fullscreen;

   bool rom_file_temporary;
   char last_rom[PATH_MAX];
   enum rarch_game_type game_type;

   char gb_rom_path[PATH_MAX];
   char bsx_rom_path[PATH_MAX];
   char sufami_rom_path[2][PATH_MAX];
   bool has_set_save_path;
   bool has_set_state_path;
   bool has_set_libretro_device[MAX_PLAYERS];
   enum config_type_enums config_type;

   // Config associated with global and specific config.
   char config_path[PATH_MAX];
   char specific_config_path[PATH_MAX];   
   char input_config_path[PATH_MAX];
   char basename[PATH_MAX];
   char fullpath[PATH_MAX];
   char savefile_name_srm[PATH_MAX];
   char savefile_name_rtc[PATH_MAX]; // Make sure that fill_pathname has space.
   char savefile_name_psrm[PATH_MAX];
   char savefile_name_asrm[PATH_MAX];
   char savefile_name_bsrm[PATH_MAX];
   char savestate_name[PATH_MAX];

   // Used on reentrancy to use a savestate dir.
   char savefile_dir[PATH_MAX];
   char savestate_dir[PATH_MAX];

#ifdef HAVE_OVERLAY
   char overlay_dir[PATH_MAX];
#endif

   int state_slot;

   struct
   {
      retro_time_t minimum_frame_time;
      retro_time_t last_frame_time;
   } frame_limit;

   struct
   {
      struct retro_system_info info;
      struct retro_system_av_info av_info;
      float aspect_ratio;

      unsigned rotation;
      bool core_shutdown;
      enum retro_pixel_format pix_fmt;

      bool block_extract;
      bool force_nonblock;
      bool no_game;

      const char *input_desc_btn[MAX_PLAYERS][RARCH_FIRST_CUSTOM_BIND];
      char valid_extensions[PATH_MAX];

      retro_keyboard_event_t key_event;

      struct retro_audio_callback audio_callback;

      struct retro_disk_control_callback disk_control;
      struct retro_hw_render_callback hw_render_callback;

      struct retro_frame_time_callback frame_time;
      retro_usec_t frame_time_last;

      core_option_manager_t *core_options;

      struct retro_controller_info *ports;
      unsigned num_ports;
   } system;

   struct
   {
      void *resampler_data;
      const rarch_resampler_t *resampler;

      float *data;

      size_t data_ptr;
      size_t chunk_size;
      size_t nonblock_chunk_size;
      size_t block_chunk_size;

      double src_ratio;

      bool use_float;
      bool mute;

      float *outsamples;
      int16_t *conv_outsamples;

      int16_t *rewind_buf;
      size_t rewind_ptr;
      size_t rewind_size;

      dylib_t dsp_lib;
      const rarch_dsp_plugin_t *dsp_plugin;
      void *dsp_handle;

      bool rate_control;
      double orig_src_ratio;
      size_t driver_buffer_size;

      float volume_db;
      float volume_gain;
   } audio_data;

#ifdef HAVE_SCALERS_BUILTIN
   struct
   {
      rarch_softfilter_t *filter;

      void *buffer;
      unsigned scale;
      unsigned out_bpp;
      bool out_rgb32;
   } filter;
#endif

   msg_queue_t *msg_queue;

   bool exec;

   // Rewind support.
   state_manager_t *state_manager;
   size_t state_size;
   bool frame_is_reverse;

   bool sram_load_disable;
   bool sram_save_disable;
   bool use_sram;

   // Pausing support
   bool is_paused;
   bool is_oneshot;
   bool is_slowmotion;

   // Turbo support
   bool turbo_frame_enable[MAX_PLAYERS];
   uint16_t turbo_enable[MAX_PLAYERS];
   unsigned turbo_count;

   // Autosave support.
   autosave_t *autosave[2];

   struct
   {
      const void *data;
      unsigned width;
      unsigned height;
      size_t pitch;
   } frame_cache;

   unsigned frame_count;
   uint32_t start_frame_time;
   char title_buf[64];

   struct
   {
      struct string_list *list;
      size_t ptr;
   } shader_dir;

   // Settings and/or global state that is specific to a console-style implementation.
   struct
   {
      unsigned resolution_idx;
      rarch_viewport_t custom_vp;
      unsigned gamma_correction;
      bool soft_filter_enable;
   } console_screen;

   uint64_t lifecycle_state;

#ifdef HAVE_FILE_LOGGER
   // If this is non-NULL. RARCH_LOG and friends will write to this file.
   FILE *log_file;
#endif

   bool main_is_init;
   bool error_in_init;
   bool config_save_on_exit;
   char error_string[1024];
   jmp_buf error_sjlj_context;
   unsigned menu_toggle_behavior;

   bool libretro_no_rom;
   bool libretro_dummy;
};

struct rarch_main_wrap
{
   const char *rom_path;
   const char *sram_path;
   const char *state_path;
   const char *config_path;
   const char *libretro_path;
   bool verbose;
   bool no_rom;
};

// Public functions
void config_load(void);
void config_set_defaults(void);
const char *config_get_default_video(void);
const char *config_get_default_audio(void);
const char *config_get_default_input(void);

#include "conf/config_file.h"
bool global_config_load_file(const char *path);
bool global_config_save_file(const char *path);
bool config_load_file(const char *path);
bool config_save_file(const char *path);

void rarch_game_reset(void);
void rarch_main_clear_state(void);
void rarch_init_system_info(void);
int rarch_main(int argc, char *argv[]);
int rarch_main_init_wrap(const struct rarch_main_wrap *args);
int rarch_main_init(int argc, char *argv[]);
bool rarch_main_idle_iterate(void);
bool rarch_main_iterate(void);
void rarch_main_deinit(void);
void rarch_render_cached_frame(void);
void rarch_init_msg_queue(void);
void rarch_deinit_msg_queue(void);
void rarch_input_poll(void);
#if defined (HAVE_OVERLAY) && !defined (RARCH_CONSOLE)
void rarch_check_overlay(void);
#endif
void rarch_check_block_hotkey(void);
void rarch_init_rewind(void);
void rarch_deinit_rewind(void);
void rarch_reset_drivers(void);
#ifndef RARCH_CONSOLE
bool rarch_check_fullscreen(void);
#endif
void rarch_disk_control_set_eject(bool state, bool log);
void rarch_disk_control_set_index(unsigned index);
void rarch_disk_control_append_image(const char *path);
bool rarch_set_rumble_state(unsigned port, enum retro_rumble_effect effect, bool enable);
void rarch_init_autosave(void);
void rarch_deinit_autosave(void);
void rarch_take_screenshot(void);
void rarch_load_state(void);
void rarch_save_state(void);
void rarch_state_slot_increase(void);
void rarch_state_slot_decrease(void);

/////////

// Public data structures
extern struct settings g_settings;
extern struct global g_extern;
/////////

#ifdef __cplusplus
}
#endif

static inline float db_to_gain(float db)
{
   return powf(10.0f, db / 20.0f);
}

static inline void rarch_fail(int error_code, const char *error)
{
   // We cannot longjmp unless we're in rarch_main_init().
   // If not, something went very wrong, and we should just exit right away.
   rarch_assert(g_extern.error_in_init);

   strlcpy(g_extern.error_string, error, sizeof(g_extern.error_string));
   longjmp(g_extern.error_sjlj_context, error_code);
}

#endif
