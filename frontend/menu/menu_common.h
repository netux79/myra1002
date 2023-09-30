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
#include "../../performance.h"
#include "../info/core_info.h"
#include "menu_display.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "file_list.h"
#include "history.h"

typedef enum
{
   RGUI_FILE_PLAIN,
   RGUI_FILE_DIRECTORY,
   RGUI_FILE_DEVICE,
   RGUI_FILE_USE_DIRECTORY,
   RGUI_SETTINGS,

   RGUI_SETTINGS_CONFIG_OPTIONS,
   RGUI_SETTINGS_SAVE_OPTIONS,
   RGUI_SETTINGS_VIDEO_OPTIONS,

   // Menu Stuff
   RGUI_SETTINGS_MENU_OPTIONS,
   RGUI_SETTINGS_MENU_ROTATION,
   RGUI_SETTINGS_MENU_THEME,
   RGUI_SETTINGS_MENU_BILINEAR,
   
   // Video stuff
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
   RGUI_SETTINGS_VIDEO_ROTATION,
   RGUI_SETTINGS_VIDEO_VSYNC,
   RGUI_SETTINGS_VIDEO_CROP_OVERSCAN,
   RGUI_SETTINGS_VIDEO_SOFT_SCALER,
   
   // settings options are done here too
   RGUI_SETTINGS_OPEN_FILEBROWSER,
   RGUI_SETTINGS_OPEN_FILEBROWSER_DEFERRED_CORE,
   RGUI_SETTINGS_OPEN_HISTORY,
   RGUI_SETTINGS_CORE,
   RGUI_SETTINGS_SYSTEM_INFO,
   RGUI_SETTINGS_DEFERRED_CORE,
   RGUI_SETTINGS_CORE_OPTIONS,
   RGUI_SETTINGS_AUDIO_OPTIONS,
   RGUI_SETTINGS_INPUT_OPTIONS,
   RGUI_SETTINGS_PATH_OPTIONS,
   RGUI_SETTINGS_OVERLAY_OPTIONS,
   RGUI_SETTINGS_OPTIONS,
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
   RGUI_SETTINGS_INPUT_TYPE,
   RGUI_SETTINGS_SCREENSHOT,
   RGUI_SETTINGS_SAVESTATE_AUTO_SAVE,
   RGUI_SETTINGS_SAVESTATE_AUTO_LOAD,
   RGUI_SETTINGS_BLOCK_SRAM_OVERWRITE,
   RGUI_SCREENSHOT_DIR_PATH,
   RGUI_GAMES_DIR_PATH,
   RGUI_SAVESTATE_DIR_PATH,
   RGUI_SAVEFILE_DIR_PATH,
   RGUI_LIBRETRO_INFO_DIR_PATH,
   RGUI_CONFIG_DIR_PATH,
   RGUI_OVERLAY_DIR_PATH,
   RGUI_SYSTEM_DIR_PATH,
   RGUI_SETTINGS_AUDIO_MUTE,
   RGUI_SETTINGS_AUDIO_CONTROL_RATE_DELTA,
   RGUI_SETTINGS_AUDIO_VOLUME,
   RGUI_SETTINGS_AUDIO_SYNC,
   
   RGUI_SETTINGS_SHOW_FRAMERATE,
   RGUI_SETTINGS_RESTART_RARCH,
   RGUI_SETTINGS_RESUME_GAME,
   RGUI_SETTINGS_QUIT_RARCH,
   RGUI_SETTINGS_RESTART_GAME,
   RGUI_SETTINGS_SHUTDOWN_SYSTEM,

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
   RGUI_SETTINGS_BIND_MENU_TOGGLE,
   RGUI_SETTINGS_CUSTOM_BIND,
   RGUI_SETTINGS_BIND_DEFAULT_ALL,
   RGUI_SETTINGS_SYSTEM_INFO_NONE = 0xffff,
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

#define RGUI_SCROLL_SIZE 5
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
   char deferred_path[MAX_LEN];

   // Quick jumping indices with L/R.
   // Rebuilt when parsing directory.
   size_t scroll_indices[2 * (26 + 2) + 1];
   unsigned scroll_indices_size;
   unsigned scroll_accel;

   const uint8_t *font;

   struct retro_system_info info;
   bool load_no_rom;

   unsigned c_player;
   unsigned s_device;

   rom_history_t *history;
   retro_time_t last_time; // Used to throttle RGUI in case VSync is broken.

   struct rgui_bind_state binds;
} rgui_handle_t;

extern rgui_handle_t *rgui;

void menu_init(void *data);
bool menu_iterate(void *data);
void menu_free(void *data);

void menu_ticker_line(char *buf, size_t len, unsigned tick, const char *str, bool selected);

void menu_init_core_info(void *data);

void load_menu_game_prepare(void *video_data);
bool load_menu_game(void);
void load_menu_game_history(unsigned game_index);
void menu_rom_history_push(const char *path, const char *core_path,
      const char *core_name);

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
