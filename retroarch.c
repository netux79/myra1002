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

#include <stdbool.h>
#include "libretro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "driver.h"
#include "file.h"
#include "general.h"
#include "dynamic.h"
#include "performance.h"
#include "audio/utils.h"
#include "rewind.h"
#include "compat/strl.h"
#include "screenshot.h"
#include "compat/getopt_rarch.h"
#include "input/input_common.h"
#include "git_version.h"

// To avoid continous switching if we hold the button down, we require that the button must go from pressed,
// unpressed back to pressed to be able to toggle between then.
static void check_fast_forward_button(void)
{
   bool new_button_state = input_key_pressed_func(RARCH_FAST_FORWARD_KEY);
   bool new_hold_button_state = input_key_pressed_func(RARCH_FAST_FORWARD_HOLD_KEY);
   static bool old_button_state = false;
   static bool old_hold_button_state = false;

   if (new_button_state && !old_button_state)
   {
      driver.nonblock_state = !driver.nonblock_state;
      driver_set_nonblock_state(driver.nonblock_state);
   }
   else if (old_hold_button_state != new_hold_button_state)
   {
      driver.nonblock_state = new_hold_button_state;
      driver_set_nonblock_state(driver.nonblock_state);
   }

   old_button_state = new_button_state;
   old_hold_button_state = new_hold_button_state;
}

#if defined(HAVE_SCREENSHOTS)
static bool take_screenshot_raw(void)
{
   const void *data = g_extern.frame_cache.data;
   unsigned width   = g_extern.frame_cache.width;
   unsigned height  = g_extern.frame_cache.height;
   int pitch        = g_extern.frame_cache.pitch;

   const char *screenshot_dir = g_settings.screenshot_directory;
   char screenshot_path[PATH_MAX];
   if (!*g_settings.screenshot_directory)
   {
      fill_pathname_basedir(screenshot_path, g_extern.basename, sizeof(screenshot_path));
      screenshot_dir = screenshot_path;
   }

   // Negative pitch is needed as screenshot takes bottom-up,
   // but we use top-down.
   return screenshot_dump(screenshot_dir,
         (const uint8_t*)data + (height - 1) * pitch,
         width, height, -pitch, false);
}

void rarch_take_screenshot(void)
{
   if ((!*g_settings.screenshot_directory) && (!*g_extern.basename)) // No way to infer screenshot directory.
      return;

   bool ret = false;

   // Clear out message queue to avoid OSD fonts to appear on screenshot.
   msg_queue_clear(g_extern.msg_queue);

   if (g_extern.frame_cache.data)
      ret = take_screenshot_raw();
   else
      RARCH_ERR("Cannot take screenshot. GPU rendering is being used.\n");

   const char *msg = NULL;
   if (ret)
   {
      RARCH_LOG("Taking screenshot.\n");
      msg = "Taking screenshot.";
   }
   else
   {
      RARCH_WARN("Failed to take screenshot ...\n");
      msg = "Failed to take screenshot.";
   }

   if (g_extern.is_paused)
   {
      msg_queue_push(g_extern.msg_queue, msg, 1, 1);
      rarch_render_cached_frame();
   }
   else
      msg_queue_push(g_extern.msg_queue, msg, 1, 180);
}
#endif

static void readjust_audio_input_rate(void)
{
   int avail = audio_write_avail_func();
   int half_size = g_extern.audio_data.driver_buffer_size / 2;
   int delta_mid = avail - half_size;
   double direction = (double)delta_mid / half_size;

   double adjust = 1.0 + g_settings.audio.rate_control_delta * direction;

   g_extern.audio_data.src_ratio = g_extern.audio_data.orig_src_ratio * adjust;
}

static void video_frame(const void *data, unsigned width, unsigned height, size_t pitch)
{
   if (!g_extern.video_active)
      return;

   g_extern.frame_cache.data   = data;
   g_extern.frame_cache.width  = width;
   g_extern.frame_cache.height = height;
   g_extern.frame_cache.pitch  = pitch;

   const char *msg = msg_queue_pull(g_extern.msg_queue);
   driver.current_msg = msg;

#ifdef HAVE_SCALERS_BUILTIN
   if (g_extern.filter.filter && *g_extern.basename) /* only use filter if game is running */
   {
      rarch_softfilter_get_output_size(g_extern.filter.filter,
            &g_extern.frame.width, &g_extern.frame.height, width, height);

      g_extern.frame.pitch = g_extern.frame.width * g_extern.filter.out_bpp;
      
      rarch_softfilter_process(g_extern.filter.filter,
            g_extern.filter.buffer, g_extern.frame.pitch,
            data, width, height, pitch);

      g_extern.frame.data = g_extern.filter.buffer;
   }
   else 
#endif
   {
      g_extern.frame.data   = data;
      g_extern.frame.width  = width;
      g_extern.frame.height = height;
      g_extern.frame.pitch  = pitch;
   }
   
   if (!video_frame_func(g_extern.frame.data, g_extern.frame.width, 
                         g_extern.frame.height, g_extern.frame.pitch, msg))
      g_extern.video_active = false;
}

void rarch_render_cached_frame(void)
{
   video_frame(g_extern.frame_cache.data,
         g_extern.frame_cache.width,
         g_extern.frame_cache.height,
         g_extern.frame_cache.pitch);
}

static bool audio_flush(const int16_t *data, size_t samples)
{
   if (g_extern.is_paused || g_extern.audio_data.mute)
      return true;
   if (!g_extern.audio_active)
      return false;

   const float *output_data = NULL;
   unsigned output_frames      = 0;

   struct resampler_data src_data = {0};
   RARCH_PERFORMANCE_INIT(audio_convert_s16);
   RARCH_PERFORMANCE_START(audio_convert_s16);
   audio_convert_s16_to_float(g_extern.audio_data.data, data, samples,
         g_extern.audio_data.volume_gain);
   RARCH_PERFORMANCE_STOP(audio_convert_s16);

   src_data.data_in      = g_extern.audio_data.data;
   src_data.input_frames = samples >> 1;

   src_data.data_out = g_extern.audio_data.outsamples;

   if (g_extern.audio_data.rate_control)
      readjust_audio_input_rate();

   src_data.ratio = g_extern.audio_data.src_ratio;
   if (g_extern.is_slowmotion)
      src_data.ratio *= g_settings.slowmotion_ratio;

   RARCH_PERFORMANCE_INIT(resampler_proc);
   RARCH_PERFORMANCE_START(resampler_proc);
   rarch_resampler_process(g_extern.audio_data.resampler,
         g_extern.audio_data.resampler_data, &src_data);
   RARCH_PERFORMANCE_STOP(resampler_proc);

   output_data   = g_extern.audio_data.outsamples;
   output_frames = src_data.output_frames;

   if (g_extern.audio_data.use_float)
   {
      if (audio_write_func(output_data, output_frames * sizeof(float) * 2) < 0)
      {
         RARCH_ERR("Audio backend failed to write. Will continue without sound.\n");
         return false;
      }
   }
   else
   {
      RARCH_PERFORMANCE_INIT(audio_convert_float);
      RARCH_PERFORMANCE_START(audio_convert_float);
      audio_convert_float_to_s16(g_extern.audio_data.conv_outsamples,
            output_data, output_frames * 2);
      RARCH_PERFORMANCE_STOP(audio_convert_float);

      if (audio_write_func(g_extern.audio_data.conv_outsamples, output_frames * sizeof(int16_t) * 2) < 0)
      {
         RARCH_ERR("Audio backend failed to write. Will continue without sound.\n");
         return false;
      }
   }

   return true;
}

static void audio_sample_rewind(int16_t left, int16_t right)
{
   g_extern.audio_data.rewind_buf[--g_extern.audio_data.rewind_ptr] = right;
   g_extern.audio_data.rewind_buf[--g_extern.audio_data.rewind_ptr] = left;
}

