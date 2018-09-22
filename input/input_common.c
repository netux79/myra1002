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

#include "input_common.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../general.h"
#include "../driver.h"
#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include "../file.h"

static const rarch_joypad_driver_t *joypad_drivers[] = {
   NULL,
};

const rarch_joypad_driver_t *input_joypad_init_driver(const char *ident)
{
   unsigned i;
   if (!ident || !*ident)
      return input_joypad_init_first();

   for (i = 0; joypad_drivers[i]; i++)
   {
      if (strcmp(ident, joypad_drivers[i]->ident) == 0 && joypad_drivers[i]->init())
      {
         RARCH_LOG("Found joypad driver: \"%s\".\n", joypad_drivers[i]->ident);
         return joypad_drivers[i];
      }
   }

   return NULL;
}

const rarch_joypad_driver_t *input_joypad_init_first(void)
{
   unsigned i;
   for (i = 0; joypad_drivers[i]; i++)
   {
      if (joypad_drivers[i]->init())
      {
         RARCH_LOG("Found joypad driver: \"%s\".\n", joypad_drivers[i]->ident);
         return joypad_drivers[i];
      }
   }

   return NULL;
}

void input_joypad_poll(const rarch_joypad_driver_t *driver)
{
   if (driver)
      driver->poll();
}

const char *input_joypad_name(const rarch_joypad_driver_t *driver, unsigned joypad)
{
   if (!driver)
      return NULL;

   return driver->name(joypad);
}

bool input_joypad_set_rumble(const rarch_joypad_driver_t *driver,
      unsigned port, enum retro_rumble_effect effect, uint16_t strength)
{
   if (!driver || !driver->set_rumble)
      return false;

   /* redirect to the controller binded to the player */
   port = g_settings.input.device_port[port];

   if (port < 0 || port >= MAX_PLAYERS)
      return false;

   return driver->set_rumble(port, effect, strength);
}

bool input_joypad_pressed(const rarch_joypad_driver_t *driver,
      unsigned port, const struct retro_keybind *binds, unsigned key)
{
   if (!driver)
      return false;

   if (port < 0 || port >= MAX_PLAYERS)
      return false;

   if (!binds[key].valid)
      return false;

   uint64_t joykey = binds[key].joykey;

   if (driver->button(port, (uint16_t)joykey))
      return true;

   uint32_t joyaxis = binds[key].joyaxis;

   int16_t axis = driver->axis(port, joyaxis);
   float scaled_axis = (float)abs(axis) / 0x8000;
   return scaled_axis > g_settings.input.axis_threshold;
}

int16_t input_joypad_analog(const rarch_joypad_driver_t *driver,
      unsigned port, unsigned index, unsigned id, const struct retro_keybind *binds)
{
   if (!driver)
      return 0;

   if (port < 0 || port >= MAX_PLAYERS)
      return 0;

   unsigned id_minus = 0;
   unsigned id_plus  = 0;
   input_conv_analog_id_to_bind_id(index, id, &id_minus, &id_plus);

   const struct retro_keybind *bind_minus = &binds[id_minus];
   const struct retro_keybind *bind_plus  = &binds[id_plus];
   if (!bind_minus->valid || !bind_plus->valid)
      return 0;

   uint32_t axis_minus = bind_minus->joyaxis;
   uint32_t axis_plus  = bind_plus->joyaxis;

   int16_t pressed_minus = abs(driver->axis(port, axis_minus));
   int16_t pressed_plus  = abs(driver->axis(port, axis_plus));

   int16_t res = pressed_plus - pressed_minus;

   if (res != 0)
      return res;

   uint64_t key_minus = bind_minus->joykey;
   uint64_t key_plus  = bind_plus->joykey;

   int16_t digital_left  = driver->button(port, (uint16_t)key_minus) ? -0x7fff : 0;
   int16_t digital_right = driver->button(port, (uint16_t)key_plus)  ?  0x7fff : 0;
   return digital_right + digital_left;
}

