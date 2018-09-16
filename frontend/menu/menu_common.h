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

#ifndef MENU_COMMON_H__
#define MENU_COMMON_H__

#include "../../general.h"

#ifdef HAVE_CONFIG_H
#include "../../config.h"
#endif

#include "../../performance.h"
#include "../info/core_info.h"
#include "menu_display.h"

#ifdef HAVE_MENU
#define MENU_TEXTURE_FULLSCREEN false
#endif

#include "../../boolean.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "file_list.h"

#if defined(HAVE_CG) || defined(HAVE_HLSL) || defined(HAVE_GLSL)
#define HAVE_SHADER_MANAGER
#include "../../gfx/shader_parse.h"
#endif

#include "history.h"

#define RGUI_MAX_SHADERS 8

typedef enum
{
   RGUI_FILE_PLAIN,
   RGUI_FILE_DIRECTORY,
   RGUI_FILE_DEVICE,
   RGUI_FILE_USE_DIRECTORY,
   RGUI_SETTINGS,
   RGUI_HELP_SCREEN,

   // Video stuff
   RGUI_SETTINGS_CONFIG_OPTIONS,
   RGUI_SETTINGS_SAVE_OPTIONS,
   RGUI_SETTINGS_VIDEO_OPTIONS,
   RGUI_SETTINGS_VIDEO_RESOLUTION,
   RGUI_SETTINGS_VIDEO_BILINEAR,
   RGUI_SETTINGS_VIDEO_VITRAP_FILTER,
   RGUI_SETTINGS_VIDEO_INTERLACED_ONLY,
   RGUI_SETTINGS_VIDEO_SCREEN_POS_X,
   RGUI_SETTINGS_VIDEO_SCREEN_POS_Y,
   RGUI_SETTINGS_VIDEO_GAMMA,
   RGUI_SETTINGS_VIDEO_INTEGER_SCALE,
   RGUI_SETTINGS_VIDEO_FORCE_ASPECT,
   RGUI_SETTINGS_VIDEO_ASPECT_RATIO,
   RGUI_SETTINGS_CUSTOM_VIEWPORT_X,
   RGUI_SETTINGS_CUSTOM_VIEWPORT_Y,
   RGUI_SETTINGS_CUSTOM_VIEWPORT_WIDTH,
   RGUI_SETTINGS_CUSTOM_VIEWPORT_HEIGHT,
   RGUI_SETTINGS_TOGGLE_FULLSCREEN,
   RGUI_SETTINGS_WINDOWED_FULLSCREEN,
   RGUI_SETTINGS_VIDEO_THREADED,
   RGUI_SETTINGS_VIDEO_ROTATION,
   RGUI_SETTINGS_VIDEO_VSYNC,
   RGUI_SETTINGS_VIDEO_SWAP_INTERVAL,
   RGUI_SETTINGS_VIDEO_WINDOW_SCALE_X,
   RGUI_SETTINGS_VIDEO_WINDOW_SCALE_Y,
   RGUI_SETTINGS_VIDEO_CROP_OVERSCAN,
   RGUI_SETTINGS_VIDEO_REFRESH_RATE,
   RGUI_SETTINGS_VIDEO_SOFT_SCALER,
   
   // Shader stuff
   RGUI_SETTINGS_SHADER_OPTIONS,
   RGUI_SETTINGS_SHADER_FILTER,
   RGUI_SETTINGS_SHADER_PRESET,
   RGUI_SETTINGS_SHADER_APPLY,
   RGUI_SETTINGS_SHADER_PASSES,
   RGUI_SETTINGS_SHADER_0,
   RGUI_SETTINGS_SHADER_0_FILTER,
   RGUI_SETTINGS_SHADER_0_SCALE,
   RGUI_SETTINGS_SHADER_LAST = RGUI_SETTINGS_SHADER_0_SCALE + (3 * (RGUI_MAX_SHADERS - 1)),
   RGUI_SETTINGS_SHADER_PRESET_SAVE,

   // settings options are done here too
   RGUI_SETTINGS_OPEN_FILEBROWSER,
   RGUI_SETTINGS_OPEN_FILEBROWSER_DEFERRED_CORE,
   RGUI_SETTINGS_OPEN_HISTORY,
   RGUI_SETTINGS_CORE,
   RGUI_SETTINGS_CORE_INFO,
   RGUI_SETTINGS_DEFERRED_CORE,
   RGUI_SETTINGS_CORE_OPTIONS,
   RGUI_SETTINGS_AUDIO_OPTIONS,
   RGUI_SETTINGS_INPUT_OPTIONS,
   RGUI_SETTINGS_PATH_OPTIONS,
   RGUI_SETTINGS_OVERLAY_OPTIONS,
   RGUI_SETTINGS_OPTIONS,
   RGUI_SETTINGS_DRIVERS,
   RGUI_SETTINGS_REWIND_ENABLE,
   RGUI_SETTINGS_REWIND_GRANULARITY,
   RGUI_SETTINGS_CONFIG_SAVE_ON_EXIT,
   RGUI_SETTINGS_CONFIG_TYPE,
   RGUI_SETTINGS_CONFIG_SAVE_GAME_SPECIFIC,
   RGUI_SETTINGS_SRAM_AUTOSAVE,
   RGUI_SETTINGS_SAVESTATE_SAVE,
   RGUI_SETTINGS_SAVESTATE_LOAD,
   RGUI_SETTINGS_DISK_OPTIONS,
   RGUI_SETTINGS_DISK_INDEX,
   RGUI_SETTINGS_DISK_APPEND,
   RGUI_SETTINGS_DRIVER_VIDEO,
   RGUI_SETTINGS_DRIVER_AUDIO,
   RGUI_SETTINGS_DRIVER_AUDIO_DEVICE,
   RGUI_SETTINGS_DRIVER_AUDIO_RESAMPLER,
   RGUI_SETTINGS_DRIVER_INPUT,
   RGUI_SETTINGS_SCREENSHOT,
   RGUI_SETTINGS_GPU_SCREENSHOT,
   RGUI_SETTINGS_SAVESTATE_AUTO_SAVE,
   RGUI_SETTINGS_SAVESTATE_AUTO_LOAD,
   RGUI_SETTINGS_BLOCK_SRAM_OVERWRITE,
   RGUI_SETTINGS_WINDOW_COMPOSITING_ENABLE,
   RGUI_SETTINGS_PAUSE_IF_WINDOW_FOCUS_LOST,
   RGUI_SCREENSHOT_DIR_PATH,
   RGUI_BROWSER_DIR_PATH,
   RGUI_SHADER_DIR_PATH,
   RGUI_SAVESTATE_DIR_PATH,
   RGUI_SAVEFILE_DIR_PATH,
   RGUI_LIBRETRO_DIR_PATH,
   RGUI_LIBRETRO_INFO_DIR_PATH,
   RGUI_CONFIG_DIR_PATH,
   RGUI_OVERLAY_DIR_PATH,
   RGUI_SYSTEM_DIR_PATH,
   RGUI_SETTINGS_AUDIO_MUTE,
   RGUI_SETTINGS_AUDIO_CONTROL_RATE_DELTA,
   RGUI_SETTINGS_AUDIO_VOLUME,
   RGUI_SETTINGS_AUDIO_SYNC,
   
   RGUI_SETTINGS_DEBUG_TEXT,
   RGUI_SETTINGS_RESTART_EMULATOR,
   RGUI_SETTINGS_RESUME_GAME,
   RGUI_SETTINGS_QUIT_RARCH,
   RGUI_SETTINGS_RESTART_GAME,

   RGUI_SETTINGS_OVERLAY_PRESET,
   RGUI_SETTINGS_OVERLAY_OPACITY,
   RGUI_SETTINGS_OVERLAY_SCALE,
   RGUI_SETTINGS_BIND_PLAYER,
   RGUI_SETTINGS_BIND_PLAYER_KEYS,
   RGUI_SETTINGS_BIND_HOTKEYS,
   RGUI_SETTINGS_BIND_DEVICE,
   RGUI_SETTINGS_BIND_DEVICE_TYPE,
   RGUI_SETTINGS_BIND_ANALOG_MODE,
   RGUI_SETTINGS_DEVICE_AUTOCONF_BUTTONS,
   RGUI_SETTINGS_MENU_ALL_PLAYERS_ENABLE,
   RGUI_SETTINGS_QUICK_SWAP_PLAYERS,

   // Match up with libretro order for simplicity.
   RGUI_SETTINGS_BIND_BEGIN,
   RGUI_SETTINGS_BIND_B = RGUI_SETTINGS_BIND_BEGIN,
   RGUI_SETTINGS_BIND_Y,
   RGUI_SETTINGS_BIND_SELECT,
   RGUI_SETTINGS_BIND_START,
   RGUI_SETTINGS_BIND_UP,
   RGUI_SETTINGS_BIND_DOWN,
   RGUI_SETTINGS_BIND_LEFT,
   RGUI_SETTINGS_BIND_RIGHT,
   RGUI_SETTINGS_BIND_A,
   RGUI_SETTINGS_BIND_X,
   RGUI_SETTINGS_BIND_L,
   RGUI_SETTINGS_BIND_R,
   RGUI_SETTINGS_BIND_L2,
   RGUI_SETTINGS_BIND_R2,
   RGUI_SETTINGS_BIND_L3,
   RGUI_SETTINGS_BIND_R3,
   RGUI_SETTINGS_BIND_ANALOG_LEFT_X_PLUS,
   RGUI_SETTINGS_BIND_ANALOG_LEFT_X_MINUS,
   RGUI_SETTINGS_BIND_ANALOG_LEFT_Y_PLUS,
   RGUI_SETTINGS_BIND_ANALOG_LEFT_Y_MINUS,
   RGUI_SETTINGS_BIND_ANALOG_RIGHT_X_PLUS,
   RGUI_SETTINGS_BIND_ANALOG_RIGHT_X_MINUS,
   RGUI_SETTINGS_BIND_ANALOG_RIGHT_Y_PLUS,
   RGUI_SETTINGS_BIND_ANALOG_RIGHT_Y_MINUS,
   RGUI_SETTINGS_BIND_LAST = RGUI_SETTINGS_BIND_ANALOG_RIGHT_Y_MINUS,
   RGUI_SETTINGS_BIND_TURBO_ENABLE,
   
   RGUI_SETTINGS_BIND_FAST_FORWARD_KEY,
   RGUI_SETTINGS_BIND_FAST_FORWARD_HOLD_KEY,
   RGUI_SETTINGS_BIND_LOAD_STATE_KEY,
   RGUI_SETTINGS_BIND_SAVE_STATE_KEY,
   RGUI_SETTINGS_BIND_QUIT_KEY,
   RGUI_SETTINGS_BIND_STATE_SLOT_PLUS,
   RGUI_SETTINGS_BIND_STATE_SLOT_MINUS,
   RGUI_SETTINGS_BIND_REWIND,
   RGUI_SETTINGS_BIND_PAUSE_TOGGLE,
   RGUI_SETTINGS_BIND_FRAMEADVANCE,
   RGUI_SETTINGS_BIND_RESET,
   RGUI_SETTINGS_BIND_SCREENSHOT,
   RGUI_SETTINGS_BIND_MUTE,
   RGUI_SETTINGS_BIND_SLOWMOTION,
   RGUI_SETTINGS_BIND_ENABLE_HOTKEY,
   RGUI_SETTINGS_BIND_VOLUME_UP,
   RGUI_SETTINGS_BIND_VOLUME_DOWN,
   RGUI_SETTINGS_BIND_DISK_EJECT_TOGGLE,
   RGUI_SETTINGS_BIND_DISK_NEXT,
   RGUI_SETTINGS_BIND_QUICK_SWAP,
#ifndef RARCH_CONSOLE
   RGUI_SETTINGS_BIND_FULLSCREEN_TOGGLE_KEY,
   RGUI_SETTINGS_BIND_SHADER_NEXT,
   RGUI_SETTINGS_BIND_SHADER_PREV,
   RGUI_SETTINGS_BIND_OVERLAY_NEXT,
   RGUI_SETTINGS_BIND_GRAB_MOUSE_TOGGLE,
#endif
#ifdef HAVE_MENU
   RGUI_SETTINGS_BIND_MENU_TOGGLE,
#endif
   RGUI_SETTINGS_CUSTOM_BIND,
   RGUI_SETTINGS_BIND_DEFAULT_ALL,
   RGUI_SETTINGS_CORE_INFO_NONE = 0xffff,
   RGUI_SETTINGS_CORE_OPTION_NONE = 0xffff,
   RGUI_SETTINGS_CORE_OPTION_START = 0x10000
} rgui_file_type_t;