size_t audio_sample_batch_rewind(const int16_t *data, size_t frames)
{
   size_t i, samples;

   samples = frames << 1;
   for (i = 0; i < samples; i++)
      g_extern.audio_data.rewind_buf[--g_extern.audio_data.rewind_ptr] = data[i];

   return frames;
}

static void audio_sample(int16_t left, int16_t right)
{
   g_extern.audio_data.conv_outsamples[g_extern.audio_data.data_ptr++] = left;
   g_extern.audio_data.conv_outsamples[g_extern.audio_data.data_ptr++] = right;

   if (g_extern.audio_data.data_ptr < g_extern.audio_data.chunk_size)
      return;

   g_extern.audio_active = audio_flush(g_extern.audio_data.conv_outsamples,
         g_extern.audio_data.data_ptr) && g_extern.audio_active;

   g_extern.audio_data.data_ptr = 0;
}

size_t audio_sample_batch(const int16_t *data, size_t frames)
{
   if (frames > (AUDIO_CHUNK_SIZE_NONBLOCKING >> 1))
      frames = AUDIO_CHUNK_SIZE_NONBLOCKING >> 1;

   g_extern.audio_active = audio_flush(data, frames << 1) && g_extern.audio_active;
   return frames;
}

void rarch_input_poll(void)
{
   input_poll_func();
}

// Turbo scheme: If turbo button is held, all buttons pressed except for D-pad will go into
// a turbo mode. Until the button is released again, the input state will be modulated by a periodic pulse defined
// by the configured duty cycle.
static bool input_apply_turbo(unsigned port, unsigned id, bool res)
{
   if (res && g_extern.turbo_frame_enable[port])
      g_extern.turbo_enable[port] |= (1 << id);
   else if (!res)
      g_extern.turbo_enable[port] &= ~(1 << id);

   if (g_extern.turbo_enable[port] & (1 << id))
      return res && ((g_extern.turbo_count % g_settings.input.turbo_period) < g_settings.input.turbo_duty_cycle);
   else
      return res;
}

static int16_t input_state(unsigned port, unsigned device, unsigned index, unsigned id)
{
   device &= RETRO_DEVICE_MASK;

   static const struct retro_keybind *binds[MAX_PLAYERS] = {
      g_settings.input.binds[0],
      g_settings.input.binds[1],
      g_settings.input.binds[2],
      g_settings.input.binds[3],
   };

   /* Change the port according to the mapping */
   port = g_settings.input.device_port[port];

   int16_t res = 0;
   if (id < RARCH_FIRST_META_KEY)
      res = input_input_state_func(binds, port, device, index, id);

   // Don't allow turbo for D-pad.
   if (device == RETRO_DEVICE_JOYPAD && (id < RETRO_DEVICE_ID_JOYPAD_UP || id > RETRO_DEVICE_ID_JOYPAD_RIGHT))
      res = input_apply_turbo(port, id, res);

   return res;
}

#ifndef GLOBAL_CONFIG_DIR
#define GLOBAL_CONFIG_DIR "/etc"
#endif
#define RARCH_DEFAULT_CONF_PATH_STR "\n\t\tBy default looks for config in $XDG_CONFIG_HOME/retroarch/retroarch.cfg,\n\t\t$HOME/.config/retroarch/retroarch.cfg,\n\t\tand $HOME/.retroarch.cfg.\n\t\tIf a default config is not found, RetroArch will attempt to create one based on the skeleton config (" GLOBAL_CONFIG_DIR "/retroarch.cfg)."

#include "config.features.h"

#define _PSUPP(var, name, desc) printf("\t%s:\n\t\t%s: %s\n", name, desc, _##var##_supp ? "yes" : "no")
static void print_features(void)
{
   puts("");
   puts("Features:");
   _PSUPP(zlib, "zlib", "PNG encode/decode and .zip extraction");
}
#undef _PSUPP

static void print_compiler(FILE *file)
{
   fprintf(file, "Compiler: ");
#if defined(__GNUC__)
   fprintf(file, "GCC (%d.%d.%d) %u-bit\n",
      __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__, (unsigned)(CHAR_BIT * sizeof(size_t)));
#else
   fprintf(file, "Unknown compiler %u-bit\n",
      (unsigned)(CHAR_BIT * sizeof(size_t)));
#endif
   fprintf(file, "Built: %s\n", __DATE__);
}

static void print_help(void)
{
   puts("===================================================================");
#ifdef HAVE_GIT_VERSION
   printf("RetroArch: Frontend for libretro -- v" PACKAGE_VERSION " -- %s --\n", rarch_git_version);
#else
   puts("RetroArch: Frontend for libretro -- v" PACKAGE_VERSION " --");
#endif
   print_compiler(stdout);
   puts("===================================================================");
   puts("Usage: retroarch [rom file] [options...]");
   puts("\t-h/--help: Show this help message.");
   puts("\t--menu: Do not require ROM or libretro core to be loaded, starts directly in menu.");
   puts("\t\tIf no arguments are passed to RetroArch, it is equivalent to using --menu as only argument.");
   puts("\t--features: Prints available features compiled into RetroArch.");
   puts("\t-s/--save: Path for save file (*.srm).");
   puts("\t-S/--savestate: Path to use for save states. If not selected, *.state will be assumed.");
   puts("\t-c/--config: Path for config file." RARCH_DEFAULT_CONF_PATH_STR);
   puts("\t--appendconfig: Extra config files are loaded in, and take priority over config selected in -c (or default).");
   puts("\t\tMultiple configs are delimited by ','.");
   puts("\t-g/--gameboy: Path to Gameboy ROM. Load SuperGameBoy as the regular rom.");
   puts("\t-b/--bsx: Path to BSX rom. Load BSX BIOS as the regular rom.");
   puts("\t-B/--bsxslot: Path to BSX slotted rom. Load BSX BIOS as the regular rom.");
   puts("\t--sufamiA: Path to A slot of Sufami Turbo. Load Sufami base cart as regular rom.");
   puts("\t--sufamiB: Path to B slot of Sufami Turbo.");

   printf("\t-N/--nodevice: Disconnects controller device connected to port (1 to %d).\n", MAX_PLAYERS);
   printf("\t-A/--dualanalog: Connect a DualAnalog controller to port (1 to %d).\n", MAX_PLAYERS);
   printf("\t-d/--device: Connect a generic device into port of the device (1 to %d).\n", MAX_PLAYERS);
   puts("\t\tFormat is port:ID, where ID is an unsigned number corresponding to the particular device.\n");

   puts("\t\t{no,}load-{no,}save describes if SRAM should be loaded, and if SRAM should be saved.");
   puts("\t\tDo note that noload-save implies that save files will be deleted and overwritten.");

   puts("\t-v/--verbose: Verbose logging.");
   puts("\t-D/--detach: Detach RetroArch from the running console. Not relevant for all platforms.\n");
}

static void set_basename(const char *path)
{
   strlcpy(g_extern.fullpath, path, sizeof(g_extern.fullpath));

   strlcpy(g_extern.basename, path, sizeof(g_extern.basename));
   char *dst = strrchr(g_extern.basename, '.');
   if (dst)
      *dst = '\0';
}

static void set_paths(const char *path)
{
   set_basename(path);

   if (!g_extern.has_set_save_path)
      fill_pathname_noext(g_extern.savefile_name_srm, g_extern.basename, ".srm", sizeof(g_extern.savefile_name_srm));
   if (!g_extern.has_set_state_path)
      fill_pathname_noext(g_extern.savestate_name, g_extern.basename, ".state", sizeof(g_extern.savestate_name));

   if (path_is_directory(g_extern.savefile_name_srm))
   {
      fill_pathname_dir(g_extern.savefile_name_srm, g_extern.basename, ".srm", sizeof(g_extern.savefile_name_srm));
      RARCH_LOG("Redirecting save file to \"%s\".\n", g_extern.savefile_name_srm);
   }
   if (path_is_directory(g_extern.savestate_name))
   {
      fill_pathname_dir(g_extern.savestate_name, g_extern.basename, ".state", sizeof(g_extern.savestate_name));
      RARCH_LOG("Redirecting save state to \"%s\".\n", g_extern.savestate_name);
   }

   // If this is already set,
   // do not overwrite it as this was initialized before in a menu or otherwise.
   if (!*g_settings.system_directory)
      fill_pathname_basedir(g_settings.system_directory, path, sizeof(g_settings.system_directory));
}