int16_t input_joypad_axis_raw(const rarch_joypad_driver_t *driver,
      unsigned joypad, unsigned axis)
{
   if (!driver)
      return 0;

   return driver->axis(joypad, AXIS_POS(axis)) + driver->axis(joypad, AXIS_NEG(axis));
}

bool input_joypad_button_raw(const rarch_joypad_driver_t *driver,
      unsigned joypad, unsigned button)
{
   if (!driver)
      return false;

   return driver->button(joypad, button);
}

bool input_joypad_hat_raw(const rarch_joypad_driver_t *driver,
      unsigned joypad, unsigned hat_dir, unsigned hat)
{
   if (!driver)
      return false;

   return driver->button(joypad, HAT_MAP(hat, hat_dir));
}

void input_joypad_hotplug(unsigned port, unsigned device_type, const char *padname, bool activated)
{
   char msg[128];
   unsigned p;

   /* look for the associated player of the port */
   for (p=0; g_settings.input.device_port[p]!=port && p<MAX_PLAYERS; p++);
   
   if (activated)
      snprintf(msg, sizeof(msg), "%s plugged to player %u", padname, p+1);
   else
      snprintf(msg, sizeof(msg), "%s unplugged from player %u", g_settings.input.device_names[port], p+1);
   
   /* Notify the controller change */
   msg_queue_push(g_extern.msg_queue, msg, 0, 80);
   
   /* Set controller's name and/or default binding */
   unsigned action = (1ULL << KEYBINDS_ACTION_SET_PAD_NAME);
   action |= g_settings.input.autoconf_buttons ? (1ULL << KEYBINDS_ACTION_SET_DEFAULT_BINDS) : 0;
   
   if (driver.input && driver.input->set_keybinds)
      driver.input->set_keybinds(NULL, device_type, port, 0, action);   
}


static const char *bind_player_prefix[MAX_PLAYERS] = {
   "input_player1",
   "input_player2",
   "input_player3",
   "input_player4",
};

#define DECLARE_BIND(x, bind, desc) { true, 0, #x, desc, bind }
#define DECLARE_META_BIND(level, x, bind, desc) { true, level, #x, desc, bind }

