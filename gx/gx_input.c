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
#include <string.h>

#include "gx_input.h"
#include "../driver.h"
#include "../libretro.h"
#include <stdlib.h>

#define GX_NUM_PADS 8
#define GX_JS_THRESHOLD (40 * 256)

typedef struct gx_input
{
   uint64_t pad_state[GX_NUM_PADS];
   int16_t analog_state[GX_NUM_PADS][2][2];
   bool mouse_l, mouse_r, mouse_m;
   int mouse_x, mouse_y;
   int mouse_last_x, mouse_last_y;
} gx_input_t;

extern const rarch_joypad_driver_t gx_joypad;

static bool g_menu;
#ifdef HW_RVL
static bool g_quit;

static void gx_power_callback(void)
{
   g_quit = true;
}
#endif
static void gx_reset_cb(void)
{
   g_menu = true;
}

static bool gx_menu_input_state(uint64_t joykey, uint64_t state, int16_t a_state[][2])
{
   bool a_dir;

   switch (joykey)
   {
      case CONSOLE_MENU_A:
         return state & (1ULL << GX_GXPAD_A);
      case CONSOLE_MENU_B:
         return state & (1ULL << GX_GXPAD_B);
      case CONSOLE_MENU_X:
         return state & (1ULL << GX_GXPAD_X);
      case CONSOLE_MENU_Y:
         return state & (1ULL << GX_GXPAD_Y);
      case CONSOLE_MENU_START:
         return state & (1ULL << GX_GXPAD_START);
      case CONSOLE_MENU_SELECT:
         return state & (1ULL << GX_GXPAD_SELECT);
      case CONSOLE_MENU_UP:
		 a_dir = a_state[RETRO_DEVICE_INDEX_ANALOG_LEFT][RETRO_DEVICE_ID_ANALOG_Y] < -GX_JS_THRESHOLD;
         return (state & (1ULL << GX_GXPAD_UP)) || a_dir;
      case CONSOLE_MENU_DOWN:
		 a_dir = a_state[RETRO_DEVICE_INDEX_ANALOG_LEFT][RETRO_DEVICE_ID_ANALOG_Y] > GX_JS_THRESHOLD;
         return (state & (1ULL << GX_GXPAD_DOWN)) || a_dir;
      case CONSOLE_MENU_LEFT:
		 a_dir = a_state[RETRO_DEVICE_INDEX_ANALOG_LEFT][RETRO_DEVICE_ID_ANALOG_X] < -GX_JS_THRESHOLD;
         return (state & (1ULL << GX_GXPAD_LEFT)) || a_dir;
      case CONSOLE_MENU_RIGHT:
		 a_dir = a_state[RETRO_DEVICE_INDEX_ANALOG_LEFT][RETRO_DEVICE_ID_ANALOG_X] > GX_JS_THRESHOLD;
         return (state & (1ULL << GX_GXPAD_RIGHT)) || a_dir;
      case CONSOLE_MENU_L:
         return state & (1ULL << GX_GXPAD_L);
      case CONSOLE_MENU_R:
         return state & (1ULL << GX_GXPAD_R);
      case CONSOLE_MENU_HOME:
         return state & (1ULL << GX_GXPAD_HOME);
      default:
         return false;
   }
}

static int16_t gx_mouse_state(gx_input_t *gx, unsigned id)
{
   switch (id)
   {
      case RETRO_DEVICE_ID_MOUSE_X:
         return gx->mouse_x - gx->mouse_last_x;
      case RETRO_DEVICE_ID_MOUSE_Y:
         return gx->mouse_y - gx->mouse_last_y;
      case RETRO_DEVICE_ID_MOUSE_LEFT:
         return gx->mouse_l;
      case RETRO_DEVICE_ID_MOUSE_RIGHT:
         return gx->mouse_r;
      default:
         return 0;
   }
}

