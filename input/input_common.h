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

#ifndef INPUT_COMMON_H__
#define INPUT_COMMON_H__

#include "../driver.h"
#include "../conf/config_file.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void input_conv_analog_id_to_bind_id(unsigned index, unsigned id,
      unsigned *id_minus, unsigned *id_plus)
{
   switch ((index << 1) | id)
   {
      case (RETRO_DEVICE_INDEX_ANALOG_LEFT << 1) | RETRO_DEVICE_ID_ANALOG_X:
         *id_minus = RARCH_ANALOG_LEFT_X_MINUS;
         *id_plus  = RARCH_ANALOG_LEFT_X_PLUS;
         break;

      case (RETRO_DEVICE_INDEX_ANALOG_LEFT << 1) | RETRO_DEVICE_ID_ANALOG_Y:
         *id_minus = RARCH_ANALOG_LEFT_Y_MINUS;
         *id_plus  = RARCH_ANALOG_LEFT_Y_PLUS;
         break;

      case (RETRO_DEVICE_INDEX_ANALOG_RIGHT << 1) | RETRO_DEVICE_ID_ANALOG_X:
         *id_minus = RARCH_ANALOG_RIGHT_X_MINUS;
         *id_plus  = RARCH_ANALOG_RIGHT_X_PLUS;
         break;

      case (RETRO_DEVICE_INDEX_ANALOG_RIGHT << 1) | RETRO_DEVICE_ID_ANALOG_Y:
         *id_minus = RARCH_ANALOG_RIGHT_Y_MINUS;
         *id_plus  = RARCH_ANALOG_RIGHT_Y_PLUS;
         break;
   }
}

struct rarch_joypad_driver
{
   bool (*init)(void);
   bool (*query_pad)(unsigned);
   void (*destroy)(void);
   bool (*button)(unsigned, uint16_t);
   int16_t (*axis)(unsigned, uint32_t);
   void (*poll)(void);
   bool (*set_rumble)(unsigned, enum retro_rumble_effect, uint16_t); // Optional
   const char *(*name)(unsigned);

   const char *ident;
};

// If ident points to NULL or a zero-length string, equivalent to calling input_joypad_init_first().
const rarch_joypad_driver_t *input_joypad_init_driver(const char *ident);
const rarch_joypad_driver_t *input_joypad_init_first(void);

bool input_joypad_pressed(const rarch_joypad_driver_t *driver,
      unsigned port, const struct retro_keybind *binds, unsigned key);

int16_t input_joypad_analog(const rarch_joypad_driver_t *driver,
      unsigned port, unsigned index, unsigned id, const struct retro_keybind *binds);

bool input_joypad_set_rumble(const rarch_joypad_driver_t *driver,
      unsigned port, enum retro_rumble_effect effect, uint16_t strength);

int16_t input_joypad_axis_raw(const rarch_joypad_driver_t *driver,
      unsigned joypad, unsigned axis);
bool input_joypad_button_raw(const rarch_joypad_driver_t *driver,
      unsigned joypad, unsigned button);
bool input_joypad_hat_raw(const rarch_joypad_driver_t *driver,
      unsigned joypad, unsigned hat_dir, unsigned hat);

void input_joypad_poll(const rarch_joypad_driver_t *driver);
const char *input_joypad_name(const rarch_joypad_driver_t *driver, unsigned joypad);
void input_joypad_hotplug(unsigned port, unsigned device_type, const char *padname, bool activated);

// Input config.
struct input_bind_map
{
   bool valid;

   // Meta binds get input as prefix, not input_playerN".
   // 0 = libretro related.
   // 1 = Common hotkey.
   // 2 = Uncommon/obscure hotkey.
   unsigned meta;

   const char *base;
   const char *desc;
   unsigned retro_key;
};

extern const struct input_bind_map input_config_bind_map[];

void input_get_bind_string(char *buf, const struct retro_keybind *bind, unsigned port, size_t size);

const char *input_config_get_prefix(unsigned player, bool meta);
unsigned input_translate_str_to_bind_id(const char *str); // Returns RARCH_BIND_LIST_END on not found.

void input_config_parse_joy_button(config_file_t *conf, const char *prefix,
      const char *btn, struct retro_keybind *bind);
void input_config_parse_joy_axis(config_file_t *conf, const char *prefix,
      const char *axis, struct retro_keybind *bind);

void input_push_analog_dpad(struct retro_keybind *binds, unsigned mode);
void input_pop_analog_dpad(struct retro_keybind *binds);

void quick_swap_controllers(void);

#ifdef __cplusplus
}
#endif

#endif

