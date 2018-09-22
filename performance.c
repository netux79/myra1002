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

#include <stdio.h>
#include "libretro.h"
#include "performance.h"
#include "general.h"

#define __mftb gettime

#if defined(__mips__)
#include <sys/time.h>
#endif

#include <ogc/lwp_watchdog.h>
#include <string.h>

#define MAX_COUNTERS 64
static const struct retro_perf_counter *perf_counters_rarch[MAX_COUNTERS];
static const struct retro_perf_counter *perf_counters_libretro[MAX_COUNTERS];
static unsigned perf_ptr_rarch;
static unsigned perf_ptr_libretro;

void rarch_perf_register(struct retro_perf_counter *perf)
{
   if (perf->registered || perf_ptr_rarch >= MAX_COUNTERS)
      return;

   perf_counters_rarch[perf_ptr_rarch++] = perf;
   perf->registered = true;
}

void retro_perf_register(struct retro_perf_counter *perf)
{
   if (perf->registered || perf_ptr_libretro >= MAX_COUNTERS)
      return;

   perf_counters_libretro[perf_ptr_libretro++] = perf;
   perf->registered = true;
}

void retro_perf_clear(void)
{
   perf_ptr_libretro = 0;
   memset(perf_counters_libretro, 0, sizeof(perf_counters_libretro));
}

#define PERF_LOG_FMT "[PERF]: Avg (%s): %llu ticks, %llu runs.\n"

static void log_counters(const struct retro_perf_counter **counters, unsigned num)
{
   unsigned i;
   for (i = 0; i < num; i++)
   {
      RARCH_LOG(PERF_LOG_FMT,
            counters[i]->ident,
            (unsigned long long)counters[i]->total / (unsigned long long)counters[i]->call_cnt,
            (unsigned long long)counters[i]->call_cnt);
   }
}

void rarch_perf_log(void)
{
#if defined(PERF_TEST) || !defined(RARCH_INTERNAL)
   RARCH_LOG("[PERF]: Performance counters (RetroArch):\n");
   log_counters(perf_counters_rarch, perf_ptr_rarch);
#endif
}

void retro_perf_log(void)
{
   RARCH_LOG("[PERF]: Performance counters (libretro):\n");
   log_counters(perf_counters_libretro, perf_ptr_libretro);
}

retro_perf_tick_t rarch_get_perf_counter(void)
{
   retro_perf_tick_t time = 0;
   time = __mftb();

   return time;
}

retro_time_t rarch_get_time_usec(void)
{
   return ticks_to_microsecs(gettime());
}

uint64_t rarch_get_cpu_features(void)
{
   uint64_t cpu = 0;

   cpu |= RETRO_SIMD_PS;
   RARCH_LOG("[CPUID]: PS: %u\n", !!(cpu & RETRO_SIMD_PS));

   return cpu;
}