static void parse_input(int argc, char *argv[])
{
   g_extern.libretro_no_rom = false;
   g_extern.libretro_dummy = false;
   g_extern.has_set_save_path = false;
   g_extern.has_set_state_path = false;

   if (argc < 2)
   {
      g_extern.libretro_dummy = true;
      return;
   }

   // Make sure we can call parse_input several times ...
   optind = 0;

   int val = 0;

   const struct option opts[] = {
      { "menu", 0, &val, 'M' },
      { "help", 0, NULL, 'h' },
      { "save", 1, NULL, 's' },
      { "verbose", 0, NULL, 'v' },
      { "gameboy", 1, NULL, 'g' },
      { "config", 1, NULL, 'c' },
      { "appendconfig", 1, &val, 'C' },
      { "nodevice", 1, NULL, 'N' },
      { "dualanalog", 1, NULL, 'A' },
      { "device", 1, NULL, 'd' },
      { "savestate", 1, NULL, 'S' },
      { "bsx", 1, NULL, 'b' },
      { "bsxslot", 1, NULL, 'B' },
      { "sufamiA", 1, NULL, 'Y' },
      { "sufamiB", 1, NULL, 'Z' },
      { "detach", 0, NULL, 'D' },
      { "features", 0, &val, 'f' },
      { NULL, 0, NULL, 0 }
   };

   const char *optstring = "hs:fvS:A:g:b:c:B:Y:Z:U:DN:d:" ;

   for (;;)
   {
      val = 0;
      int c = getopt_long(argc, argv, optstring, opts, NULL);
      int port;

      if (c == -1)
         break;


      switch (c)
      {
         case 'h':
            print_help();
            exit(0);

         case 'd':
         {
            struct string_list *list = string_split(optarg, ":");
            port = (list && list->size == 2) ? strtol(list->elems[0].data, NULL, 0) : 0;
            unsigned id = (list && list->size == 2) ? strtoul(list->elems[1].data, NULL, 0) : 0;
            string_list_free(list);

            if (port < 1 || port > MAX_PLAYERS)
            {
               RARCH_ERR("Connect device to a valid port.\n");
               print_help();
               rarch_fail(1, "parse_input()");
            }
            g_settings.input.libretro_device[port - 1] = id;
            g_extern.has_set_libretro_device[port - 1] = true;
            break;
         }

         case 'A':
            port = strtol(optarg, NULL, 0);
            if (port < 1 || port > MAX_PLAYERS)
            {
               RARCH_ERR("Connect dualanalog to a valid port.\n");
               print_help();
               rarch_fail(1, "parse_input()");
            }
            g_settings.input.libretro_device[port - 1] = RETRO_DEVICE_ANALOG;
            g_extern.has_set_libretro_device[port - 1] = true;
            break;

         case 's':
            strlcpy(g_extern.savefile_name_srm, optarg, sizeof(g_extern.savefile_name_srm));
            g_extern.has_set_save_path = true;
            break;

         case 'g':
            strlcpy(g_extern.gb_rom_path, optarg, sizeof(g_extern.gb_rom_path));
            g_extern.game_type = RARCH_CART_SGB;
            break;

         case 'b':
            strlcpy(g_extern.bsx_rom_path, optarg, sizeof(g_extern.bsx_rom_path));
            g_extern.game_type = RARCH_CART_BSX;
            break;

         case 'B':
            strlcpy(g_extern.bsx_rom_path, optarg, sizeof(g_extern.bsx_rom_path));
            g_extern.game_type = RARCH_CART_BSX_SLOTTED;
            break;

         case 'Y':
            strlcpy(g_extern.sufami_rom_path[0], optarg, sizeof(g_extern.sufami_rom_path[0]));
            g_extern.game_type = RARCH_CART_SUFAMI;
            break;

         case 'Z':
            strlcpy(g_extern.sufami_rom_path[1], optarg, sizeof(g_extern.sufami_rom_path[1]));
            g_extern.game_type = RARCH_CART_SUFAMI;
            break;

         case 'S':
            strlcpy(g_extern.savestate_name, optarg, sizeof(g_extern.savestate_name));
            g_extern.has_set_state_path = true;
            break;

         case 'v':
            g_extern.verbose = true;
            break;

         case 'N':
            port = strtol(optarg, NULL, 0);
            if (port < 1 || port > MAX_PLAYERS)
            {
               RARCH_ERR("Disconnect device from a valid port.\n");
               print_help();
               rarch_fail(1, "parse_input()");
            }
            g_settings.input.libretro_device[port - 1] = RETRO_DEVICE_NONE;
            g_extern.has_set_libretro_device[port - 1] = true;
            break;

         case 'c':
            strlcpy(g_extern.config_path, optarg, sizeof(g_extern.config_path));
            break;

         case 'D':
            break;

         case 0:
            switch (val)
            {
               case 'M':
                  g_extern.libretro_dummy = true;
                  break;

               case 'f':
                  print_features();
                  exit(0);

               default:
                  break;
            }
            break;

         case '?':
            print_help();
            rarch_fail(1, "parse_input()");

         default:
            RARCH_ERR("Error parsing arguments.\n");
            rarch_fail(1, "parse_input()");
      }
   }

   if (g_extern.libretro_dummy)
   {
      if (optind < argc)
      {
         RARCH_ERR("--menu was used, but ROM file was passed as well.\n");
         rarch_fail(1, "parse_input()");
      }
   }
   else if (optind < argc)
      set_paths(argv[optind]);
   else
      g_extern.libretro_no_rom = true;

   // Copy SRM/state dirs used, so they can be reused on reentrancy.
   if (g_extern.has_set_save_path && path_is_directory(g_extern.savefile_name_srm))
      strlcpy(g_extern.savefile_dir, g_extern.savefile_name_srm, sizeof(g_extern.savefile_dir));
   if (g_extern.has_set_state_path && path_is_directory(g_extern.savestate_name))
      strlcpy(g_extern.savestate_dir, g_extern.savestate_name, sizeof(g_extern.savestate_dir));
}

static void init_controllers(void)
{
   unsigned i;
   for (i = 0; i < MAX_PLAYERS; i++)
   {
      unsigned device = g_settings.input.libretro_device[i];

      // This is default, don't bother.
      if (device == RETRO_DEVICE_JOYPAD)
         continue;

      pretro_set_controller_port_device(i, device);

      const struct retro_controller_description *desc = NULL;
      if (i < g_extern.system.num_ports)
         desc = libretro_find_controller_description(&g_extern.system.ports[i], device);

      const char *ident = desc ? desc->desc : NULL;

      if (!ident)
      {
         switch (device)
         {
            case RETRO_DEVICE_ANALOG: ident = "analog"; break;
            default: ident = "Unknown"; break;
         }
      }

      if (device == RETRO_DEVICE_NONE)
         RARCH_LOG("Disconnecting device from port %u.\n", i + 1);
      else
         RARCH_LOG("Connecting %s (ID: %u) to port %u.\n", ident, device, i + 1);
   }
}