typedef enum
{
   RGUI_ACTION_UP,
   RGUI_ACTION_DOWN,
   RGUI_ACTION_LEFT,
   RGUI_ACTION_RIGHT,
   RGUI_ACTION_OK,
   RGUI_ACTION_CANCEL,
   RGUI_ACTION_REFRESH,
   RGUI_ACTION_SELECT,
   RGUI_ACTION_START,
   RGUI_ACTION_MESSAGE,
   RGUI_ACTION_SCROLL_DOWN,
   RGUI_ACTION_SCROLL_UP,
   RGUI_ACTION_NOOP
} rgui_action_t;

#define RGUI_MAX_BUTTONS 32
#define RGUI_MAX_AXES 32
#define RGUI_MAX_HATS 4
struct rgui_bind_state_port
{
   bool buttons[RGUI_MAX_BUTTONS];
   int16_t axes[RGUI_MAX_AXES];
   uint16_t hats[RGUI_MAX_HATS];
};

struct rgui_bind_axis_state
{
   // Default axis state.
   int16_t rested_axes[RGUI_MAX_AXES];
   // Locked axis state. If we configured an axis, avoid having the same axis state trigger something again right away.
   int16_t locked_axes[RGUI_MAX_AXES];
};

struct rgui_bind_state
{
   struct retro_keybind *target;
   unsigned begin;
   unsigned last;
   unsigned port;
   struct rgui_bind_state_port state[MAX_PLAYERS];
   struct rgui_bind_axis_state axis_state[MAX_PLAYERS];
   bool abort;
};

