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

#include "file_path.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "compat/strl.h"
#include "general.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

// Dump stuff to file.
bool write_file(const char *path, const void *data, size_t size)
{
   FILE *file = fopen(path, "wb");
   if (!file)
      return false;
   else
   {
      bool ret = fwrite(data, 1, size, file) == size;
      fclose(file);
      return ret;
   }
}

// Generic file loader.
long read_file(const char *path, void **buf)
{
   long rc = 0, len = 0;
   void *rom_buf = NULL;
   FILE *file = fopen(path, "rb");

   if (!file)
      goto error;

   fseek(file, 0, SEEK_END);
   len = ftell(file);
   rewind(file);
   rom_buf = malloc(len + 1);
   if (!rom_buf)
   {
      RARCH_ERR("Couldn't allocate memory.\n");
      goto error;
   }

   if ((rc = fread(rom_buf, 1, len, file)) < len)
      RARCH_WARN("Didn't read whole file.\n");

   *buf = rom_buf;
   // Allow for easy reading of strings to be safe.
   // Will only work with sane character formatting (Unix).
   ((char*)rom_buf)[len] = '\0';
   fclose(file);
   return rc;

error:
   if (file)
      fclose(file);
   free(rom_buf);
   *buf = NULL;
   return -1;
}

// Reads file content as one string.
bool read_file_string(const char *path, char **buf)
{
   *buf = NULL;
   FILE *file = fopen(path, "r");
   long len = 0;
   char *ptr = NULL;

   if (!file)
      goto error;

   // ftell with "r" can be troublesome ...
   // Haven't run into issues yet though.
   fseek(file, 0, SEEK_END);
   len = ftell(file) + 2; // Takes account of being able to read in EOF and '\0' at end.
   rewind(file);

   *buf = (char*)calloc(len, sizeof(char));
   if (!*buf)
      goto error;

   ptr = *buf;

   while (ptr && !feof(file))
   {
      size_t bufsize = (size_t)(((ptrdiff_t)*buf + (ptrdiff_t)len) - (ptrdiff_t)ptr);
      fgets(ptr, bufsize, file);

      ptr += strlen(ptr);
   }

   ptr = strchr(ptr, EOF);
   if (ptr)
      *ptr = '\0';

   fclose(file);
   return true;

error:
   if (file)
      fclose(file);
   if (*buf)
      free(*buf);
   return false;
}

void string_list_free(struct string_list *list)
{
   size_t i;
   if (!list)
      return;

   for (i = 0; i < list->size; i++)
      free(list->elems[i].data);
   free(list->elems);
   free(list);
}

static bool string_list_capacity(struct string_list *list, size_t cap)
{
   rarch_assert(cap > list->size);

   struct string_list_elem *new_data = (struct string_list_elem*)realloc(list->elems, cap * sizeof(*new_data));
   if (!new_data)
      return false;

   list->elems = new_data;
   list->cap   = cap;
   return true;
}

struct string_list *string_list_new(void)
{
   struct string_list *list = (struct string_list*)calloc(1, sizeof(*list));
   if (!list)
      return NULL;

   if (!string_list_capacity(list, 32))
   {
      string_list_free(list);
      return NULL;
   }

   return list;
}

bool string_list_append(struct string_list *list, const char *elem, union string_list_elem_attr attr)
{
   if (list->size >= list->cap &&
         !string_list_capacity(list, list->cap * 2))
      return false;

   char *dup = strdup(elem);
   if (!dup)
      return false;

   list->elems[list->size].data = dup;
   list->elems[list->size].attr = attr;

   list->size++;
   return true;
}

void string_list_join_concat(char *buffer, size_t size, const struct string_list *list, const char *sep)
{
   size_t len = strlen(buffer);
   rarch_assert(len < size);
   buffer += len;
   size -= len;

   size_t i;
   for (i = 0; i < list->size; i++)
   {
      strlcat(buffer, list->elems[i].data, size);
      if ((i + 1) < list->size)
         strlcat(buffer, sep, size);
   }
}

struct string_list *string_split(const char *str, const char *delim)
{
   char *copy      = NULL;
   const char *tmp = NULL;

   struct string_list *list = string_list_new();
   if (!list)
      goto error;

   copy = strdup(str);
   if (!copy)
      goto error;

   char *save;
   tmp = strtok_r(copy, delim, &save);
   while (tmp)
   {
      union string_list_elem_attr attr;
      memset(&attr, 0, sizeof(attr));

      if (!string_list_append(list, tmp, attr))
         goto error;

      tmp = strtok_r(NULL, delim, &save);
   }

   free(copy);
   return list;

error:
   string_list_free(list);
   free(copy);
   return NULL;
}