static inline void load_save_files(void)
{
   switch (g_extern.game_type)
   {
      case RARCH_CART_NORMAL:
         load_ram_file(g_extern.savefile_name_srm, RETRO_MEMORY_SAVE_RAM);
         load_ram_file(g_extern.savefile_name_rtc, RETRO_MEMORY_RTC);
         break;

      case RARCH_CART_SGB:
         load_ram_file(g_extern.savefile_name_srm, RETRO_MEMORY_SNES_GAME_BOY_RAM);
         load_ram_file(g_extern.savefile_name_rtc, RETRO_MEMORY_SNES_GAME_BOY_RTC);
         break;

      case RARCH_CART_BSX:
      case RARCH_CART_BSX_SLOTTED:
         load_ram_file(g_extern.savefile_name_srm, RETRO_MEMORY_SNES_BSX_RAM);
         load_ram_file(g_extern.savefile_name_psrm, RETRO_MEMORY_SNES_BSX_PRAM);
         break;

      case RARCH_CART_SUFAMI:
         load_ram_file(g_extern.savefile_name_asrm, RETRO_MEMORY_SNES_SUFAMI_TURBO_A_RAM);
         load_ram_file(g_extern.savefile_name_bsrm, RETRO_MEMORY_SNES_SUFAMI_TURBO_B_RAM);
         break;

      default:
         break;
   }
}

static inline void save_files(void)
{
   switch (g_extern.game_type)
   {
      case RARCH_CART_NORMAL:
         RARCH_LOG("Saving regular SRAM.\n");
         RARCH_LOG("SRM: %s\n", g_extern.savefile_name_srm);
         RARCH_LOG("RTC: %s\n", g_extern.savefile_name_rtc);
         save_ram_file(g_extern.savefile_name_srm, RETRO_MEMORY_SAVE_RAM);
         save_ram_file(g_extern.savefile_name_rtc, RETRO_MEMORY_RTC);
         break;

      case RARCH_CART_SGB:
         RARCH_LOG("Saving Gameboy SRAM.\n");
         RARCH_LOG("SRM: %s\n", g_extern.savefile_name_srm);
         RARCH_LOG("RTC: %s\n", g_extern.savefile_name_rtc);
         save_ram_file(g_extern.savefile_name_srm, RETRO_MEMORY_SNES_GAME_BOY_RAM);
         save_ram_file(g_extern.savefile_name_rtc, RETRO_MEMORY_SNES_GAME_BOY_RTC);
         break;

      case RARCH_CART_BSX:
      case RARCH_CART_BSX_SLOTTED:
         RARCH_LOG("Saving BSX (P)RAM.\n");
         RARCH_LOG("SRM:  %s\n", g_extern.savefile_name_srm);
         RARCH_LOG("PSRM: %s\n", g_extern.savefile_name_psrm);
         save_ram_file(g_extern.savefile_name_srm, RETRO_MEMORY_SNES_BSX_RAM);
         save_ram_file(g_extern.savefile_name_psrm, RETRO_MEMORY_SNES_BSX_PRAM);
         break;

      case RARCH_CART_SUFAMI:
         RARCH_LOG("Saving Sufami turbo A/B RAM.\n");
         RARCH_LOG("ASRM: %s\n", g_extern.savefile_name_asrm);
         RARCH_LOG("BSRM: %s\n", g_extern.savefile_name_bsrm);
         save_ram_file(g_extern.savefile_name_asrm, RETRO_MEMORY_SNES_SUFAMI_TURBO_A_RAM);
         save_ram_file(g_extern.savefile_name_bsrm, RETRO_MEMORY_SNES_SUFAMI_TURBO_B_RAM);
         break;

      default:
         break;
   }
}

void rarch_init_msg_queue(void)
{
   if (g_extern.msg_queue)
      return;

   rarch_assert(g_extern.msg_queue = msg_queue_new(8));
}

void rarch_deinit_msg_queue(void)
{
   if (g_extern.msg_queue)
   {
      msg_queue_free(g_extern.msg_queue);
      g_extern.msg_queue = NULL;
   }
}

void rarch_init_rewind(void)
{
   if (!g_settings.rewind_enable || g_extern.state_manager)
      return;

   g_extern.state_size = pretro_serialize_size();
   if (!g_extern.state_size)
   {
      RARCH_ERR("Implementation does not support save states. Cannot use rewind.\n");
      return;
   }

   RARCH_LOG("Initing rewind buffer with size: %u MB\n", (unsigned)(g_settings.rewind_buffer_size / 1000000));
   g_extern.state_manager = state_manager_new(g_extern.state_size, g_settings.rewind_buffer_size);

   if (!g_extern.state_manager)
      RARCH_WARN("Failed to init rewind buffer. Rewinding will be disabled.\n");

   void *state;
   state_manager_push_where(g_extern.state_manager, &state);
   pretro_serialize(state, g_extern.state_size);
   state_manager_push_do(g_extern.state_manager);
}

void rarch_deinit_rewind(void)
{
   if (g_extern.state_manager)
      state_manager_free(g_extern.state_manager);
   g_extern.state_manager = NULL;
}

static void init_libretro_cbs_plain(void)
{
   pretro_set_video_refresh(video_frame);
   pretro_set_audio_sample(audio_sample);
   pretro_set_audio_sample_batch(audio_sample_batch);
   pretro_set_input_state(input_state);
   pretro_set_input_poll(rarch_input_poll);
}

static void init_libretro_cbs(void)
{
   init_libretro_cbs_plain();
}

static void set_savestate_auto_index(void)
{
   if (!g_settings.savestate_auto_index)
      return;

   // Find the file in the same directory as g_extern.savestate_name with the largest numeral suffix.
   // E.g. /foo/path/game.state, will try to find /foo/path/game.state%d, where %d is the largest number available.

   char state_dir[PATH_MAX];
   char state_base[PATH_MAX];

   fill_pathname_basedir(state_dir, g_extern.savestate_name, sizeof(state_dir));
   fill_pathname_base(state_base, g_extern.savestate_name, sizeof(state_base));

   unsigned max_index = 0;

   struct string_list *dir_list = dir_list_new(state_dir, NULL, false);
   if (!dir_list)
      return;

   size_t i;
   for (i = 0; i < dir_list->size; i++)
   {
      const char *dir_elem = dir_list->elems[i].data;

      char elem_base[PATH_MAX];
      fill_pathname_base(elem_base, dir_elem, sizeof(elem_base));

      if (strstr(elem_base, state_base) != elem_base)
         continue;

      const char *end = dir_elem + strlen(dir_elem);
      while ((end > dir_elem) && isdigit(end[-1])) end--;

      unsigned index = strtoul(end, NULL, 0);
      if (index > max_index)
         max_index = index;
   }

   dir_list_free(dir_list);

   g_extern.state_slot = max_index;
   RARCH_LOG("Found last state slot: #%u\n", g_extern.state_slot);
}

static void fill_pathnames(void)
{
   switch (g_extern.game_type)
   {
      case RARCH_CART_BSX:
      case RARCH_CART_BSX_SLOTTED:
         // BSX PSRM
         if (!g_extern.has_set_save_path)
         {
            fill_pathname(g_extern.savefile_name_srm,
                  g_extern.bsx_rom_path, ".srm", sizeof(g_extern.savefile_name_srm));
         }

         fill_pathname(g_extern.savefile_name_psrm,
               g_extern.savefile_name_srm, ".psrm", sizeof(g_extern.savefile_name_psrm));

         if (!g_extern.has_set_state_path)
         {
            fill_pathname(g_extern.savestate_name,
                  g_extern.bsx_rom_path, ".state", sizeof(g_extern.savestate_name));
         }
         break;

      case RARCH_CART_SUFAMI:
         if (g_extern.has_set_save_path && *g_extern.sufami_rom_path[0] && *g_extern.sufami_rom_path[1])
            RARCH_WARN("Sufami Turbo SRAM paths will be inferred from their respective paths to avoid conflicts.\n");

         // SUFAMI ARAM
         fill_pathname(g_extern.savefile_name_asrm,
               g_extern.sufami_rom_path[0], ".srm", sizeof(g_extern.savefile_name_asrm));

         // SUFAMI BRAM
         fill_pathname(g_extern.savefile_name_bsrm,
               g_extern.sufami_rom_path[1], ".srm", sizeof(g_extern.savefile_name_bsrm));

         if (!g_extern.has_set_state_path)
         {
            fill_pathname(g_extern.savestate_name,
                  *g_extern.sufami_rom_path[0] ?
                     g_extern.sufami_rom_path[0] : g_extern.sufami_rom_path[1],
                     ".state", sizeof(g_extern.savestate_name));
         }
         break;

      case RARCH_CART_SGB:
         if (!g_extern.has_set_save_path)
         {
            fill_pathname(g_extern.savefile_name_srm,
                  g_extern.gb_rom_path, ".srm", sizeof(g_extern.savefile_name_srm));
         }

         if (!g_extern.has_set_state_path)
         {
            fill_pathname(g_extern.savestate_name,
                  g_extern.gb_rom_path, ".state", sizeof(g_extern.savestate_name));
         }

         fill_pathname(g_extern.savefile_name_rtc,
               g_extern.savefile_name_srm, ".rtc", sizeof(g_extern.savefile_name_rtc));
         break;

      default:
         // Infer .rtc save path from save ram path.
         fill_pathname(g_extern.savefile_name_rtc,
               g_extern.savefile_name_srm, ".rtc", sizeof(g_extern.savefile_name_rtc));
   }
}

