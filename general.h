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

#include <stdbool.h>
#include <stdio.h>
#include <limits.h>
#include <setjmp.h>
#include "driver.h"
#include "message_queue.h"
#include "rewind.h"
#include "dynamic.h"
#include "compat/strl.h"
#include "performance.h"
#include "core_options.h"
#include "miscellaneous.h"

#ifdef HAVE_SCALERS_BUILTIN
#include "gfx/filter.h"
#endif

#define PACKAGE_NAME "MyRA1002"
#define PACKAGE_VERSION "2.0"

// Wii - for usleep (among others)
#include <unistd.h>
#include "audio/resampler.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PLAYERS 4

enum menu_enums
{
   MODE_GAME = 0,
   MODE_GAME_RUN,
   MODE_LOAD_GAME,
   MODE_MENU,
   MODE_MENU_PREINIT,
   MODE_EXITSPAWN,
   MODE_EXITSPAWN_START_GAME,
};

enum config_type_enums
{
   CONFIG_PER_CORE = 0,
   CONFIG_PER_GAME,
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

// All config related settings go here.
struct settings
{
   struct
   {
      float manual_aspect_ratio;
      float refresh_rate;
      unsigned aspect_ratio_idx;
      unsigned rotation;
      unsigned menu_rotation;
      unsigned resolution_idx;
      unsigned gamma_correction;
#ifdef HAVE_SCALERS_BUILTIN
      unsigned filter_idx;
#endif
      int pos_x;
      int pos_y;
      bool vsync;
      bool bilinear_filter;
      bool force_aspect;
      bool crop_overscan;
      bool scale_integer;
      bool vi_trap_filter;
      bool interlaced_resolution_only;      
      rarch_viewport_t custom_vp;
      char driver[32];
   } video;

   struct
   {
      float rate_control_delta;
      float volume; // dB scale
      unsigned out_rate;
      unsigned latency;
      bool enable;
      bool mute;
      bool sync;
      bool rate_control;
      char resampler[32];
      char driver[32];
   } audio;

   struct
   {
      char driver[32];
      struct retro_keybind binds[MAX_PLAYERS][RARCH_BIND_LIST_END];
      struct retro_keybind menu_binds[RARCH_BIND_LIST_END];

      unsigned libretro_device[MAX_PLAYERS];
      unsigned analog_dpad_mode[MAX_PLAYERS];
      float axis_threshold;
      int device_port[MAX_PLAYERS];
      unsigned device[MAX_PLAYERS];
      char device_names[MAX_PLAYERS][64];
      bool autoconf_buttons;
      bool menu_all_players_enable;
      unsigned quick_swap_players;
      
      unsigned turbo_period;
      unsigned turbo_duty_cycle;

      char overlay_path[PATH_MAX];
      float overlay_opacity;
      float overlay_scale;
   } input;

   unsigned game_history_size;

   char libretro[PATH_MAX];
   char libretro_info_directory[PATH_MAX];
   char screenshot_directory[PATH_MAX];
   char system_directory[PATH_MAX];
   char games_directory[PATH_MAX];
   char config_directory[PATH_MAX];
   char savefile_directory[PATH_MAX];
   char savestate_directory[PATH_MAX];
#ifdef HAVE_OVERLAY
   char overlay_directory[PATH_MAX];
#endif

   float slowmotion_ratio;
   int state_slot;
   size_t rewind_buffer_size;
   unsigned rewind_granularity;

   bool rewind_enable;
   bool block_sram_overwrite;
   bool savestate_auto_save;
   bool savestate_auto_load;
   
   bool fps_show;
   bool config_save_on_exit;
};

typedef struct
{
   const void *data;
   unsigned width;
   unsigned height;
   size_t pitch;
} frame_t;

// All run-time- / command line flag-related globals go here.
struct global
{
   bool audio_active;
   bool video_active;

   bool rom_file_temporary;
   char last_rom[PATH_MAX];

   bool has_set_libretro_device[MAX_PLAYERS];
   enum config_type_enums config_type;

   // Config associated with global and specific config.
   char config_path[PATH_MAX];
   char specific_config_path[PATH_MAX];   
   char basename[PATH_MAX];
   char fullpath[PATH_MAX];
   char savefile_name_srm[PATH_MAX];
   char savefile_name_rtc[PATH_MAX];
   char savestate_name[PATH_MAX];

   struct
   {
      enum retro_pixel_format pix_fmt;

      bool core_shutdown;
      bool block_extract;
      bool force_nonblock;
      bool no_game;

      unsigned num_ports;
      
      const char *input_desc_btn[MAX_PLAYERS][RARCH_FIRST_CUSTOM_BIND];
      char valid_extensions[PATH_MAX];
      
      struct retro_system_info info;
      struct retro_system_av_info av_info;
      struct retro_controller_info *ports;
      struct retro_disk_control_callback disk_control;
      struct retro_frame_time_callback frame_time;
      
      retro_usec_t frame_time_last;
      core_option_manager_t *core_options;
   } system;

   struct
   {
      bool using_component;
      unsigned resolution_first_hires;      
   } video;
   
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

      float in_rate;
      float *outsamples;
      int16_t *conv_outsamples;

      int16_t *rewind_buf;
      size_t rewind_ptr;
      size_t rewind_size;

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

   frame_t frame_cache;
   frame_t frame;
   
   unsigned frame_count;
   uint32_t start_frame_time;
   uint64_t lifecycle_state;

#ifdef HAVE_FILE_LOGGER
   // If this is non-NULL. RARCH_LOG and friends will write to this file.
   FILE *log_file;
#endif

   bool main_is_init;
   char error_string[1024];
   jmp_buf error_sjlj_context;
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
void rarch_check_block_hotkey(void);
void rarch_init_rewind(void);
void rarch_deinit_rewind(void);
void rarch_reset_drivers(void);
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
   strlcpy(g_extern.error_string, error, sizeof(g_extern.error_string));
   longjmp(g_extern.error_sjlj_context, error_code);
}

#endif