const struct input_bind_map input_config_bind_map[RARCH_BIND_LIST_END_NULL] = {
      DECLARE_BIND(b,         RETRO_DEVICE_ID_JOYPAD_B, "B button (down)"),
      DECLARE_BIND(y,         RETRO_DEVICE_ID_JOYPAD_Y, "Y button (left)"),
      DECLARE_BIND(select,    RETRO_DEVICE_ID_JOYPAD_SELECT, "Select button"),
      DECLARE_BIND(start,     RETRO_DEVICE_ID_JOYPAD_START, "Start button"),
      DECLARE_BIND(up,        RETRO_DEVICE_ID_JOYPAD_UP, "Up D-pad"),
      DECLARE_BIND(down,      RETRO_DEVICE_ID_JOYPAD_DOWN, "Down D-pad"),
      DECLARE_BIND(left,      RETRO_DEVICE_ID_JOYPAD_LEFT, "Left D-pad"),
      DECLARE_BIND(right,     RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right D-pad"),
      DECLARE_BIND(a,         RETRO_DEVICE_ID_JOYPAD_A, "A button (right)"),
      DECLARE_BIND(x,         RETRO_DEVICE_ID_JOYPAD_X, "X button (top)"),
      DECLARE_BIND(l,         RETRO_DEVICE_ID_JOYPAD_L, "L button (shoulder)"),
      DECLARE_BIND(r,         RETRO_DEVICE_ID_JOYPAD_R, "R button (shoulder)"),
      DECLARE_BIND(l2,        RETRO_DEVICE_ID_JOYPAD_L2, "L2 button (trigger)"),
      DECLARE_BIND(r2,        RETRO_DEVICE_ID_JOYPAD_R2, "R2 button (trigger)"),
      DECLARE_BIND(l3,        RETRO_DEVICE_ID_JOYPAD_L3, "L3 button (thumb)"),
      DECLARE_BIND(r3,        RETRO_DEVICE_ID_JOYPAD_R3, "R3 button (thumb)"),
      DECLARE_BIND(l_x_plus,  RARCH_ANALOG_LEFT_X_PLUS, "Left analog X+ (right)"),
      DECLARE_BIND(l_x_minus, RARCH_ANALOG_LEFT_X_MINUS, "Left analog X- (left)"),
      DECLARE_BIND(l_y_plus,  RARCH_ANALOG_LEFT_Y_PLUS, "Left analog Y+ (down)"),
      DECLARE_BIND(l_y_minus, RARCH_ANALOG_LEFT_Y_MINUS, "Left analog Y- (up)"),
      DECLARE_BIND(r_x_plus,  RARCH_ANALOG_RIGHT_X_PLUS, "Right analog X+ (right)"),
      DECLARE_BIND(r_x_minus, RARCH_ANALOG_RIGHT_X_MINUS, "Right analog X- (left)"),
      DECLARE_BIND(r_y_plus,  RARCH_ANALOG_RIGHT_Y_PLUS, "Right analog Y+ (down)"),
      DECLARE_BIND(r_y_minus, RARCH_ANALOG_RIGHT_Y_MINUS, "Right analog Y- (up)"),

      DECLARE_BIND(turbo, RARCH_TURBO_ENABLE, "Turbo enable"),

      DECLARE_META_BIND(1, toggle_fast_forward,   RARCH_FAST_FORWARD_KEY, "Fast forward toggle"),
      DECLARE_META_BIND(2, hold_fast_forward,     RARCH_FAST_FORWARD_HOLD_KEY, "Fast forward hold"),
      DECLARE_META_BIND(1, load_state,            RARCH_LOAD_STATE_KEY, "Load state"),
      DECLARE_META_BIND(1, save_state,            RARCH_SAVE_STATE_KEY, "Save state"),
      DECLARE_META_BIND(2, exit_emulator,         RARCH_QUIT_KEY, "Quit RetroArch"),
      DECLARE_META_BIND(2, state_slot_increase,   RARCH_STATE_SLOT_PLUS, "Savestate slot +"),
      DECLARE_META_BIND(2, state_slot_decrease,   RARCH_STATE_SLOT_MINUS, "Savestate slot -"),
      DECLARE_META_BIND(1, rewind,                RARCH_REWIND, "Rewind"),
      DECLARE_META_BIND(2, pause_toggle,          RARCH_PAUSE_TOGGLE, "Pause toggle"),
      DECLARE_META_BIND(2, frame_advance,         RARCH_FRAMEADVANCE, "Frameadvance"),
      DECLARE_META_BIND(2, reset,                 RARCH_RESET, "Reset game"),
      DECLARE_META_BIND(2, screenshot,            RARCH_SCREENSHOT, "Take screenshot"),
      DECLARE_META_BIND(2, audio_mute,            RARCH_MUTE, "Audio mute toggle"),
      DECLARE_META_BIND(2, slowmotion,            RARCH_SLOWMOTION, "Slow motion"),
      DECLARE_META_BIND(2, enable_hotkey,         RARCH_ENABLE_HOTKEY, "Enable hotkeys"),
      DECLARE_META_BIND(2, volume_up,             RARCH_VOLUME_UP, "Volume +"),
      DECLARE_META_BIND(2, volume_down,           RARCH_VOLUME_DOWN, "Volume -"),
      DECLARE_META_BIND(2, disk_eject_toggle,     RARCH_DISK_EJECT_TOGGLE, "Disk eject toggle"),
      DECLARE_META_BIND(2, disk_next,             RARCH_DISK_NEXT, "Disk next"),
      DECLARE_META_BIND(2, quick_swap,            RARCH_QUICK_SWAP, "Controller Quick Swap"),
#ifdef HAVE_MENU
      DECLARE_META_BIND(1, menu_toggle,           RARCH_MENU_TOGGLE, "RGUI menu toggle"),
#endif
};

const char *input_config_get_prefix(unsigned player, bool meta)
{
   if (player == 0)
      return meta ? "input" : bind_player_prefix[player];
   else if (player != 0 && !meta)
      return bind_player_prefix[player];
   else
      return NULL; // Don't bother with meta bind for anyone else than first player.
}

unsigned input_translate_str_to_bind_id(const char *str)
{
   unsigned i;
   for (i = 0; input_config_bind_map[i].valid; i++)
      if (!strcmp(str, input_config_bind_map[i].base))
         return i;
   return RARCH_BIND_LIST_END;
}

static void parse_hat(struct retro_keybind *bind, const char *str)
{
   if (!isdigit(*str))
      return;

   char *dir = NULL;
   uint16_t hat = strtoul(str, &dir, 0);
   uint16_t hat_dir = 0;

   if (!dir)
   {
      RARCH_WARN("Found invalid hat in config!\n");
      return;
   }

   if (strcasecmp(dir, "up") == 0)
      hat_dir = HAT_UP_MASK;
   else if (strcasecmp(dir, "down") == 0)
      hat_dir = HAT_DOWN_MASK;
   else if (strcasecmp(dir, "left") == 0)
      hat_dir = HAT_LEFT_MASK;
   else if (strcasecmp(dir, "right") == 0)
      hat_dir = HAT_RIGHT_MASK;

   if (hat_dir)
      bind->joykey = HAT_MAP(hat, hat_dir);
}

void input_config_parse_joy_button(config_file_t *conf, const char *prefix,
      const char *btn, struct retro_keybind *bind)
{
   char tmp[64];
   char key[64];
   snprintf(key, sizeof(key), "%s_%s_btn", prefix, btn);

   if (config_get_array(conf, key, tmp, sizeof(tmp)))
   {
      const char *btn = tmp;
      if (strcmp(btn, "nul") == 0)
         bind->joykey = NO_BTN;
      else
      {
         if (*btn == 'h')
            parse_hat(bind, btn + 1);
         else
            bind->joykey = strtoull(tmp, NULL, 0);
      }
   }
}

void input_config_parse_joy_axis(config_file_t *conf, const char *prefix,
      const char *axis, struct retro_keybind *bind)
{
   char tmp[64];
   char key[64];
   snprintf(key, sizeof(key), "%s_%s_axis", prefix, axis);

   if (config_get_array(conf, key, tmp, sizeof(tmp)))
   {
      if (strcmp(tmp, "nul") == 0)
         bind->joyaxis = AXIS_NONE;
      else if (strlen(tmp) >= 2 && (*tmp == '+' || *tmp == '-'))
      {
         int axis = strtol(tmp + 1, NULL, 0);
         if (*tmp == '+')
            bind->joyaxis = AXIS_POS(axis);
         else
            bind->joyaxis = AXIS_NEG(axis);

      }
   }
}

void input_get_bind_string(char *buf, const struct retro_keybind *bind, unsigned port, size_t size)
{
   strlcpy(buf, "   -", size);/* By default, a centered dash indicating unassigned */

   if (g_settings.input.device_names[port][0] == '\0' || 
         (bind->joykey == NO_BTN && bind->joyaxis == AXIS_NONE))
      return;/* Nothing to do if No Device or No Button assigned */
   
   if (bind->joykey != NO_BTN)
   {
      if (driver.input->set_keybinds)
      {
         struct platform_bind key_label;
         key_label.joykey = bind->joykey;
         driver.input->set_keybinds(&key_label, 0, port, 0, (1ULL << KEYBINDS_ACTION_GET_BIND_LABEL));
         if (strcmp(key_label.desc, "") != 0)
            snprintf(buf, size, "%s (btn) ", key_label.desc);
      }
   }
   else if (bind->joyaxis != AXIS_NONE)
   {
      unsigned axis = 0;
      char dir = '\0';
      if (AXIS_NEG_GET(bind->joyaxis) != AXIS_DIR_NONE)
      {
         dir = '-';
         axis = AXIS_NEG_GET(bind->joyaxis);
      }
      else if (AXIS_POS_GET(bind->joyaxis) != AXIS_DIR_NONE)
      {
         dir = '+';
         axis = AXIS_POS_GET(bind->joyaxis);
      }
      snprintf(buf, size, "%c%u (axis) ", dir, axis);
   }
}

void input_push_analog_dpad(struct retro_keybind *binds, unsigned mode)
{
   unsigned i;
   unsigned analog_base;
   switch (mode)
   {
      case ANALOG_DPAD_LSTICK:
      case ANALOG_DPAD_RSTICK:
         analog_base = mode == ANALOG_DPAD_LSTICK ? RARCH_ANALOG_LEFT_X_PLUS : RARCH_ANALOG_RIGHT_X_PLUS;

         binds[RETRO_DEVICE_ID_JOYPAD_RIGHT].orig_joyaxis = binds[RETRO_DEVICE_ID_JOYPAD_RIGHT].joyaxis;
         binds[RETRO_DEVICE_ID_JOYPAD_LEFT].orig_joyaxis  = binds[RETRO_DEVICE_ID_JOYPAD_LEFT].joyaxis;
         binds[RETRO_DEVICE_ID_JOYPAD_DOWN].orig_joyaxis  = binds[RETRO_DEVICE_ID_JOYPAD_DOWN].joyaxis;
         binds[RETRO_DEVICE_ID_JOYPAD_UP].orig_joyaxis    = binds[RETRO_DEVICE_ID_JOYPAD_UP].joyaxis;

         // Inherit joyaxis from analogs.
         binds[RETRO_DEVICE_ID_JOYPAD_RIGHT].joyaxis = binds[analog_base + 0].joyaxis;
         binds[RETRO_DEVICE_ID_JOYPAD_LEFT].joyaxis  = binds[analog_base + 1].joyaxis;
         binds[RETRO_DEVICE_ID_JOYPAD_DOWN].joyaxis  = binds[analog_base + 2].joyaxis;
         binds[RETRO_DEVICE_ID_JOYPAD_UP].joyaxis    = binds[analog_base + 3].joyaxis;
         break;

      default:
         for (i = RETRO_DEVICE_ID_JOYPAD_UP; i <= RETRO_DEVICE_ID_JOYPAD_RIGHT; i++)
            binds[i].orig_joyaxis = binds[i].joyaxis;
         break;
   }
}

// Restore binds temporarily overridden by input_push_analog_dpad.
void input_pop_analog_dpad(struct retro_keybind *binds)
{
   unsigned i;
   for (i = RETRO_DEVICE_ID_JOYPAD_UP; i <= RETRO_DEVICE_ID_JOYPAD_RIGHT; i++)
      binds[i].joyaxis = binds[i].orig_joyaxis;
}

void quick_swap_controllers(void)
{
   unsigned cp, np, t;

   /* if first controller is No Device, we cannot proceed */
   if (g_settings.input.device_names[0][0] == '\0')
      return;

   /* Look for the first controller player to calculate the next */
   for (cp = 0; g_settings.input.device_port[cp]!=0 && cp < MAX_PLAYERS; cp++);
   np = (cp < g_settings.input.quick_swap_players - 1) ? cp+1 : 0;

   if (np != cp)
   {
      t = g_settings.input.device_port[np];
      g_settings.input.device_port[np] = 0;
      g_settings.input.device_port[cp] = t;

      char msg[50];
      snprintf(msg, sizeof(msg), "<%d:%s> switched to Player %d", 1, g_settings.input.device_names[0], np+1);
      msg_queue_push(g_extern.msg_queue, msg, 1, 90);                  
   }
}