static int16_t gx_lightgun_state(gx_input_t *gx, unsigned id)
{
   switch (id)
   {
      case RETRO_DEVICE_ID_LIGHTGUN_X:
         return gx->mouse_x - gx->mouse_last_x;
      case RETRO_DEVICE_ID_LIGHTGUN_Y:
         return gx->mouse_y - gx->mouse_last_y;
      case RETRO_DEVICE_ID_LIGHTGUN_TRIGGER:
         return gx->mouse_l;
      case RETRO_DEVICE_ID_LIGHTGUN_CURSOR:
         return gx->mouse_m;
      case RETRO_DEVICE_ID_LIGHTGUN_TURBO:
         return gx->mouse_r;
      case RETRO_DEVICE_ID_LIGHTGUN_START:
         return gx->mouse_m && gx->mouse_r;
      case RETRO_DEVICE_ID_LIGHTGUN_PAUSE:
         return gx->mouse_m && gx->mouse_l;
      default:
         return 0;
   }
}

static int16_t gx_input_state(void *data, const struct retro_keybind **binds,
      unsigned port, unsigned device,
      unsigned index, unsigned id)
{
   gx_input_t *gx = (gx_input_t*)data;

   if (port >= GX_NUM_PADS)
      return 0;

   switch (device)
   {
      case RETRO_DEVICE_JOYPAD:
         if (binds[port][id].joykey >= CONSOLE_MENU_FIRST && binds[port][id].joykey <= CONSOLE_MENU_LAST)
            return gx_menu_input_state(binds[port][id].joykey, gx->pad_state[port], gx->analog_state[port]) ? 1 : 0;
         else
            return input_joypad_pressed(&gx_joypad, port, binds[port], id);
      case RETRO_DEVICE_ANALOG:
         return input_joypad_analog(&gx_joypad, port, index, id, binds[port]);
      case RETRO_DEVICE_MOUSE:
         return gx_mouse_state(gx, id);
	  case RETRO_DEVICE_LIGHTGUN:
         return gx_lightgun_state(gx, id);
      default:
         return 0;
   }
}

static void gx_input_free_input(void *data)
{
   (void)data;

   if (!driver.video_data) {
	   /* shutdown pads */
	   gxpad_shutdown();
   }
}

static void gx_input_set_keybinds(void *data, unsigned device, unsigned port,
      unsigned id, unsigned keybind_action)
{
   uint64_t *key = &g_settings.input.binds[port][id].joykey;

   if (keybind_action & (1ULL << KEYBINDS_ACTION_SET_DEFAULT_BIND))
      *key = g_settings.input.binds[port][id].def_joykey;

   /* Set the name when binding default keys too */
   if ((keybind_action & (1ULL << KEYBINDS_ACTION_SET_PAD_NAME)) || (keybind_action & (1ULL << KEYBINDS_ACTION_SET_DEFAULT_BINDS)))
   {
      /* We are not supporting different devices but GXPAD */
      g_settings.input.device[port] = DEVICE_GXPAD;
      strlcpy(g_settings.input.device_names[port], gxpad_padname(port), sizeof(g_settings.input.device_names[port]));
   }

   if (keybind_action & (1ULL << KEYBINDS_ACTION_SET_DEFAULT_BINDS))
   {
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_B].def_joykey       = GX_GXPAD_B;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_Y].def_joykey       = GX_GXPAD_Y;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_SELECT].def_joykey  = GX_GXPAD_SELECT;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_START].def_joykey   = GX_GXPAD_START;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_UP].def_joykey      = GX_GXPAD_UP;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_DOWN].def_joykey    = GX_GXPAD_DOWN;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_LEFT].def_joykey    = GX_GXPAD_LEFT;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_RIGHT].def_joykey   = GX_GXPAD_RIGHT;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_A].def_joykey       = GX_GXPAD_A;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_X].def_joykey       = GX_GXPAD_X;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_L].def_joykey       = GX_GXPAD_L;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_R].def_joykey       = GX_GXPAD_R;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_L2].def_joykey      = GX_GXPAD_L2;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_R2].def_joykey      = GX_GXPAD_R2;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_L3].def_joykey      = GX_GXPAD_L3;
      g_settings.input.binds[port][RETRO_DEVICE_ID_JOYPAD_R3].def_joykey      = GX_GXPAD_R3;
      g_settings.input.binds[port][RARCH_ANALOG_LEFT_X_PLUS].def_joykey       = NO_BTN;
      g_settings.input.binds[port][RARCH_ANALOG_LEFT_X_MINUS].def_joykey      = NO_BTN;
      g_settings.input.binds[port][RARCH_ANALOG_LEFT_Y_PLUS].def_joykey       = NO_BTN;
      g_settings.input.binds[port][RARCH_ANALOG_LEFT_Y_MINUS].def_joykey      = NO_BTN;
      g_settings.input.binds[port][RARCH_ANALOG_RIGHT_X_PLUS].def_joykey      = NO_BTN;
      g_settings.input.binds[port][RARCH_ANALOG_RIGHT_X_MINUS].def_joykey     = NO_BTN;
      g_settings.input.binds[port][RARCH_ANALOG_RIGHT_Y_PLUS].def_joykey      = NO_BTN;
      g_settings.input.binds[port][RARCH_ANALOG_RIGHT_Y_MINUS].def_joykey     = NO_BTN;
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

      /* Assign the default binding to the actual controller */
      for (unsigned i = 0; i < RARCH_CUSTOM_BIND_LIST_END - 1; i++)
      {
         g_settings.input.binds[port][i].id = i;
		 g_settings.input.binds[port][i].joykey = g_settings.input.binds[port][i].def_joykey;
		 g_settings.input.binds[port][i].joyaxis = g_settings.input.binds[port][i].def_joyaxis;
      }
   }

   if (keybind_action & (1ULL << KEYBINDS_ACTION_GET_BIND_LABEL))
   {
      struct platform_bind *ret = (struct platform_bind*)data;

      if (ret->joykey == NO_BTN)
         strlcpy(ret->desc, "No button", sizeof(ret->desc));
	  else
         strlcpy(ret->desc, gxpad_label(port, ret->joykey), sizeof(ret->desc));
   }
}