void menu_poll_bind_get_rested_axes(struct rgui_bind_state *state);
void menu_poll_bind_state(struct rgui_bind_state *state);
bool menu_poll_find_trigger(struct rgui_bind_state *state, struct rgui_bind_state *new_state);

typedef struct
{
   uint64_t old_input_state;
   uint64_t trigger_state;
   bool do_held;

   unsigned delay_timer;
   unsigned delay_count;

   unsigned width;
   unsigned height;

   uint16_t *frame_buf;
   size_t frame_buf_pitch;
   bool frame_buf_show;

   file_list_t *menu_stack;
   file_list_t *selection_buf;
   size_t selection_ptr;
   bool need_refresh;
   bool msg_force;

   core_info_list_t *core_info;
   core_info_t core_info_current;
   bool defer_core;
   char deferred_path[PATH_MAX];

   // Quick jumping indices with L/R.
   // Rebuilt when parsing directory.
   size_t scroll_indices[2 * (26 + 2) + 1];
   unsigned scroll_indices_size;
   unsigned scroll_accel;

   char default_glslp[PATH_MAX];
   char default_cgp[PATH_MAX];

   const uint8_t *font;
   bool alloc_font;

   char libretro_dir[PATH_MAX];
   struct retro_system_info info;
   bool load_no_rom;

#ifdef HAVE_SHADER_MANAGER
   struct gfx_shader shader;
#endif
   unsigned c_player;
   unsigned s_device;

   rom_history_t *history;
   retro_time_t last_time; // Used to throttle RGUI in case VSync is broken.

   struct rgui_bind_state binds;
   struct
   {
      const char **buffer;
      const char *label;
      bool display;
   } keyboard;
} rgui_handle_t;