bool string_list_find_elem(const struct string_list *list, const char *elem)
{
   size_t i;
   if (!list)
      return false;

   for (i = 0; i < list->size; i++)
   {
      if (strcasecmp(list->elems[i].data, elem) == 0)
         return true;
   }

   return false;
}

bool string_list_find_elem_prefix(const struct string_list *list, const char *prefix, const char *elem)
{
   size_t i;
   if (!list)
      return false;

   char prefixed[MAX_LEN];
   snprintf(prefixed, sizeof(prefixed), "%s%s", prefix, elem);

   for (i = 0; i < list->size; i++)
   {
      if (strcasecmp(list->elems[i].data, elem) == 0 ||
            strcasecmp(list->elems[i].data, prefixed) == 0)
         return true;
   }

   return false;
}

const char *path_get_extension(const char *path)
{
   const char *ext = strrchr(path_basename(path), '.');
   if (ext)
      return ext + 1;
   else
      return "";
}

char *path_remove_extension(char *path)
{
   char *last = (char*)strrchr(path_basename(path), '.');
   if (*last)
      *last = '\0';
   return last;
}

static int qstrcmp_plain(const void *a_, const void *b_)
{
   const struct string_list_elem *a = (const struct string_list_elem*)a_; 
   const struct string_list_elem *b = (const struct string_list_elem*)b_; 

   return strcasecmp(a->data, b->data);
}

static int qstrcmp_dir(const void *a_, const void *b_)
{
   const struct string_list_elem *a = (const struct string_list_elem*)a_; 
   const struct string_list_elem *b = (const struct string_list_elem*)b_; 

   // Sort directories before files.
   int a_dir = a->attr.b;
   int b_dir = b->attr.b;
   if (a_dir != b_dir)
      return b_dir - a_dir;
   else
      return strcasecmp(a->data, b->data);
}

void dir_list_sort(struct string_list *list, bool dir_first)
{
   if (!list)
      return;

   qsort(list->elems, list->size, sizeof(struct string_list_elem),
         dir_first ? qstrcmp_dir : qstrcmp_plain);
}

static bool dirent_is_directory(const char *path, const struct dirent *entry)
{
#ifdef DT_DIR
   if (entry->d_type == DT_DIR)
      return true;
   else if (entry->d_type == DT_UNKNOWN // This can happen on certain file systems.
         || entry->d_type == DT_LNK)
      return path_is_directory(path);
   else
      return false;
#else // dirent struct doesn't have d_type, do it the slow way ...
   return path_is_directory(path);
#endif
}

struct string_list *dir_list_new(const char *dir, const char *ext, bool include_dirs)
{
   struct string_list *list = string_list_new();
   if (!list)
      return NULL;

   DIR *directory = NULL;
   const struct dirent *entry = NULL;

   struct string_list *ext_list = NULL;
   if (ext)
      ext_list = string_split(ext, "|");

   directory = opendir(dir);
   if (!directory)
      goto error;

   while ((entry = readdir(directory)))
   {
      const char *name     = entry->d_name;
      const char *file_ext = path_get_extension(name);

      char file_path[MAX_LEN];
      fill_pathname_join(file_path, dir, name, sizeof(file_path));

      bool is_dir = dirent_is_directory(file_path, entry);
      if (!include_dirs && is_dir)
         continue;

      if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
         continue;

      if (!is_dir && ext_list && !string_list_find_elem_prefix(ext_list, ".", file_ext))
         continue;

      union string_list_elem_attr attr;
      attr.b = is_dir;

      if (!string_list_append(list, file_path, attr))
         goto error;
   }

   closedir(directory);

   string_list_free(ext_list);
   return list;

error:
   RARCH_ERR("Failed to open directory: \"%s\"\n", dir);

   if (directory)
      closedir(directory);

   string_list_free(list);
   string_list_free(ext_list);
   return NULL;
}

void dir_list_free(struct string_list *list)
{
   string_list_free(list);
}

static bool path_char_is_slash(char c)
{
   return c == '/';
}

static const char *path_default_slash(void)
{
   return "/";
}


bool path_is_directory(const char *path)
{
   struct stat buf;
   if (stat(path, &buf) < 0)
      return false;

   return S_ISDIR(buf.st_mode);
}

bool path_file_exists(const char *path)
{
   FILE *dummy = fopen(path, "rb");
   if (dummy)
   {
      fclose(dummy);
      return true;
   }
   return false;
}

void fill_pathname(char *out_path, const char *in_path, const char *replace, size_t size)
{
   char tmp_path[MAX_LEN];

   rarch_assert(strlcpy(tmp_path, in_path, sizeof(tmp_path)) < sizeof(tmp_path));
   char *tok = (char*)strrchr(path_basename(tmp_path), '.');
   if (tok)
      *tok = '\0';

   rarch_assert(strlcpy(out_path, tmp_path, size) < size);
   rarch_assert(strlcat(out_path, replace, size) < size);
}