static void load_auto_state(void)
{
   if (!g_settings.savestate_auto_load)
      return;

   char savestate_name_auto[PATH_MAX];
   fill_pathname_noext(savestate_name_auto, g_extern.savestate_name,
         ".auto", sizeof(savestate_name_auto));

   if (path_file_exists(savestate_name_auto))
   {
      RARCH_LOG("Found auto savestate in: %s\n", savestate_name_auto);
      bool ret = load_state(savestate_name_auto);

      char msg[PATH_MAX];
      snprintf(msg, sizeof(msg), "Auto-loading savestate from \"%s\" %s.", savestate_name_auto, ret ? "succeeded" : "failed");
      msg_queue_push(g_extern.msg_queue, msg, 1, 180);
      RARCH_LOG("%s\n", msg);
   }
}

static void save_auto_state(void)
{
   if (!g_settings.savestate_auto_save)
      return;

   char savestate_name_auto[PATH_MAX];
   fill_pathname_noext(savestate_name_auto, g_extern.savestate_name,
         ".auto", sizeof(savestate_name_auto));

   bool ret = save_state(savestate_name_auto);
   
   (void)ret;
   
   RARCH_LOG("Auto save state to \"%s\" %s.\n", savestate_name_auto, ret ? "succeeded" : "failed");
}

void rarch_load_state(void)
{
   char load_path[PATH_MAX];

   if (g_extern.state_slot > 0)
      snprintf(load_path, sizeof(load_path), "%s%d", g_extern.savestate_name, g_extern.state_slot);
   else if (g_extern.state_slot < 0)
      snprintf(load_path, sizeof(load_path), "%s.auto", g_extern.savestate_name);
   else
      snprintf(load_path, sizeof(load_path), "%s", g_extern.savestate_name);

   size_t size = pretro_serialize_size();
   char msg[512];

   if (size)
   {
      if (load_state(load_path))
      {
         if (g_extern.state_slot < 0)
            snprintf(msg, sizeof(msg), "Loaded state from slot #-1 (auto).");
         else
            snprintf(msg, sizeof(msg), "Loaded state from slot #%d.", g_extern.state_slot);
      }
      else
         snprintf(msg, sizeof(msg), "Failed to load state from \"%s\".", load_path);
   }
   else
      strlcpy(msg, "Core does not support save states.", sizeof(msg));

   msg_queue_clear(g_extern.msg_queue);
   msg_queue_push(g_extern.msg_queue, msg, 2, 180);
   RARCH_LOG("%s\n", msg);
}

void rarch_save_state(void)
{
   if (g_settings.savestate_auto_index)
      g_extern.state_slot++;

   char save_path[PATH_MAX];

   if (g_extern.state_slot > 0)
      snprintf(save_path, sizeof(save_path), "%s%d", g_extern.savestate_name, g_extern.state_slot);
   else if (g_extern.state_slot < 0)
      snprintf(save_path, sizeof(save_path), "%s.auto", g_extern.savestate_name);
   else
      snprintf(save_path, sizeof(save_path), "%s", g_extern.savestate_name);

   size_t size = pretro_serialize_size();
   char msg[512];

   if (size)
   {
      if (save_state(save_path))
      {
         if (g_extern.state_slot < 0)
            snprintf(msg, sizeof(msg), "Saved state to slot #-1 (auto).");
         else
            snprintf(msg, sizeof(msg), "Saved state to slot #%u.", g_extern.state_slot);
      }
      else
         snprintf(msg, sizeof(msg), "Failed to save state to \"%s\".", save_path);
   }
   else
      strlcpy(msg, "Core does not support save states.", sizeof(msg));

   msg_queue_clear(g_extern.msg_queue);
   msg_queue_push(g_extern.msg_queue, msg, 2, 180);
   RARCH_LOG("%s\n", msg);
}

// Save or load state here.
static void check_savestates(bool immutable)
{
   static bool old_should_savestate = false;
   bool should_savestate = input_key_pressed_func(RARCH_SAVE_STATE_KEY);

   if (should_savestate && !old_should_savestate)
      rarch_save_state();
   old_should_savestate = should_savestate;

   if (!immutable)
   {
      static bool old_should_loadstate = false;
      bool should_loadstate = input_key_pressed_func(RARCH_LOAD_STATE_KEY);

      if (!should_savestate && should_loadstate && !old_should_loadstate)
         rarch_load_state();
      old_should_loadstate = should_loadstate;
   }
}

void rarch_reset_drivers(void)
{
   uninit_drivers();
   init_drivers();

   // Poll input to avoid possibly stale data to corrupt things.
   if (driver.input)
      input_poll_func();
}

void rarch_state_slot_increase(void)
{
   g_extern.state_slot++;

   if (g_extern.msg_queue)
      msg_queue_clear(g_extern.msg_queue);
   char msg[256];

   snprintf(msg, sizeof(msg), "State slot: %u", g_extern.state_slot);

   if (g_extern.msg_queue)
      msg_queue_push(g_extern.msg_queue, msg, 1, 180);

   RARCH_LOG("%s\n", msg);
}

void rarch_state_slot_decrease(void)
{
   if (g_extern.state_slot > 0)
      g_extern.state_slot--;

   if (g_extern.msg_queue)
      msg_queue_clear(g_extern.msg_queue);

   char msg[256];

   snprintf(msg, sizeof(msg), "State slot: %u", g_extern.state_slot);

   if (g_extern.msg_queue)
      msg_queue_push(g_extern.msg_queue, msg, 1, 180);

   RARCH_LOG("%s\n", msg);
}

static void check_stateslots(void)
{
   // Save state slots
   static bool old_should_slot_increase = false;
   bool should_slot_increase = input_key_pressed_func(RARCH_STATE_SLOT_PLUS);
   if (should_slot_increase && !old_should_slot_increase)
      rarch_state_slot_increase();
   old_should_slot_increase = should_slot_increase;

   static bool old_should_slot_decrease = false;
   bool should_slot_decrease = input_key_pressed_func(RARCH_STATE_SLOT_MINUS);
   if (should_slot_decrease && !old_should_slot_decrease)
      rarch_state_slot_decrease();
   old_should_slot_decrease = should_slot_decrease;
}

static inline void flush_rewind_audio(void)
{
   if (g_extern.frame_is_reverse) // We just rewound. Flush rewind audio buffer.
   {
      g_extern.audio_active = audio_flush(g_extern.audio_data.rewind_buf + g_extern.audio_data.rewind_ptr,
            g_extern.audio_data.rewind_size - g_extern.audio_data.rewind_ptr) && g_extern.audio_active;
   }
}

static inline void setup_rewind_audio(void)
{
   unsigned i;
   // Push audio ready to be played.
   g_extern.audio_data.rewind_ptr = g_extern.audio_data.rewind_size;
   for (i = 0; i < g_extern.audio_data.data_ptr; i += 2)
   {
      g_extern.audio_data.rewind_buf[--g_extern.audio_data.rewind_ptr] =
         g_extern.audio_data.conv_outsamples[i + 1];

      g_extern.audio_data.rewind_buf[--g_extern.audio_data.rewind_ptr] =
         g_extern.audio_data.conv_outsamples[i + 0];
   }

   g_extern.audio_data.data_ptr = 0;
}

