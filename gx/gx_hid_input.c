/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2014 - Hans-Kristian Arntzen
 *  Copyright (C) 2011-2014 - Daniel De Matteis
 *  Copyright (C) 2012-2014 - Michael Lelli
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

#include <stdint.h>
#include <gccore.h>
#include <ogc/pad.h>
#include <wiiuse/wpad.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "gx_hid_input.h"
#include "../driver.h"
#include "../libretro.h"

#define NUM_PADS 2 /* Matching the # of USB slots */
#define JS_THRESHOLD (40 * 256)

const struct platform_bind gx_hid_platform_keys[] = {
   { GX_HID_WIIMOTE_A, "Wiimote A" },
   { GX_HID_WIIMOTE_B, "Wiimote B" },
   { GX_HID_WIIMOTE_1, "Wiimote 1" },
   { GX_HID_WIIMOTE_2, "Wiimote 2" },
   { GX_HID_WIIMOTE_UP, "Wiimote Up" },
   { GX_HID_WIIMOTE_DOWN, "Wiimote Down" },
   { GX_HID_WIIMOTE_LEFT, "Wiimote Left" },
   { GX_HID_WIIMOTE_RIGHT, "Wiimote Right" },
   { GX_HID_WIIMOTE_PLUS, "Wiimote Plus" },
   { GX_HID_WIIMOTE_MINUS, "Wiimote Minus" },
   { GX_HID_WIIMOTE_HOME, "Wiimote Home" },
};

typedef struct gx_hid_input
{
   uint64_t pad_state[NUM_PADS];
   int16_t analog_state[NUM_PADS][2][2];
   bool ml_left, ml_right;
   bool ml_cursor, ml_pause, ml_start;
   int ml_x, ml_y;
} gx_hid_input_t;

extern const rarch_joypad_driver_t gx_hid_joypad;

#ifdef HW_RVL
static void gx_hid_power_cb(void)
{
   g_extern.lifecycle_state |= (1ULL << RARCH_QUIT_KEY);   
}
#endif
static void gx_hid_reset_cb(void)
{
   g_extern.lifecycle_state |= (1ULL << RARCH_MENU_TOGGLE);
}

static bool gx_hid_menu_input_state(uint64_t joykey, uint64_t state, int16_t a_state[][2])
{
   bool a_dir;

   switch (joykey)
   {
      case CONSOLE_MENU_A:
         return state & ((1ULL << GX_HID_WIIMOTE_2) | (1ULL << GX_HID_USBPAD_A));
      case CONSOLE_MENU_B:
         return state & ((1ULL << GX_HID_WIIMOTE_1) | (1ULL << GX_HID_USBPAD_B));
      case CONSOLE_MENU_START:
         return state & ((1ULL << GX_HID_WIIMOTE_PLUS) | (1ULL << GX_HID_USBPAD_START));
      case CONSOLE_MENU_SELECT:
         return state & ((1ULL << GX_HID_WIIMOTE_MINUS) | (1ULL << GX_HID_USBPAD_SELECT));
      case CONSOLE_MENU_UP:
		 a_dir = a_state[RETRO_DEVICE_INDEX_ANALOG_LEFT][RETRO_DEVICE_ID_ANALOG_Y] < -JS_THRESHOLD;
         return (state & ((1ULL << GX_HID_WIIMOTE_UP) | (1ULL << GX_HID_USBPAD_UP))) || a_dir;
      case CONSOLE_MENU_DOWN:
		 a_dir = a_state[RETRO_DEVICE_INDEX_ANALOG_LEFT][RETRO_DEVICE_ID_ANALOG_Y] > JS_THRESHOLD;
         return (state & ((1ULL << GX_HID_WIIMOTE_DOWN) | (1ULL << GX_HID_USBPAD_DOWN))) || a_dir;
      case CONSOLE_MENU_LEFT:
		 a_dir = a_state[RETRO_DEVICE_INDEX_ANALOG_LEFT][RETRO_DEVICE_ID_ANALOG_X] < -JS_THRESHOLD;
         return (state & ((1ULL << GX_HID_WIIMOTE_LEFT) | (1ULL << GX_HID_USBPAD_LEFT))) || a_dir;
      case CONSOLE_MENU_RIGHT:
		 a_dir = a_state[RETRO_DEVICE_INDEX_ANALOG_LEFT][RETRO_DEVICE_ID_ANALOG_X] > JS_THRESHOLD;
         return (state & ((1ULL << GX_HID_WIIMOTE_RIGHT) | (1ULL << GX_HID_USBPAD_RIGHT))) || a_dir;
      case CONSOLE_MENU_L:
         return state & ((1ULL << GX_HID_USBPAD_L));
      case CONSOLE_MENU_R:
         return state & ((1ULL << GX_HID_USBPAD_R));
      case CONSOLE_MENU_HOME:
         return state & ((1ULL << GX_HID_WIIMOTE_HOME) | (1ULL << GX_HID_USBPAD_HOME));
      default:
         return false;
   }
}

