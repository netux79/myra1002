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

#include "dynamic.h"
#include "general.h"
#include "compat/strl.h"
#include "retroarch_logger.h"
#include "performance.h"
#include "file.h"
#include <string.h>
#include <ctype.h>
#include "console/rarch_console.h"

#include <stdbool.h>
#include "libretro_private.h"
#include "dynamic_dummy.h"

#define SYM(x) p##x = x

#define SYM_DUMMY(x) p##x = libretro_dummy_##x

void (*pretro_init)(void);
void (*pretro_deinit)(void);

unsigned (*pretro_api_version)(void);

void (*pretro_get_system_info)(struct retro_system_info*);
void (*pretro_get_system_av_info)(struct retro_system_av_info*);

void (*pretro_set_environment)(retro_environment_t);
void (*pretro_set_video_refresh)(retro_video_refresh_t);
void (*pretro_set_audio_sample)(retro_audio_sample_t);
void (*pretro_set_audio_sample_batch)(retro_audio_sample_batch_t);
void (*pretro_set_input_poll)(retro_input_poll_t);
void (*pretro_set_input_state)(retro_input_state_t);

void (*pretro_set_controller_port_device)(unsigned, unsigned);

void (*pretro_reset)(void);
void (*pretro_run)(void);

size_t (*pretro_serialize_size)(void);
bool (*pretro_serialize)(void*, size_t);
bool (*pretro_unserialize)(const void*, size_t);

bool (*pretro_load_game)(const struct retro_game_info*);
bool (*pretro_load_game_special)(unsigned, const struct retro_game_info*, size_t);

void (*pretro_unload_game)(void);

unsigned (*pretro_get_region)(void);

void *(*pretro_get_memory_data)(unsigned);
size_t (*pretro_get_memory_size)(unsigned);

const struct retro_controller_description *libretro_find_controller_description(const struct retro_controller_info *info, unsigned id)
{
   unsigned i;
   for (i = 0; i < info->num_types; i++)
   {
      if (info->types[i].id == id)
         return &info->types[i];
   }

   return NULL;
}

static void load_symbols(bool is_dummy)
{
   if (is_dummy)
   {
      SYM_DUMMY(retro_init);
      SYM_DUMMY(retro_deinit);

      SYM_DUMMY(retro_api_version);
      SYM_DUMMY(retro_get_system_info);
      SYM_DUMMY(retro_get_system_av_info);

      SYM_DUMMY(retro_set_environment);
      SYM_DUMMY(retro_set_video_refresh);
      SYM_DUMMY(retro_set_audio_sample);
      SYM_DUMMY(retro_set_audio_sample_batch);
      SYM_DUMMY(retro_set_input_poll);
      SYM_DUMMY(retro_set_input_state);

      SYM_DUMMY(retro_set_controller_port_device);

      SYM_DUMMY(retro_reset);
      SYM_DUMMY(retro_run);

      SYM_DUMMY(retro_serialize_size);
      SYM_DUMMY(retro_serialize);
      SYM_DUMMY(retro_unserialize);

      SYM_DUMMY(retro_load_game);
      SYM_DUMMY(retro_load_game_special);

      SYM_DUMMY(retro_unload_game);
      SYM_DUMMY(retro_get_region);
      SYM_DUMMY(retro_get_memory_data);
      SYM_DUMMY(retro_get_memory_size);
   }
   else
   {
      SYM(retro_init);
      SYM(retro_deinit);

      SYM(retro_api_version);
      SYM(retro_get_system_info);
      SYM(retro_get_system_av_info);

      SYM(retro_set_environment);
      SYM(retro_set_video_refresh);
      SYM(retro_set_audio_sample);
      SYM(retro_set_audio_sample_batch);
      SYM(retro_set_input_poll);
      SYM(retro_set_input_state);

      SYM(retro_set_controller_port_device);

      SYM(retro_reset);
      SYM(retro_run);

      SYM(retro_serialize_size);
      SYM(retro_serialize);
      SYM(retro_unserialize);

      SYM(retro_load_game);
      SYM(retro_load_game_special);

      SYM(retro_unload_game);
      SYM(retro_get_region);
      SYM(retro_get_memory_data);
      SYM(retro_get_memory_size);
   }
}