static void check_rewind(void)
{
   flush_rewind_audio();
   g_extern.frame_is_reverse = false;

   static bool first = true;
   if (first)
   {
      first = false;
      return;
   }

   if (!g_extern.state_manager)
      return;

   if (input_key_pressed_func(RARCH_REWIND))
   {
      msg_queue_clear(g_extern.msg_queue);
      const void *buf;
      if (state_manager_pop(g_extern.state_manager, &buf))
      {
         g_extern.frame_is_reverse = true;
         setup_rewind_audio();

         msg_queue_push(g_extern.msg_queue, "Rewinding.", 0, g_extern.is_paused ? 1 : 30);
         pretro_unserialize(buf, g_extern.state_size);
      }
      else
         msg_queue_push(g_extern.msg_queue, "Reached end of rewind buffer.", 0, 30);
   }
   else
   {
      static unsigned cnt = 0;
      cnt = (cnt + 1) % (g_settings.rewind_granularity ? g_settings.rewind_granularity : 1); // Avoid possible SIGFPE.

      if (cnt == 0)
      {
         void *state;
         state_manager_push_where(g_extern.state_manager, &state);

         RARCH_PERFORMANCE_INIT(rewind_serialize);
         RARCH_PERFORMANCE_START(rewind_serialize);
         pretro_serialize(state, g_extern.state_size);
         RARCH_PERFORMANCE_STOP(rewind_serialize);

         state_manager_push_do(g_extern.state_manager);
      }
   }

   pretro_set_audio_sample(g_extern.frame_is_reverse ?
         audio_sample_rewind : audio_sample);
   pretro_set_audio_sample_batch(g_extern.frame_is_reverse ?
         audio_sample_batch_rewind : audio_sample_batch);
}

static void check_slowmotion(void)
{
   g_extern.is_slowmotion = input_key_pressed_func(RARCH_SLOWMOTION);
   if (g_extern.is_slowmotion)
   {
      msg_queue_clear(g_extern.msg_queue);
      msg_queue_push(g_extern.msg_queue, g_extern.frame_is_reverse ? "Slow motion rewind." : "Slow motion.", 0, 30);
   }
}

static void check_pause(void)
{
   static bool old_state = false;
   bool new_state = input_key_pressed_func(RARCH_PAUSE_TOGGLE);

   // FRAMEADVANCE will set us into pause mode.
   new_state |= !g_extern.is_paused && input_key_pressed_func(RARCH_FRAMEADVANCE);

   if (new_state && !old_state)
   {
      g_extern.is_paused = !g_extern.is_paused;

      if (g_extern.is_paused)
      {
         RARCH_LOG("Paused.\n");
         if (driver.audio_data)
            audio_stop_func();
      }
      else
      {
         RARCH_LOG("Unpaused.\n");
         if (driver.audio_data)
         {
            if (!g_extern.audio_data.mute && !audio_start_func())
            {
               RARCH_ERR("Failed to resume audio driver. Will continue without audio.\n");
               g_extern.audio_active = false;
            }
         }
      }
   }
   
   old_state = new_state;
}

static void check_oneshot(void)
{
   static bool old_state = false;
   bool new_state = input_key_pressed_func(RARCH_FRAMEADVANCE);
   g_extern.is_oneshot = (new_state && !old_state);
   old_state = new_state;

   // Rewind buttons works like FRAMEREWIND when paused. We will one-shot in that case.
   static bool old_rewind_state = false;
   bool new_rewind_state = input_key_pressed_func(RARCH_REWIND);
   g_extern.is_oneshot |= new_rewind_state && !old_rewind_state;
   old_rewind_state = new_rewind_state;
}

void rarch_game_reset(void)
{
   RARCH_LOG("Resetting game.\n");
   msg_queue_clear(g_extern.msg_queue);
   msg_queue_push(g_extern.msg_queue, "Reset.", 1, 120);
   pretro_reset();
   init_controllers(); // bSNES since v073r01 resets controllers to JOYPAD after a reset, so just enforce it here.
}

static void check_reset(void)
{
   static bool old_state = false;
   bool new_state = input_key_pressed_func(RARCH_RESET);
   if (new_state && !old_state)
      rarch_game_reset();

   old_state = new_state;
}

static void check_turbo(void)
{
   unsigned i;

   g_extern.turbo_count++;

   static const struct retro_keybind *binds[MAX_PLAYERS] = {
      g_settings.input.binds[0],
      g_settings.input.binds[1],
      g_settings.input.binds[2],
      g_settings.input.binds[3],
   };

   for (i = 0; i < MAX_PLAYERS; i++)
      g_extern.turbo_frame_enable[i] =
         input_input_state_func(binds, i, RETRO_DEVICE_JOYPAD, 0, RARCH_TURBO_ENABLE);
}

void rarch_disk_control_append_image(const char *path)
{
   const struct retro_disk_control_callback *control = &g_extern.system.disk_control;
   rarch_disk_control_set_eject(true, false);

   control->add_image_index();
   unsigned new_index = control->get_num_images();
   if (!new_index)
      return;
   new_index--;

   struct retro_game_info info = {0};
   info.path = path;
   control->replace_image_index(new_index, &info);

   rarch_disk_control_set_index(new_index);

   char msg[512];
   snprintf(msg, sizeof(msg), "Appended disk: %s", path);
   RARCH_LOG("%s\n", msg);
   msg_queue_clear(g_extern.msg_queue);
   msg_queue_push(g_extern.msg_queue, msg, 0, 180);

   // Update paths for our new image.
   // If we actually use append_image,
   // we assume that we started out in a single disk case,
   // and that this way of doing it makes the most sense.
   set_paths(path);
   fill_pathnames();

   rarch_disk_control_set_eject(false, false);
}

void rarch_disk_control_set_eject(bool new_state, bool log)
{
   const struct retro_disk_control_callback *control = &g_extern.system.disk_control;
   if (!control->get_num_images)
      return;

   bool error = false;
   char msg[256];
   *msg = '\0';

   if (control->set_eject_state(new_state))
      snprintf(msg, sizeof(msg), "%s virtual disk tray.", new_state ? "Ejected" : "Closed");
   else
   {
      error = true;
      snprintf(msg, sizeof(msg), "Failed to %s virtual disk tray.", new_state ? "eject" : "close");
   }

   if (*msg)
   {
      if (error)
         RARCH_ERR("%s\n", msg);
      else
         RARCH_LOG("%s\n", msg);

      // Only noise in RGUI.
      if (log)
      {
         msg_queue_clear(g_extern.msg_queue);
         msg_queue_push(g_extern.msg_queue, msg, 1, 180);
      }
   }
}

void rarch_disk_control_set_index(unsigned next_index)
{
   const struct retro_disk_control_callback *control = &g_extern.system.disk_control;
   if (!control->get_num_images)
      return;

   bool error = false;
   char msg[256];
   *msg = '\0';

   unsigned num_disks = control->get_num_images();
   if (control->set_image_index(next_index))
   {
      if (next_index < num_disks)
         snprintf(msg, sizeof(msg), "Setting disk %u of %u in tray.", next_index + 1, num_disks);
      else
         strlcpy(msg, "Removed disk from tray.", sizeof(msg));
   }
   else
   {
      if (next_index < num_disks)
         snprintf(msg, sizeof(msg), "Failed to set disk %u of %u.", next_index + 1, num_disks);
      else
         strlcpy(msg, "Failed to remove disk from tray.", sizeof(msg));
      error = true;
   }

   if (*msg)
   {
      if (error)
         RARCH_ERR("%s\n", msg);
      else
         RARCH_LOG("%s\n", msg);
      msg_queue_clear(g_extern.msg_queue);
      msg_queue_push(g_extern.msg_queue, msg, 1, 180);
   }
}