static void *gx_input_init(void)
{
   gx_input_t *gx = (gx_input_t*)calloc(1, sizeof(*gx));
   if (!gx)
      return NULL;

   SYS_SetResetCallback(gx_reset_cb);
   SYS_SetPowerCallback(gx_power_callback);

   gxpad_init();

   return gx;
}

/*
static void gx_input_poll_mouse(gx_input_t *gx)
{
   ir_t ir;
   WPAD_IR(0, &ir);
   gx->mouse_last_x = gx->mouse_x;
   gx->mouse_last_y = gx->mouse_y;
   gx->mouse_x = ir.x;
   gx->mouse_y = ir.y;

   uint64_t *state = &gx->pad_state[0]; / Check buttons from first port /
   gx->mouse_l = *state & (1ULL << GX_HID_WIIMOTE_B);
   gx->mouse_m = *state & (1ULL << GX_HID_WIIMOTE_1);
   gx->mouse_r = *state & (1ULL << GX_HID_WIIMOTE_A);
}
*/

static void gx_input_poll(void *data)
{
   static bool lt_active[GX_NUM_PADS];
   gx_input_t *gx = (gx_input_t*)data;

   for (unsigned port = 0; port < GX_NUM_PADS; port++)
   {
	  bool hotplug = false;
	  uint64_t *state = &gx->pad_state[port];

      if (gxpad_avail(port))
      {
	     *state = gxpad_buttons(port);

		 gx->analog_state[port][RETRO_DEVICE_INDEX_ANALOG_LEFT][RETRO_DEVICE_ID_ANALOG_X] = gxpad_js_lx(port);
		 gx->analog_state[port][RETRO_DEVICE_INDEX_ANALOG_LEFT][RETRO_DEVICE_ID_ANALOG_Y] = gxpad_js_ly(port);
		 gx->analog_state[port][RETRO_DEVICE_INDEX_ANALOG_RIGHT][RETRO_DEVICE_ID_ANALOG_X] = gxpad_js_rx(port);
		 gx->analog_state[port][RETRO_DEVICE_INDEX_ANALOG_RIGHT][RETRO_DEVICE_ID_ANALOG_Y] = gxpad_js_ry(port);

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

	  if (hotplug)
	  {
         /* show the pad change */
         char msg[128];
         if (lt_active[port])
		    snprintf(msg, sizeof(msg), "%s plugged -p %u", gxpad_padname(port), port);
		 else
			snprintf(msg, sizeof(msg), "%s unplugged -p %u", g_settings.input.device_names[port], port);
         msg_queue_push(g_extern.msg_queue, msg, 0, 80);

		 if (g_settings.input.autodetect_enable)
            gx_input_set_keybinds(NULL, DEVICE_GXPAD, port, 0, (1ULL << KEYBINDS_ACTION_SET_DEFAULT_BINDS));
         else
            gx_input_set_keybinds(NULL, DEVICE_GXPAD, port, 0, (1ULL << KEYBINDS_ACTION_SET_PAD_NAME));
	  }
   }

   uint64_t *state_p1 = &gx->pad_state[0];

   /* check if the user press the Wii's reset button, it works as the HOME button */
   *state_p1 |= g_menu ? (1ULL << GX_GXPAD_HOME) : 0;
   /* clear the reset Wii button flag */
   g_menu = false;

   /* poll mouse data
   gx_input_poll_mouse(gx); */

   /* Check if we need to get into the menu */
   uint64_t *lifecycle_state = &g_extern.lifecycle_state;
   *lifecycle_state &= ~((1ULL << RARCH_MENU_TOGGLE));

   if (*state_p1 & (1ULL << GX_GXPAD_HOME)) *lifecycle_state |= (1ULL << RARCH_MENU_TOGGLE);
}

static bool gx_input_key_pressed(void *data, int key)
{
   return (g_extern.lifecycle_state & (1ULL << key)) || input_joypad_pressed(&gx_joypad, 0, g_settings.input.binds[0], key);
}

static uint64_t gx_input_get_capabilities(void *data)
{
   uint64_t caps = 0;

   caps |= (1 << RETRO_DEVICE_JOYPAD);
   caps |= (1 << RETRO_DEVICE_ANALOG);
   caps |= (1 << RETRO_DEVICE_MOUSE);
   caps |= (1 << RETRO_DEVICE_LIGHTGUN);
   return caps;
}

static const rarch_joypad_driver_t *gx_input_get_joypad_driver(void *data)
{
   return &gx_joypad;
}

const input_driver_t input_gx = {
   gx_input_init,
   gx_input_poll,
   gx_input_state,
   gx_input_key_pressed,
   gx_input_free_input,
   gx_input_set_keybinds,
   NULL,
   NULL,
   gx_input_get_capabilities,
   "gx",

   NULL,
   NULL,
   gx_input_get_joypad_driver,
};


static bool gx_joypad_init(void)
{
   return true;
}

static bool gx_joypad_button(unsigned port_num, uint16_t joykey)
{
   gx_input_t *gx = (gx_input_t*)driver.input_data;

   if (port_num < GX_NUM_PADS)
      return gx->pad_state[port_num] & (1ULL << joykey);

   return false;
}

static int16_t gx_joypad_axis(unsigned port_num, uint32_t joyaxis)
{
   gx_input_t *gx = (gx_input_t*)driver.input_data;
   if (joyaxis == AXIS_NONE || port_num >= GX_NUM_PADS)
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

static void gx_joypad_poll(void)
{
}

static bool gx_joypad_query_pad(unsigned pad)
{
   gx_input_t *gx = (gx_input_t*)driver.input_data;
   return pad < MAX_PLAYERS && gx->pad_state[pad];
}

static const char *gx_joypad_name(unsigned pad)
{
   return NULL;
}

static void gx_joypad_destroy(void)
{
}

const rarch_joypad_driver_t gx_joypad = {
   gx_joypad_init,
   gx_joypad_query_pad,
   gx_joypad_destroy,
   gx_joypad_button,
   gx_joypad_axis,
   gx_joypad_poll,
   NULL,
   gx_joypad_name,
   "gx",
};
