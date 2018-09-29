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

#include "file.h"
#include "general.h"
#include <stdlib.h>
#include <stdbool.h>
#include "libretro.h"
#include <string.h>
#include <time.h>
#include "dynamic.h"
#include "compat/strl.h"
#include "file_extract.h"

static const char *ramtype2str(int type)
{
   switch (type)
   {
      case RETRO_MEMORY_SAVE_RAM:
         return ".srm";
      case RETRO_MEMORY_RTC:
         return ".rtc";
      default:
         return "";
   }
}

// Attempt to save valuable RAM data somewhere ...
static void dump_to_file_desperate(const void *data, size_t size, int type)
{
   const char *base = getenv("HOME");

   if (!base)
      goto error;

   char path[PATH_MAX];
   snprintf(path, sizeof(path), "%s/RetroArch-recovery-", base);
   char timebuf[PATH_MAX];

   time_t time_;
   time(&time_);
   strftime(timebuf, sizeof(timebuf), "%Y-%m-%d-%H-%M-%S", localtime(&time_));
   strlcat(path, timebuf, sizeof(path));
   strlcat(path, ramtype2str(type), sizeof(path));

   if (write_file(path, data, size))
      RARCH_WARN("Succeeded in saving RAM data to \"%s\".\n", path);
   else
      goto error;

   return;

error:
   RARCH_WARN("Failed ... Cannot recover save file.\n");
}

bool save_state(const char *path)
{
   RARCH_LOG("Saving state: \"%s\".\n", path);
   size_t size = pretro_serialize_size();
   if (size == 0)
      return false;

   void *data = malloc(size);
   if (!data)
   {
      RARCH_ERR("Failed to allocate memory for save state buffer.\n");
      return false;
   }

   RARCH_LOG("State size: %d bytes.\n", (int)size);
   bool ret = pretro_serialize(data, size);
   if (ret)
      ret = write_file(path, data, size);

   if (!ret)
      RARCH_ERR("Failed to save state to \"%s\".\n", path);

   free(data);
   return ret;
}

bool load_state(const char *path)
{
   unsigned i;
   void *buf = NULL;
   ssize_t size = read_file(path, &buf);

   RARCH_LOG("Loading state: \"%s\".\n", path);

   if (size < 0)
   {
      RARCH_ERR("Failed to load state from \"%s\".\n", path);
      return false;
   }

   bool ret = true;
   RARCH_LOG("State size: %u bytes.\n", (unsigned)size);

   void *block_buf[2] = {NULL, NULL};
   int block_type[2] = {-1, -1};
   size_t block_size[2] = {0};

   if (g_settings.block_sram_overwrite)
   {
      RARCH_LOG("Blocking SRAM overwrite.\n");
      block_type[0] = RETRO_MEMORY_SAVE_RAM;
      block_type[1] = RETRO_MEMORY_RTC;
   }

   for (i = 0; i < 2; i++)
      if (block_type[i] != -1)
         block_size[i] = pretro_get_memory_size(block_type[i]);

   for (i = 0; i < 2; i++)
      if (block_size[i])
         block_buf[i] = malloc(block_size[i]);

   // Backup current SRAM which is overwritten by unserialize.
   for (i = 0; i < 2; i++)
   {
      if (block_buf[i])
      {
         const void *ptr = pretro_get_memory_data(block_type[i]);
         if (ptr)
            memcpy(block_buf[i], ptr, block_size[i]);
      }
   }

   ret = pretro_unserialize(buf, size);

   // Flush back :D
   for (i = 0; i < 2 && ret; i++)
   {
      if (block_buf[i])
      {
         void *ptr = pretro_get_memory_data(block_type[i]);
         if (ptr)
            memcpy(ptr, block_buf[i], block_size[i]);
      }
   }

   for (i = 0; i < 2; i++)
      if (block_buf[i])
         free(block_buf[i]);

   free(buf);
   return ret;
}