extern rgui_handle_t *rgui;

void menu_init(void *data);
bool menu_iterate(void *data);
void menu_free(void *data);

#ifdef HAVE_SHADER_MANAGER
void shader_manager_init(void *data);
void shader_manager_get_str(struct gfx_shader *shader,
      char *type_str, size_t type_str_size, unsigned type);
void shader_manager_set_preset(struct gfx_shader *shader,
      enum rarch_shader_type type, const char *path);
void shader_manager_save_preset(void *data, const char *basename, bool apply);
#endif

void menu_ticker_line(char *buf, size_t len, unsigned tick, const char *str, bool selected);

void menu_init_core_info(void *data);

void load_menu_game_prepare(void *video_data);
void load_menu_game_prepare_dummy(void);
bool load_menu_game(void);
void load_menu_game_history(unsigned game_index);
void menu_rom_history_push(const char *path, const char *core_path,
      const char *core_name);
void menu_rom_history_push_current(void);

int menu_settings_toggle_setting(void *data, void *video_data, unsigned setting, unsigned action, unsigned menu_type);
int menu_set_settings(void *data, void *video_data, unsigned setting, unsigned action);
void menu_set_settings_label(char *type_str, size_t type_str_size, unsigned *w, unsigned type);

void menu_populate_entries(void *data, unsigned menu_type);
unsigned menu_type_is(unsigned type);

uint64_t menu_input(void);

extern const menu_driver_t *menugui_driver;

#ifdef __cplusplus
}
#endif

#endif