static void check_disk(void)
{
   const struct retro_disk_control_callback *control = &g_extern.system.disk_control;
   if (!control->get_num_images)
      return;

   static bool old_pressed_eject;
   static bool old_pressed_next;

   bool pressed_eject = input_key_pressed_func(RARCH_DISK_EJECT_TOGGLE);
   bool pressed_next  = input_key_pressed_func(RARCH_DISK_NEXT);

   if (pressed_eject && !old_pressed_eject)
   {
      bool new_state = !control->get_eject_state();
      rarch_disk_control_set_eject(new_state, true);
   }
   else if (pressed_next && !old_pressed_next)
   {
      unsigned num_disks = control->get_num_images();
      unsigned current   = control->get_image_index();
      if (num_disks && num_disks != UINT_MAX)
      {
         // Use "no disk" state when index == num_disks.
         unsigned next_index = current >= num_disks ? 0 : ((current + 1) % (num_disks + 1));
         rarch_disk_control_set_index(next_index);
      }
      else
         RARCH_ERR("Got invalid disk index from libretro.\n");
   }

   old_pressed_eject = pressed_eject;
   old_pressed_next  = pressed_next;
}

#if defined(HAVE_SCREENSHOTS)
static void check_screenshot(void)
{
   static bool old_pressed;
   bool pressed = input_key_pressed_func(RARCH_SCREENSHOT);
   if (pressed && !old_pressed)
      rarch_take_screenshot();

   old_pressed = pressed;
}
#endif

static void check_quick_swap(void)
{
   static bool old_pressed;
   
   if (g_settings.input.quick_swap_players < 2 ||
         g_settings.input.quick_swap_players > MAX_PLAYERS)
      return;

   bool pressed = input_key_pressed_func(RARCH_QUICK_SWAP);
   if (pressed && !old_pressed) quick_swap_controllers();

   old_pressed = pressed;
}

static void check_mute(void)
{
   if (!g_extern.audio_active)
      return;

   static bool old_pressed;
   bool pressed = input_key_pressed_func(RARCH_MUTE);
   if (pressed && !old_pressed)
   {
      g_extern.audio_data.mute = !g_extern.audio_data.mute;

      const char *msg = g_extern.audio_data.mute ? "Audio muted." : "Audio unmuted.";
      msg_queue_clear(g_extern.msg_queue);
      msg_queue_push(g_extern.msg_queue, msg, 1, 180);

      if (driver.audio_data)
      {
         if (g_extern.audio_data.mute)
            audio_stop_func();
         else if (!audio_start_func())
         {
            RARCH_ERR("Failed to unmute audio.\n");
            g_extern.audio_active = false;
         }
      }

      RARCH_LOG("%s\n", msg);
   }

   old_pressed = pressed;
}

static void check_volume(void)
{
   if (!g_extern.audio_active)
      return;

   float db_change   = 0.0f;
   bool pressed_up   = input_key_pressed_func(RARCH_VOLUME_UP);
   bool pressed_down = input_key_pressed_func(RARCH_VOLUME_DOWN);
   if (!pressed_up && !pressed_down)
      return;

   if (pressed_up)
      db_change += 0.5f;
   if (pressed_down)
      db_change -= 0.5f;

   g_extern.audio_data.volume_db += db_change;
   g_extern.audio_data.volume_db = max(g_extern.audio_data.volume_db, -80.0f);
   g_extern.audio_data.volume_db = min(g_extern.audio_data.volume_db, 12.0f);

   char msg[256];
   snprintf(msg, sizeof(msg), "Volume: %.1f dB", g_extern.audio_data.volume_db);
   msg_queue_clear(g_extern.msg_queue);
   msg_queue_push(g_extern.msg_queue, msg, 1, 180);
   RARCH_LOG("%s\n", msg);

   g_extern.audio_data.volume_gain = db_to_gain(g_extern.audio_data.volume_db);
}

void rarch_check_block_hotkey(void)
{
   driver.block_hotkey = driver.block_input;

   // If we haven't bound anything to this,
   // always allow hotkeys.
   static const struct retro_keybind *bind = &g_settings.input.binds[0][RARCH_ENABLE_HOTKEY];
   if (!driver.block_hotkey && bind->joykey == NO_BTN && bind->joyaxis == AXIS_NONE)
      return;

   driver.block_hotkey = driver.block_input || !input_key_pressed_func(RARCH_ENABLE_HOTKEY);
}

static void do_state_checks(void)
{
   rarch_check_block_hotkey();

#if defined(HAVE_SCREENSHOTS)
   check_screenshot();
#endif
   check_mute();
   check_volume();
   check_turbo();
   check_pause();
   check_oneshot();

   if ( g_extern.is_paused)
      rarch_render_cached_frame();

   if (g_extern.is_paused && !g_extern.is_oneshot)
      return;

   check_fast_forward_button();
   check_stateslots();
   check_savestates(false);
   check_rewind();
   check_slowmotion();
   check_disk();
   check_quick_swap();
   check_reset();
}

static void init_state(void)
{
   g_extern.video_active = true;
   g_extern.audio_active = true;
   g_extern.game_type = RARCH_CART_NORMAL;
}

static void init_state_first(void)
{
   unsigned i;

   init_state();
   
#ifdef HAVE_FILE_LOGGER
   g_extern.log_file = fopen(LOG_FILENAME, "a");
#endif

   /* Default pixel format */
   g_extern.system.pix_fmt = RETRO_PIXEL_FORMAT_RGB565;

   for (i = 0; i < MAX_PLAYERS; i++)
      g_settings.input.libretro_device[i] = RETRO_DEVICE_JOYPAD;
}

void rarch_main_clear_state(void)
{
   memset(&g_settings, 0, sizeof(g_settings));

#ifdef HAVE_FILE_LOGGER
   if (g_extern.log_file)
      fclose(g_extern.log_file);
#endif

   memset(&g_extern, 0, sizeof(g_extern));

   init_state_first();
}

#ifdef HAVE_ZLIB
#define DEFAULT_EXT "ZIP|zip"
#else
#define DEFAULT_EXT ""
#endif

void rarch_init_system_info(void)
{
   struct retro_system_info *info = &g_extern.system.info;
   pretro_get_system_info(info);

   if (!info->library_name)
      info->library_name = "Unknown";
   if (!info->library_version)
      info->library_version = "v0";

   snprintf(g_extern.title_buf, sizeof(g_extern.title_buf), "%s %s",
         info->library_name, info->library_version);

   strlcpy(g_extern.system.valid_extensions, info->valid_extensions ? info->valid_extensions : DEFAULT_EXT,
         sizeof(g_extern.system.valid_extensions));
   g_extern.system.block_extract = info->block_extract;
}

static void init_system_av_info(void)
{
   pretro_get_system_av_info(&g_extern.system.av_info);
   g_extern.frame_limit.last_frame_time = rarch_get_time_usec();
   g_extern.frame_limit.minimum_frame_time = (retro_time_t)roundf(1000000.0f / (g_extern.system.av_info.timing.fps * g_settings.fastforward_ratio));
}

static void verify_api_version(void)
{
   RARCH_LOG("Version of libretro API: %u\n", pretro_api_version());
   RARCH_LOG("Compiled against API: %u\n", RETRO_API_VERSION);
   if (pretro_api_version() != RETRO_API_VERSION)
      RARCH_WARN("RetroArch is compiled against a different version of libretro than this libretro implementation.\n");
}

// Make sure we haven't compiled for something we cannot run.
// Ideally, code would get swapped out depending on CPU support, but this will do for now.
static void validate_cpu_features(void)
{
   uint64_t cpu = rarch_get_cpu_features();
   (void)cpu;

#define FAIL_CPU(simd_type) do { \
   RARCH_ERR(simd_type " code is compiled in, but CPU does not support this feature. Cannot continue.\n"); \
   rarch_fail(1, "validate_cpu_features()"); \
} while(0)
}