void libretro_get_current_core_pathname(char *name, size_t size)
{
   size_t i;
   if (size == 0)
      return;

   struct retro_system_info info = {0};
   pretro_get_system_info(&info);
   const char *id = info.library_name ? info.library_name : "Unknown";

   if (!id || strlen(id) >= size)
   {
      name[0] = '\0';
      return;
   }

   name[strlen(id)] = '\0';

   for (i = 0; id[i] != '\0'; i++)
   {
      char c = id[i];
      if (isspace(c) || isblank(c))
         name[i] = '_';
      else
         name[i] = tolower(c);
   }
}

void init_libretro_sym(bool dummy)
{
   // Guarantee that we can do "dirty" casting.
   // Every OS that this program supports should pass this ...
   rarch_assert(sizeof(void*) == sizeof(void (*)(void)));

   load_symbols(dummy);

   pretro_set_environment(rarch_environment_cb);
}

void uninit_libretro_sym(void)
{
   if (g_extern.system.core_options)
   {
      core_option_flush(g_extern.system.core_options);
      core_option_free(g_extern.system.core_options);
   }

   // No longer valid.
   free(g_extern.system.ports);
   memset(&g_extern.system, 0, sizeof(g_extern.system));

   // Performance counters no longer valid.
   retro_perf_clear();
}

static void rarch_log_libretro(enum retro_log_level level, const char *fmt, ...)
{
   va_list vp;
   va_start(vp, fmt);

   switch (level)
   {
      case RETRO_LOG_DEBUG:
         RARCH_LOG_V("[libretro DEBUG] :: ", fmt, vp);
         break;

      case RETRO_LOG_INFO:
         RARCH_LOG_OUTPUT_V("[libretro INFO] :: ", fmt, vp);
         break;

      case RETRO_LOG_WARN:
         RARCH_WARN_V("[libretro WARN] :: ", fmt, vp);
         break;

      case RETRO_LOG_ERROR:
         RARCH_ERR_V("[libretro ERROR] :: ", fmt, vp);
         break;

      default:
         break;
   }

   va_end(vp);
}