static int16_t gx_hid_mouse_state(gx_hid_input_t *gx, unsigned id)
{
   switch (id)
   {
      case RETRO_DEVICE_ID_MOUSE_X:
         return gx->ml_x;
      case RETRO_DEVICE_ID_MOUSE_Y:
         return gx->ml_y;
      case RETRO_DEVICE_ID_MOUSE_LEFT:
         return gx->ml_left;
      case RETRO_DEVICE_ID_MOUSE_RIGHT:
         return gx->ml_right;
      default:
         return 0;
   }
}

static int16_t gx_hid_lightgun_state(gx_hid_input_t *gx, unsigned id)
{
   switch (id)
   {
      case RETRO_DEVICE_ID_LIGHTGUN_X:
         return gx->ml_x;
      case RETRO_DEVICE_ID_LIGHTGUN_Y:
         return gx->ml_y;
      case RETRO_DEVICE_ID_LIGHTGUN_TRIGGER:
         return gx->ml_left;
      case RETRO_DEVICE_ID_LIGHTGUN_CURSOR:
         return gx->ml_cursor;
      case RETRO_DEVICE_ID_LIGHTGUN_TURBO:
         return gx->ml_right;
      case RETRO_DEVICE_ID_LIGHTGUN_PAUSE:
         return gx->ml_pause;
      case RETRO_DEVICE_ID_LIGHTGUN_START:
         return gx->ml_start;
      default:
         return 0;
   }
}

static int16_t gx_hid_input_state(void *data, const struct retro_keybind **binds,
      unsigned port, unsigned device,
      unsigned index, unsigned id)
{
   gx_hid_input_t *gx = (gx_hid_input_t*)data;

   if (port >= NUM_PADS)
      return 0;

   switch (device)
   {
      case RETRO_DEVICE_JOYPAD:
         if (binds[port][id].joykey >= CONSOLE_MENU_FIRST && binds[port][id].joykey <= CONSOLE_MENU_LAST)
            return gx_hid_menu_input_state(binds[port][id].joykey, gx->pad_state[port], gx->analog_state[port]) ? 1 : 0;
         else
            return input_joypad_pressed(&gx_hid_joypad, port, binds[port], id);
      case RETRO_DEVICE_ANALOG:
         return input_joypad_analog(&gx_hid_joypad, port, index, id, binds[port]);
      case RETRO_DEVICE_MOUSE:
         return gx_hid_mouse_state(gx, id);
	  case RETRO_DEVICE_LIGHTGUN:
         return gx_hid_lightgun_state(gx, id);
      default:
         return 0;
   }
}

static void gx_hid_input_free_input(void *data)
{
   if (data)
   {
      /* shutdown pads */
      usbpad_shutdown(true);
      free(data);
   }
}