void fill_pathname_noext(char *out_path, const char *in_path, const char *replace, size_t size)
{
   rarch_assert(strlcpy(out_path, in_path, size) < size);
   rarch_assert(strlcat(out_path, replace, size) < size);
}

static char *find_last_slash(const char *str)
{
   const char *slash = strrchr(str, '/');

   return (char*)slash;
}

// Assumes path is a directory. Appends a slash
// if not already there.
static void fill_pathname_slash(char *path, size_t size)
{
   size_t path_len = strlen(path);
   const char *last_slash = find_last_slash(path);

   // Try to preserve slash type.
   if (last_slash && (last_slash != (path + path_len - 1)))
   {
      char join_str[2] = {*last_slash};
      rarch_assert(strlcat(path, join_str, size) < size);
   }
   else if (!last_slash)
      rarch_assert(strlcat(path, path_default_slash(), size) < size);
}

void fill_pathname_dir(char *in_dir, const char *in_basename, const char *replace, size_t size)
{
   fill_pathname_slash(in_dir, size);
   const char *base = path_basename(in_basename);
   rarch_assert(strlcat(in_dir, base, size) < size);
   rarch_assert(strlcat(in_dir, replace, size) < size);
}

void fill_pathname_base(char *out, const char *in_path, size_t size)
{
   const char *ptr = find_last_slash(in_path);

   if (ptr)
      ptr++;
   else
      ptr = in_path;

   rarch_assert(strlcpy(out, ptr, size) < size);
}

void fill_pathname_basedir(char *out_dir, const char *in_path, size_t size)
{
   rarch_assert(strlcpy(out_dir, in_path, size) < size);
   path_basedir(out_dir);
}

void fill_pathname_parent_dir(char *out_dir, const char *in_dir, size_t size)
{
   rarch_assert(strlcpy(out_dir, in_dir, size) < size);
   path_parent_dir(out_dir);
}

void fill_dated_filename(char *out_filename, const char *ext, size_t size)
{
   time_t cur_time;
   time(&cur_time);

   strftime(out_filename, size, "RetroArch-%m%d-%H%M%S.", localtime(&cur_time));
   strlcat(out_filename, ext, size);
}

void path_basedir(char *path)
{
   if (strlen(path) < 2)
      return;

   char *last = find_last_slash(path);

   if (last)
      last[1] = '\0';
   else
      snprintf(path, 3, ".%s", path_default_slash());
}

void path_parent_dir(char *path)
{
   size_t len = strlen(path);
   if (len && path_char_is_slash(path[len - 1]))
      path[len - 1] = '\0';
   path_basedir(path);
}

const char *path_basename(const char *path)
{
   const char *last = find_last_slash(path);

   if (last)
      return last + 1;
   else
      return path;
}

bool path_is_absolute(const char *path)
{
   return path[0] == '/';
}

static bool path_mkdir_norecurse(const char *dir)
{
   int ret = mkdir(dir, 0750);
   
   if (ret < 0 && errno == EEXIST && path_is_directory(dir)) // Don't treat this as an error.
      ret = 0;
   if (ret < 0)
      RARCH_ERR("mkdir(%s) error: %s.\n", dir, strerror(errno));
   return ret == 0;
}

bool path_mkdir(const char *dir)
{
   const char *target = NULL;
   char *basedir = strdup(dir); // Use heap. Real chance of stack overflow if we recurse too hard.
   bool ret = true;

   if (!basedir)
      return false;

   path_parent_dir(basedir);
   if (!*basedir || !strcmp(basedir, dir))
   {
      ret = false;
      goto end;
   }

   if (path_is_directory(basedir))
   {
      target = dir;
      ret = path_mkdir_norecurse(dir);
   }
   else
   {
      target = basedir;
      ret = path_mkdir(basedir);
      if (ret)
      {
         target = dir;
         ret = path_mkdir_norecurse(dir);
      }
   }

end:
   if (target && !ret)
      RARCH_ERR("Failed to create directory: \"%s\".\n", target);
   free(basedir);
   return ret;
}

void fill_pathname_resolve_relative(char *out_path, const char *in_refpath, const char *in_path, size_t size)
{
   if (path_is_absolute(in_path))
      rarch_assert(strlcpy(out_path, in_path, size) < size);
   else
   {
      rarch_assert(strlcpy(out_path, in_refpath, size) < size);
      path_basedir(out_path);
      rarch_assert(strlcat(out_path, in_path, size) < size);
   }
}

void fill_pathname_join(char *out_path, const char *dir, const char *path, size_t size)
{
   rarch_assert(strlcpy(out_path, dir, size) < size);

   if (*out_path)
      fill_pathname_slash(out_path, size);

   rarch_assert(strlcat(out_path, path, size) < size);
}