bool rarch_environment_cb(unsigned cmd, void *data)
{
   unsigned p, id;
   switch (cmd)
   {
      case RETRO_ENVIRONMENT_GET_OVERSCAN:
         *(bool*)data = !g_settings.video.crop_overscan;
         RARCH_LOG("Environ GET_OVERSCAN: %u\n", (unsigned)!g_settings.video.crop_overscan);
         break;

      case RETRO_ENVIRONMENT_GET_CAN_DUPE:
         *(bool*)data = true;
         RARCH_LOG("Environ GET_CAN_DUPE: true\n");
         break;

      case RETRO_ENVIRONMENT_GET_VARIABLE:
      {
         struct retro_variable *var = (struct retro_variable*)data;
         RARCH_LOG("Environ GET_VARIABLE %s:\n", var->key);

         if (g_extern.system.core_options)
            core_option_get(g_extern.system.core_options, var);
         else
            var->value = NULL;

         RARCH_LOG("\t%s\n", var->value ? var->value : "N/A");
         break;
      }

      case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
         *(bool*)data = g_extern.system.core_options ?
            core_option_updated(g_extern.system.core_options) : false;
         break;

      case RETRO_ENVIRONMENT_SET_VARIABLES:
      {
         RARCH_LOG("Environ SET_VARIABLES.\n");

         if (g_extern.system.core_options)
         {
            core_option_flush(g_extern.system.core_options);
            core_option_free(g_extern.system.core_options);
         }

         const struct retro_variable *vars = (const struct retro_variable*)data;

         char options_path[MAX_LEN];
         if (*g_extern.config_path)
         {
            fill_pathname_resolve_relative(options_path, g_extern.config_path, "retroarch-core-options.cfg", sizeof(options_path));
         }
         g_extern.system.core_options = core_option_new(options_path, vars);

         break;
      }

      case RETRO_ENVIRONMENT_SET_MESSAGE:
      {
         const struct retro_message *msg = (const struct retro_message*)data;
         RARCH_LOG("Environ SET_MESSAGE: %s\n", msg->msg);
         if (g_extern.msg_queue)
         {
            msg_queue_clear(g_extern.msg_queue);
            msg_queue_push(g_extern.msg_queue, msg->msg, 1, msg->frames);
         }
         break;
      }

      case RETRO_ENVIRONMENT_SET_ROTATION:
      {
         unsigned rotation = *(const unsigned*)data;
         RARCH_LOG("Environ SET_ROTATION: %u\n", rotation);
         
         /* This will be only honored if AUTO rotation is set 
          * For the Wii, we will use a CRT and for vertical 
          * orientations we are rotating the actual TV but
          * we want the screen rotate for flipped games 
          * (ORIENTATION_FLIPPED and ORIENTATION_FLIPPED_ROTATED */
          
         switch (rotation)
         {
            case ORIENTATION_NORMAL:
            case ORIENTATION_VERTICAL:
               g_extern.video.original_orientation = ORIENTATION_NORMAL;
               break;
            case ORIENTATION_FLIPPED:
            case ORIENTATION_FLIPPED_ROTATED:
               g_extern.video.original_orientation = ORIENTATION_FLIPPED;
               break;
         }
         break;
      }

      case RETRO_ENVIRONMENT_SHUTDOWN:
         RARCH_LOG("Environ SHUTDOWN.\n");
         g_extern.system_shutdown = true;
         break;

      case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
         *(const char**)data = *g_settings.system_directory ? g_settings.system_directory : NULL;
         RARCH_LOG("Environ SYSTEM_DIRECTORY: \"%s\".\n", g_settings.system_directory);
         break;

      case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
         *(const char**)data = *g_settings.savefile_directory ? g_settings.savefile_directory : NULL;
         RARCH_LOG("Environ SAVE_DIRECTORY: \"%s\".\n", g_settings.savefile_directory);
         break;

      case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
      {
         enum retro_pixel_format pix_fmt = *(const enum retro_pixel_format*)data;
         switch (pix_fmt)
         {
            case RETRO_PIXEL_FORMAT_RGB565:
               RARCH_LOG("Environ SET_PIXEL_FORMAT: RGB565.\n");
               break;
            case RETRO_PIXEL_FORMAT_XRGB8888:
               RARCH_LOG("Environ SET_PIXEL_FORMAT: XRGB8888.\n");
               break;
            default:
               return false;
         }

         g_extern.system.pix_fmt = pix_fmt;
         break;
      }

      case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS:
      {
         memset(g_extern.system.input_desc_btn, 0, sizeof(g_extern.system.input_desc_btn));

         const struct retro_input_descriptor *desc = (const struct retro_input_descriptor*)data;
         for (; desc->description; desc++)
         {
            if (desc->port >= MAX_PLAYERS)
               continue;

            if (desc->device != RETRO_DEVICE_JOYPAD) // Ignore all others for now.
               continue;

            if (desc->id >= RARCH_FIRST_CUSTOM_BIND)
               continue;

            g_extern.system.input_desc_btn[desc->port][desc->id] = desc->description;
         }

         static const char *libretro_btn_desc[] = {
            "B (bottom)", "Y (left)", "Select", "Start",
            "D-Pad Up", "D-Pad Down", "D-Pad Left", "D-Pad Right",
            "A (right)", "X (up)",
            "L", "R", "L2", "R2", "L3", "R3",
         };
         
         (void)libretro_btn_desc;

         RARCH_LOG("Environ SET_INPUT_DESCRIPTORS:\n");
         for (p = 0; p < MAX_PLAYERS; p++)
         {
            for (id = 0; id < RARCH_FIRST_CUSTOM_BIND; id++)
            {
               const char *desc = g_extern.system.input_desc_btn[p][id];
               if (desc)
               {
                  RARCH_LOG("\tRetroPad, Player %u, Button \"%s\" => \"%s\"\n",
                        p + 1, libretro_btn_desc[id], desc);
               }
            }
         }

         break;
      }

      case RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE:
         RARCH_LOG("Environ SET_DISK_CONTROL_INTERFACE.\n");
         g_extern.system.disk_control = *(const struct retro_disk_control_callback*)data;
         break;

      case RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME:
      {
         bool state = *(const bool*)data;
         RARCH_LOG("Environ SET_SUPPORT_NO_GAME: %s.\n", state ? "yes" : "no");
         g_extern.system.no_game = state;
         break;
      }

      case RETRO_ENVIRONMENT_GET_LIBRETRO_PATH:
      {
         const char **path = (const char**)data;
         *path = NULL;
         break;
      }

      case RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK:
      {
         RARCH_LOG("Environ SET_FRAME_TIME_CALLBACK.\n");

         const struct retro_frame_time_callback *info = (const struct retro_frame_time_callback*)data;
         g_extern.system.frame_time = *info;
         break;
      }

      case RETRO_ENVIRONMENT_GET_RUMBLE_INTERFACE:
      {
         RARCH_LOG("Environ GET_RUMBLE_INTERFACE.\n");
         struct retro_rumble_interface *iface = (struct retro_rumble_interface*)data;
         iface->set_rumble_state = driver_set_rumble_state;
         break;
      }

      case RETRO_ENVIRONMENT_GET_INPUT_DEVICE_CAPABILITIES:
      {
         RARCH_LOG("Environ GET_INPUT_DEVICE_CAPABILITIES.\n");
         uint64_t *mask = (uint64_t*)data;
         if (driver.input && driver.input->get_capabilities && driver.input_data)
            *mask = driver.input->get_capabilities(driver.input_data);
         else
            return false;
         break;
      }



      case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
      {
         RARCH_LOG("Environ GET_LOG_INTERFACE.\n");
         struct retro_log_callback *cb = (struct retro_log_callback*)data;
         cb->log = rarch_log_libretro;
         break;
      }

      case RETRO_ENVIRONMENT_GET_PERF_INTERFACE:
      {
         RARCH_LOG("Environ GET_PERF_INTERFACE.\n");
         struct retro_perf_callback *cb = (struct retro_perf_callback*)data;
         cb->get_time_usec    = rarch_get_time_usec;
         cb->get_cpu_features = rarch_get_cpu_features;
         cb->get_perf_counter = rarch_get_perf_counter;
         cb->perf_register    = retro_perf_register; // libretro specific path.
         cb->perf_start       = rarch_perf_start;
         cb->perf_stop        = rarch_perf_stop;
         cb->perf_log         = retro_perf_log; // libretro specific path.
         break;
      }

      // Private extensions for internal use, not part of libretro API.
      case RETRO_ENVIRONMENT_SET_LIBRETRO_PATH:
         RARCH_LOG("Environ (Private) SET_LIBRETRO_PATH.\n");

         if (path_file_exists((const char*)data))
            strlcpy(g_settings.libretro, (const char*)data, sizeof(g_settings.libretro));
         else
            return false;
         break;

      case RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO:
      {
         RARCH_LOG("Environ SET_SYSTEM_AV_INFO.\n");
         return driver_update_system_av_info((const struct retro_system_av_info*)data);
      }

      case RETRO_ENVIRONMENT_SET_CONTROLLER_INFO:
      {
         RARCH_LOG("Environ SET_CONTROLLER_INFO.\n");
         unsigned i, j;
         const struct retro_controller_info *info = (const struct retro_controller_info*)data;
         for (i = 0; info[i].types; i++)
         {
            RARCH_LOG("Controller port: %u\n", i + 1);
            for (j = 0; j < info[i].num_types; j++)
               RARCH_LOG("   %s (ID: %u)\n", info[i].types[j].desc, info[i].types[j].id);
         }

         free(g_extern.system.ports);
         g_extern.system.ports = (struct retro_controller_info*)calloc(i, sizeof(*g_extern.system.ports));
         if (!g_extern.system.ports)
            return false;

         memcpy(g_extern.system.ports, info, i * sizeof(*g_extern.system.ports));
         g_extern.system.num_ports = i;
         break;
      }

      case RETRO_ENVIRONMENT_EXEC:

         if (data)
            strlcpy(g_extern.fullpath, (const char*)data, sizeof(g_extern.fullpath));
         else
            *g_extern.fullpath = '\0';

         g_extern.lifecycle_state &= ~(1ULL << MODE_GAME);
         g_extern.lifecycle_state |= (1ULL << MODE_EXITSPAWN);
         g_extern.lifecycle_state |= (1ULL << MODE_EXITSPAWN_START_GAME);

         RARCH_LOG("Environ (Private) EXEC.\n");
         break;

      default:
         RARCH_LOG("Environ UNSUPPORTED (#%u).\n", cmd);
         return false;
   }

   return true;
}