static void gx_hid_input_set_keybinds(void *data, unsigned device, unsigned port,
      unsigned id, unsigned keybind_action)
{
   if (keybind_action & (1ULL << KEYBINDS_ACTION_SET_DEFAULT_BIND))
   {
      g_settings.input.binds[port][id].joykey = g_settings.input.binds[port][id].def_joykey;
      g_settings.input.binds[port][id].joyaxis = g_settings.input.binds[port][id].def_joyaxis;
   }
   
    /* Set the name when binding default keys too */
   if (keybind_action & (1ULL << KEYBINDS_ACTION_SET_PAD_NAME))
   {
      const char *pad_name = usbpad_padname(port);
      
      if (pad_name)
      {
         /* We are only supporting GXPAD devices */
         g_settings.input.device[port] = device;
         strlcpy(g_settings.input.device_names[port], pad_name, sizeof(g_settings.input.device_names[port]));
      }
      else /* Clear device */
      {
         g_settings.input.device[port] = 0;
         g_settings.input.device_names[port][0] = '\0';
      }
   }

   if (keybind_action & (1ULL << KEYBINDS_ACTION_SET_DEFAULT_BINDS))
   {
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_B].def_joykey       = GX_HID_USBPAD_B;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_Y].def_joykey       = GX_HID_USBPAD_Y;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_SELECT].def_joykey  = GX_HID_USBPAD_SELECT;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_START].def_joykey   = GX_HID_USBPAD_START;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_UP].def_joykey      = GX_HID_USBPAD_UP;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_DOWN].def_joykey    = GX_HID_USBPAD_DOWN;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_LEFT].def_joykey    = GX_HID_USBPAD_LEFT;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_RIGHT].def_joykey   = GX_HID_USBPAD_RIGHT;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_A].def_joykey       = GX_HID_USBPAD_A;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_X].def_joykey       = GX_HID_USBPAD_X;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_L].def_joykey       = GX_HID_USBPAD_L;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_R].def_joykey       = GX_HID_USBPAD_R;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_L2].def_joykey      = GX_HID_USBPAD_L2;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_R2].def_joykey      = GX_HID_USBPAD_R2;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_L3].def_joykey      = GX_HID_USBPAD_L3;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_R3].def_joykey      = GX_HID_USBPAD_R3;
      g_settings.input.binds[port][RARCH_ANALOG_LEFT_X_PLUS].def_joykey       = NO_BTN;
      g_settings.input.binds[port][RARCH_ANALOG_LEFT_X_MINUS].def_joykey      = NO_BTN;
      g_settings.input.binds[port][RARCH_ANALOG_LEFT_Y_PLUS].def_joykey       = NO_BTN;
      g_settings.input.binds[port][RARCH_ANALOG_LEFT_Y_MINUS].def_joykey      = NO_BTN;
      g_settings.input.binds[port][RARCH_ANALOG_RIGHT_X_PLUS].def_joykey      = NO_BTN;
      g_settings.input.binds[port][RARCH_ANALOG_RIGHT_X_MINUS].def_joykey     = NO_BTN;
      g_settings.input.binds[port][RARCH_ANALOG_RIGHT_Y_PLUS].def_joykey      = NO_BTN;
      g_settings.input.binds[port][RARCH_ANALOG_RIGHT_Y_MINUS].def_joykey     = NO_BTN;
      g_settings.input.binds[port][RARCH_TURBO_ENABLE].def_joykey             = NO_BTN;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_B].def_joyaxis      = AXIS_NONE;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_Y].def_joyaxis      = AXIS_NONE;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_SELECT].def_joyaxis = AXIS_NONE;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_START].def_joyaxis  = AXIS_NONE;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_UP].def_joyaxis     = AXIS_NONE;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_DOWN].def_joyaxis   = AXIS_NONE;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_LEFT].def_joyaxis   = AXIS_NONE;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_RIGHT].def_joyaxis  = AXIS_NONE;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_A].def_joyaxis      = AXIS_NONE;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_X].def_joyaxis      = AXIS_NONE;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_L].def_joyaxis      = AXIS_NONE;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_R].def_joyaxis      = AXIS_NONE;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_L2].def_joyaxis     = AXIS_NONE;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_R2].def_joyaxis     = AXIS_NONE;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_L3].def_joyaxis     = AXIS_NONE;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_R3].def_joyaxis     = AXIS_NONE;
      g_settings.input.binds[port][RARCH_ANALOG_LEFT_X_PLUS].def_joyaxis      = AXIS_POS(0);
      g_settings.input.binds[port][RARCH_ANALOG_LEFT_X_MINUS].def_joyaxis     = AXIS_NEG(0);
      g_settings.input.binds[port][RARCH_ANALOG_LEFT_Y_PLUS].def_joyaxis      = AXIS_POS(1);
      g_settings.input.binds[port][RARCH_ANALOG_LEFT_Y_MINUS].def_joyaxis     = AXIS_NEG(1);
      g_settings.input.binds[port][RARCH_ANALOG_RIGHT_X_PLUS].def_joyaxis     = AXIS_POS(2);
      g_settings.input.binds[port][RARCH_ANALOG_RIGHT_X_MINUS].def_joyaxis    = AXIS_NEG(2);
      g_settings.input.binds[port][RARCH_ANALOG_RIGHT_Y_PLUS].def_joyaxis     = AXIS_POS(3);
      g_settings.input.binds[port][RARCH_ANALOG_RIGHT_Y_MINUS].def_joyaxis    = AXIS_NEG(3);
      g_settings.input.binds[port][RARCH_TURBO_ENABLE].def_joyaxis            = AXIS_NONE;

      /* Assign the default binding to the actual controller */
      for (unsigned i = 0; i < RARCH_CUSTOM_BIND_LIST_END; i++)
      {
         g_settings.input.binds[port][i].id = i;
         g_settings.input.binds[port][i].joykey = g_settings.input.binds[port][i].def_joykey;
         g_settings.input.binds[port][i].joyaxis = g_settings.input.binds[port][i].def_joyaxis;
      }     
   }

   if (keybind_action & (1ULL << KEYBINDS_ACTION_GET_BIND_LABEL))
   {
      struct platform_bind *ret = (struct platform_bind*)data;

      if (ret->joykey >= GX_HID_WIIMOTE_A && ret->joykey <= GX_HID_WIIMOTE_HOME)
         strlcpy(ret->desc, gx_hid_platform_keys[ret->joykey - GX_HID_WIIMOTE_A].desc, sizeof(ret->desc));
      else
         strlcpy(ret->desc, usbpad_label(port, ret->joykey), sizeof(ret->desc));
   }
}