void load_ram_file(const char *path, int type)
{
   size_t size = pretro_get_memory_size(type);
   void *data = pretro_get_memory_data(type);

   if (size == 0 || !data)
      return;

   void *buf = NULL;
   ssize_t rc = read_file(path, &buf);
   if (rc > 0)
   {
      if (rc > (ssize_t)size)
      {
         RARCH_WARN("SRAM is larger than implementation expects, doing partial load (truncating %u bytes to %u).\n",
               (unsigned)rc, (unsigned)size);
         rc = size;
      }
      memcpy(data, buf, rc);
   }

   free(buf);
}

void save_ram_file(const char *path, int type)
{
   size_t size = pretro_get_memory_size(type);
   void *data = pretro_get_memory_data(type);

   if (data && size > 0)
   {
      if (!write_file(path, data, size))
      {
         RARCH_ERR("Failed to save SRAM.\n");
         RARCH_WARN("Attempting to recover ...\n");
         dump_to_file_desperate(data, size, type);
      }
      else
         RARCH_LOG("Saved successfully to \"%s\".\n", path);
   }
}

#define MAX_ROMS 4

static bool load_roms(unsigned rom_type, const char **rom_paths, size_t roms)
{
   size_t i;
   bool ret = true;

   if (roms == 0)
      return false;

   if (roms > MAX_ROMS)
      return false;

   void *rom_buf[MAX_ROMS] = {NULL};
   long rom_len[MAX_ROMS] = {0};
   struct retro_game_info info[MAX_ROMS] = {{NULL}};

   for (i = 0; i < roms; i++)
   {
      RARCH_LOG("Loading ROM file: %s.\n", rom_paths[i]);
      if (rom_paths[i] &&
            !g_extern.system.info.need_fullpath &&
            (rom_len[i] = read_file(rom_paths[i], &rom_buf[i])) == -1)
      {
         RARCH_ERR("Could not read ROM file: \"%s\".\n", rom_paths[i]);
         ret = false;
         goto end;
      }
      RARCH_LOG("ROM size: %u bytes.\n", (unsigned)rom_len[i]);
      
      info[i].path = rom_paths[i];
      info[i].data = rom_buf[i];
      info[i].size = rom_len[i];
      info[i].meta = NULL;
   }

   if (rom_type == 0)
      ret = pretro_load_game(&info[0]);
   else
      ret = pretro_load_game_special(rom_type, info, roms);

   if (!ret)
      RARCH_ERR("Failed to load game.\n");

end:
   for (i = 0; i < MAX_ROMS; i++)
      free(rom_buf[i]);
   return ret;
}

static bool load_normal_rom(void)
{
   if (g_extern.libretro_no_rom && g_extern.system.no_game)
      return pretro_load_game(NULL);
   else if (g_extern.libretro_no_rom && !g_extern.system.no_game)
   {
      RARCH_ERR("No ROM is used, but libretro core does not support this.\n");
      return false;
   }
   else
   {
      const char *path = g_extern.fullpath;
      return load_roms(0, &path, 1);
   }
}

bool init_rom_file(void)
{
#ifdef HAVE_ZLIB
   if (*g_extern.fullpath && !g_extern.system.block_extract)
   {
      const char *ext = path_get_extension(g_extern.fullpath);
      if (ext && !strcasecmp(ext, "zip"))
      {
         g_extern.rom_file_temporary = true;

         if (!zlib_extract_first_rom(g_extern.fullpath, sizeof(g_extern.fullpath), g_extern.system.valid_extensions))
         {
            RARCH_ERR("Failed to extract ROM from zipped file: %s.\n", g_extern.fullpath);
            g_extern.rom_file_temporary = false;
            return false;
         }

         strlcpy(g_extern.last_rom, g_extern.fullpath, sizeof(g_extern.last_rom));
      }
   }
#endif

   if (!load_normal_rom())
      return false;

   return true;
}