int rarch_main_init(int argc, char *argv[])
{
   init_state();

   int sjlj_ret;
   if ((sjlj_ret = setjmp(g_extern.error_sjlj_context)) > 0)
   {
      RARCH_ERR("Fatal error received in: \"%s\"\n", g_extern.error_string);
      return sjlj_ret;
   }
   parse_input(argc, argv);

   if (g_extern.verbose)
   {
      RARCH_LOG_OUTPUT("=== Build =======================================\n");
      RARCH_LOG_OUTPUT("Version: %s\n", PACKAGE_VERSION);
      print_compiler(LOG_FILE);
#ifdef HAVE_GIT_VERSION
      RARCH_LOG_OUTPUT("Git: %s\n", rarch_git_version);
#endif
      RARCH_LOG_OUTPUT("=================================================\n");
   }

   validate_cpu_features();
   config_load();
   init_libretro_sym(g_extern.libretro_dummy);
   rarch_init_system_info();
   verify_api_version();
   pretro_init();

   g_extern.use_sram = !g_extern.libretro_dummy && !g_extern.libretro_no_rom;

   if (g_extern.libretro_no_rom && !g_extern.libretro_dummy)
   {
      if (!init_rom_file(g_extern.game_type))
         goto error;
   }
   else if (!g_extern.libretro_dummy)
   {
      fill_pathnames();

      if (!init_rom_file(g_extern.game_type))
         goto error;

      set_savestate_auto_index();

      if (!g_extern.sram_load_disable)
         load_save_files();
      else
         RARCH_LOG("Skipping SRAM load.\n");

      load_auto_state();
   }

   init_libretro_cbs();
   init_system_av_info();
   find_drivers();   
   init_drivers();

   rarch_init_rewind();

   init_controllers();

   g_extern.use_sram = g_extern.use_sram && !g_extern.sram_save_disable;

   if (!g_extern.use_sram)
      RARCH_LOG("SRAM will not be saved.\n");

   g_extern.main_is_init  = true;
   return 0;

error:
   uninit_drivers();
   pretro_unload_game();
   pretro_deinit();
   uninit_libretro_sym();

   g_extern.main_is_init = false;
   return 1;
}

static inline bool check_enter_rgui(void)
{
   static bool old_rgui_toggle = true;

   // Always go into menu if dummy core is loaded.
   bool rgui_toggle = input_key_pressed_func(RARCH_MENU_TOGGLE) || (g_extern.libretro_dummy && !old_rgui_toggle);
   /* Clear lifecycle_state from MENU TOGGLE command to avoid further triggering */
   g_extern.lifecycle_state &= ~(1ULL << RARCH_MENU_TOGGLE);
   rgui->old_input_state |= (1ULL << RARCH_MENU_TOGGLE);
   
   if (rgui_toggle && !old_rgui_toggle)
   {
      g_extern.lifecycle_state |= (1ULL << MODE_MENU_PREINIT);
      old_rgui_toggle = true;
      g_extern.system.frame_time_last = 0;
      return true;
   }
   else
   {
      old_rgui_toggle = rgui_toggle;
      return false;
   }
}

static inline void update_frame_time(void)
{
   if (!g_extern.system.frame_time.callback)
      return;

   retro_time_t time = rarch_get_time_usec();
   retro_time_t delta = 0;

   bool is_locked_fps = g_extern.is_paused || driver.nonblock_state;

   if (!g_extern.system.frame_time_last || is_locked_fps)
      delta = g_extern.system.frame_time.reference;
   else
      delta = time - g_extern.system.frame_time_last;

   if (!is_locked_fps && g_extern.is_slowmotion)
      delta /= g_settings.slowmotion_ratio;

   g_extern.system.frame_time_last = is_locked_fps ? 0 : time;
   g_extern.system.frame_time.callback(delta);
}

static inline void limit_frame_time(void)
{
   if (g_settings.fastforward_ratio < 0.0f)
      return;

   retro_time_t current = rarch_get_time_usec();
   retro_time_t target = g_extern.frame_limit.last_frame_time + g_extern.frame_limit.minimum_frame_time;
   retro_time_t to_sleep_ms = (target - current) / 1000;
   if (to_sleep_ms > 0)
   {
      rarch_sleep((unsigned int)to_sleep_ms);
      g_extern.frame_limit.last_frame_time += g_extern.frame_limit.minimum_frame_time; // Combat jitter a bit.
   }
   else
      g_extern.frame_limit.last_frame_time = rarch_get_time_usec();
}

bool rarch_main_iterate(void)
{
   unsigned i;

   // SHUTDOWN on consoles should exit RetroArch completely.
   if (g_extern.system.core_shutdown)
      return false;

   // Time to drop?
   if (input_key_pressed_func(RARCH_QUIT_KEY))
      return false;

   if (check_enter_rgui())
      return false; // Enter menu, don't exit.

   if (g_extern.exec)
   {
      g_extern.exec = false;
      return false;
   }

   // Checks for stuff like save states, etc.
   do_state_checks();

   // Update binds for analog dpad modes.
   for (i = 0; i < MAX_PLAYERS; i++)
      input_push_analog_dpad(g_settings.input.binds[i], g_settings.input.analog_dpad_mode[i]);

   update_frame_time();
   pretro_run();
   limit_frame_time();

   for (i = 0; i < MAX_PLAYERS; i++)
      input_pop_analog_dpad(g_settings.input.binds[i]);

   return true;
}

void rarch_main_deinit(void)
{
   if (g_extern.use_sram)
      save_files();

   rarch_deinit_rewind();

   if (!g_extern.libretro_dummy && !g_extern.libretro_no_rom)
      save_auto_state();

   uninit_drivers();
   pretro_unload_game();
   pretro_deinit();
   uninit_libretro_sym();

   if (g_extern.rom_file_temporary)
   {
      RARCH_LOG("Removing temporary ROM file: %s.\n", g_extern.last_rom);
      if (remove(g_extern.last_rom) < 0)
         RARCH_ERR("Failed to remove temporary file: %s.\n", g_extern.last_rom);
      g_extern.rom_file_temporary = false;
   }

   g_extern.main_is_init = false;
}

#define MAX_ARGS 32

int rarch_main_init_wrap(const struct rarch_main_wrap *args)
{
   unsigned i;

   int argc = 0;
   char *argv[MAX_ARGS] = {NULL};
   char *argv_copy[MAX_ARGS];

   argv[argc++] = strdup("retroarch");

   if (!args->no_rom)
   {
      if (args->rom_path)
      {
         RARCH_LOG("Using ROM: %s.\n", args->rom_path);
         argv[argc++] = strdup(args->rom_path);
      }
      else
      {
         RARCH_LOG("No ROM, starting dummy core.\n");
         argv[argc++] = strdup("--menu");
      }
   }

   if (args->sram_path)
   {
      argv[argc++] = strdup("-s");
      argv[argc++] = strdup(args->sram_path);
   }

   if (args->state_path)
   {
      argv[argc++] = strdup("-S");
      argv[argc++] = strdup(args->state_path);
   }

   if (args->config_path)
   {
      argv[argc++] = strdup("-c");
      argv[argc++] = strdup(args->config_path);
   }

   if (args->verbose)
      argv[argc++] = strdup("-v");

   for (i = 0; i < argc; i++)
      RARCH_LOG("arg #%d: %s\n", i, argv[i]);

   // The pointers themselves are not const, and can be messed around with by getopt_long().
   memcpy(argv_copy, argv, sizeof(argv));

   int ret = rarch_main_init(argc, argv);

   for (i = 0; i < ARRAY_SIZE(argv_copy); i++)
      free(argv_copy[i]);

   return ret;
}

bool rarch_main_idle_iterate(void)
{
   if (input_key_pressed_func(RARCH_QUIT_KEY))
      return false;

   do_state_checks();
   rarch_input_poll();
   rarch_sleep(10);
   return true;
}