static void *gx_hid_input_init(void)
{
   gx_hid_input_t *gx = (gx_hid_input_t*)calloc(1, sizeof(*gx));
   if (!gx)
      return NULL;

   WPAD_Init();
   WPAD_SetVRes(0, 640, 480);
   WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR); /* read IR info from wiimote 1 */

   SYS_SetResetCallback(gx_hid_reset_cb);
   SYS_SetPowerCallback(gx_hid_power_cb);

   usbpad_init(NUM_PADS, true);

   return gx;
}

static void gx_hid_input_poll_ml(gx_hid_input_t *gx)
{
   ir_t ir;
   WPAD_IR(WPAD_CHAN_0, &ir);
   if (ir.valid) {
	   gx->ml_x = ir.x;
	   gx->ml_y = ir.y;
   }
   else {
	   /* reset the position if we are offscreen */
	   gx->ml_x = 0;
	   gx->ml_y = 0;
   }

   uint64_t *state = &gx->pad_state[0]; /* Check buttons from first port */
   gx->ml_left	 = *state & (1ULL << GX_HID_WIIMOTE_B); /* trigger */
   gx->ml_cursor = *state & (1ULL << GX_HID_WIIMOTE_1);
   gx->ml_right	 = *state & (1ULL << GX_HID_WIIMOTE_A); /* turbo */
   gx->ml_pause	 = *state & (1ULL << GX_HID_WIIMOTE_PLUS);
   gx->ml_start	 = *state & (1ULL << GX_HID_WIIMOTE_MINUS);
}

static void gx_hid_input_poll(void *data)
{
   static bool lt_active[NUM_PADS];
   gx_hid_input_t *gx = (gx_hid_input_t*)data;

   for (unsigned port = 0; port < NUM_PADS; port++)
   {
      bool hotplug = false;
      uint64_t *state = &gx->pad_state[port];

      if (usbpad_avail(port))
      {
         *state = usbpad_buttons(port);

         gx->analog_state[port][RETRO_DEVICE_INDEX_ANALOG_LEFT][RETRO_DEVICE_ID_ANALOG_X] = usbpad_js_lx(port);
         gx->analog_state[port][RETRO_DEVICE_INDEX_ANALOG_LEFT][RETRO_DEVICE_ID_ANALOG_Y] = usbpad_js_ly(port);
         gx->analog_state[port][RETRO_DEVICE_INDEX_ANALOG_RIGHT][RETRO_DEVICE_ID_ANALOG_X] = usbpad_js_rx(port);
         gx->analog_state[port][RETRO_DEVICE_INDEX_ANALOG_RIGHT][RETRO_DEVICE_ID_ANALOG_Y] = usbpad_js_ry(port);

         if (usbpad_nanalogs(port) > 4)
         {
            /*  when having a 3rd analog we assume it is the hat control */
            int16_t js3x = usbpad_analog(port, 4);
            int16_t js3y = usbpad_analog(port, 5);
            if (js3x > JS_THRESHOLD)  *state |= (1ULL << GX_HID_USBPAD_RIGHT);
            if (js3x < -JS_THRESHOLD) *state |= (1ULL << GX_HID_USBPAD_LEFT);
            if (js3y > JS_THRESHOLD)  *state |= (1ULL << GX_HID_USBPAD_DOWN);
            if (js3y < -JS_THRESHOLD) *state |= (1ULL << GX_HID_USBPAD_UP);
         }

         hotplug = (lt_active[port]) ? false : true;
         lt_active[port] = true;
      }
      else
      {
         *state = 0; /* reset the button state */
         for (uint8_t j = 0; j < 2; j++)
            for (uint8_t i = 0; i < 2; i++)
               gx->analog_state[port][j][i] = 0; /*  clear also all the analogs */

         hotplug = (lt_active[port]) ? true : false;
         lt_active[port] = false;
      }

      /* show the pad change */
      if (hotplug) input_joypad_hotplug(port, DEVICE_GXPAD, usbpad_padname(port), lt_active[port]);
   }

   uint64_t *state_p1 = &gx->pad_state[0];

   WPAD_ReadPending(WPAD_CHAN_0, NULL);
   /* Get the state of the port 0 wiimote */
   uint32_t down = WPAD_ButtonsHeld(WPAD_CHAN_0);

   *state_p1 |= (down & WPAD_BUTTON_A) ? (1ULL << GX_HID_WIIMOTE_A) : 0;
   *state_p1 |= (down & WPAD_BUTTON_B) ? (1ULL << GX_HID_WIIMOTE_B) : 0;
   *state_p1 |= (down & WPAD_BUTTON_1) ? (1ULL << GX_HID_WIIMOTE_1) : 0;
   *state_p1 |= (down & WPAD_BUTTON_2) ? (1ULL << GX_HID_WIIMOTE_2) : 0;
   *state_p1 |= (down & WPAD_BUTTON_PLUS) ? (1ULL << GX_HID_WIIMOTE_PLUS) : 0;
   *state_p1 |= (down & WPAD_BUTTON_MINUS) ? (1ULL << GX_HID_WIIMOTE_MINUS) : 0;
   *state_p1 |= (down & WPAD_BUTTON_HOME) ? (1ULL << GX_HID_WIIMOTE_HOME) : 0;
   /* rotated d-pad on Wiimote */
   *state_p1 |= (down & WPAD_BUTTON_UP) ? (1ULL << GX_HID_WIIMOTE_LEFT) : 0;
   *state_p1 |= (down & WPAD_BUTTON_DOWN) ? (1ULL << GX_HID_WIIMOTE_RIGHT) : 0;
   *state_p1 |= (down & WPAD_BUTTON_LEFT) ? (1ULL << GX_HID_WIIMOTE_DOWN) : 0;
   *state_p1 |= (down & WPAD_BUTTON_RIGHT) ? (1ULL << GX_HID_WIIMOTE_UP) : 0;

   /* poll mouse & lightgun data */
   gx_hid_input_poll_ml(gx);
}

static bool gx_hid_input_key_pressed(void *data, int key)
{
   return (g_extern.lifecycle_state & (1ULL << key)) || input_joypad_pressed(&gx_hid_joypad, 0, g_settings.input.binds[0], key);
}

static uint64_t gx_hid_input_get_capabilities(void *data)
{
   uint64_t caps = 0;

   caps |= (1 << RETRO_DEVICE_JOYPAD);
   caps |= (1 << RETRO_DEVICE_ANALOG);
   caps |= (1 << RETRO_DEVICE_MOUSE);
   caps |= (1 << RETRO_DEVICE_LIGHTGUN);
   return caps;
}

static const rarch_joypad_driver_t *gx_hid_input_get_joypad_driver(void *data)
{
   return &gx_hid_joypad;
}

const input_driver_t input_gx_hid = {
   gx_hid_input_init,
   gx_hid_input_poll,
   gx_hid_input_state,
   gx_hid_input_key_pressed,
   gx_hid_input_free_input,
   gx_hid_input_set_keybinds,
   NULL,
   NULL,
   gx_hid_input_get_capabilities,
   "usb",
   NULL,
   NULL,
   gx_hid_input_get_joypad_driver,
};


static bool gx_hid_joypad_init(void)
{
   return true;
}

static bool gx_hid_joypad_button(unsigned port_num, uint16_t joykey)
{
   gx_hid_input_t *gx = (gx_hid_input_t*)driver.input_data;

   if (port_num < NUM_PADS)
      return gx->pad_state[port_num] & (1ULL << joykey);

   return false;
}

static int16_t gx_hid_joypad_axis(unsigned port_num, uint32_t joyaxis)
{
   gx_hid_input_t *gx = (gx_hid_input_t*)driver.input_data;
   if (joyaxis == AXIS_NONE || port_num >= NUM_PADS)
      return 0;

   int val = 0;

   int axis    = -1;
   bool is_neg = false;
   bool is_pos = false;

   if (AXIS_NEG_GET(joyaxis) < 4)
   {
      axis = AXIS_NEG_GET(joyaxis);
      is_neg = true;
   }
   else if (AXIS_POS_GET(joyaxis) < 4)
   {
      axis = AXIS_POS_GET(joyaxis);
      is_pos = true;
   }

   switch (axis)
   {
      case 0: val = gx->analog_state[port_num][0][0]; break;
      case 1: val = gx->analog_state[port_num][0][1]; break;
      case 2: val = gx->analog_state[port_num][1][0]; break;
      case 3: val = gx->analog_state[port_num][1][1]; break;
   }

   if (is_neg && val > 0)
      val = 0;
   else if (is_pos && val < 0)
      val = 0;

   return val;
}

static void gx_hid_joypad_poll(void)
{
}

static bool gx_hid_joypad_query_pad(unsigned pad)
{
   gx_hid_input_t *gx = (gx_hid_input_t*)driver.input_data;
   return pad < MAX_PLAYERS && gx->pad_state[pad];
}

static const char *gx_hid_joypad_name(unsigned pad)
{
   return NULL;
}

static void gx_hid_joypad_destroy(void)
{
}

const rarch_joypad_driver_t gx_hid_joypad = {
   gx_hid_joypad_init,
   gx_hid_joypad_query_pad,
   gx_hid_joypad_destroy,
   gx_hid_joypad_button,
   gx_hid_joypad_axis,
   gx_hid_joypad_poll,
   NULL,
   gx_hid_joypad_name,
   "gxhid joy",
};
