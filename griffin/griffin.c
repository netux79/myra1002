/* RetroArch - A frontend for libretro.
* Copyright (C) 2010-2014 - Hans-Kristian Arntzen
* Copyright (C) 2011-2014 - Daniel De Matteis
*
* RetroArch is free software: you can redistribute it and/or modify it under the terms
* of the GNU General Public License as published by the Free Software Found-
* ation, either version 3 of the License, or (at your option) any later version.
*
* RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE. See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with RetroArch.
* If not, see <http://www.gnu.org/licenses/>.
*/

/*============================================================
CONSOLE EXTENSIONS
============================================================ */
#ifdef HW_DOL
#include "../ngc/ssaram.c"
#endif

#ifdef HAVE_ZLIB
#include "../file_extract.c"
#endif

/*============================================================
PERFORMANCE
============================================================ */
#include "../performance.c"

/*============================================================
COMPATIBILITY
============================================================ */
#include "../compat/compat.c"

/*============================================================
CONFIG FILE
============================================================ */
#include "../conf/config_file.c"
#include "../core_options.c"

/*============================================================
VIDEO IMAGE
============================================================ */
#include "../gfx/image/image.c"

#ifdef WANT_RPNG
#include "../gfx/rpng/rpng.c"
#endif

/*============================================================
VIDEO DRIVER
============================================================ */

#ifdef HW_RVL
#include "../wii/vi_encoder.c"
#include "../wii/mem2_manager.c"
#endif
#include "../gfx/gfx_common.c"
#include "../gx/gx_video.c"

/*============================================================
INPUT
============================================================ */
#include "../input/input_common.c"

#ifdef HAVE_OVERLAY
#include "../input/overlay.c"
#endif

#include "../gx/gx_gxpad.c"
#include "../gx/gx_input.c"
#if defined (HW_RVL) && defined(HAVE_WIIUSBPAD)
#include "../gx/gx_usbpad.c"
#include "../gx/gx_hid_input.c"
#endif

/*============================================================
AUDIO RESAMPLER
============================================================ */
#include "../audio/resampler.c"
#include "../audio/sinc.c"

/*============================================================
AUDIO UTILS
============================================================ */
#include "../audio/utils.c"

/*============================================================
AUDIO
============================================================ */
#include "../gx/gx_audio.c"

/*============================================================
DRIVERS
============================================================ */
#include "../driver.c"

/*============================================================
FILTERS
============================================================ */
#ifdef HAVE_SCALERS_BUILTIN
#include "../gfx/filters/snes_ntsc/snes_ntsc.c"
#include "../gfx/filters/blargg_ntsc.c"
#include "../gfx/filters/epx.c"
#include "../gfx/filters/2xsai.c"
#include "../gfx/filters/supereagle.c"
#include "../gfx/filters/super2xsai.c"
#include "../gfx/filters/hq2x.c"
#include "../gfx/filters/scanlines.c"
#ifdef HAVE_ALL_SCALERS
#include "../gfx/filters/scale2x.c"
#include "../gfx/filters/lq2x.c"
#include "../gfx/filters/phosphor2x.c"
#include "../gfx/filters/2xbr.c"
#include "../gfx/filters/darken.c"
#endif
#include "../gfx/filter.c"
#endif

/*============================================================
DYNAMIC
============================================================ */
#include "../dynamic.c"
#include "../dynamic_dummy.c"

/*============================================================
FILE
============================================================ */
#include "../file.c"
#include "../file_path.c"
#include "../hash.c"

/*============================================================
MESSAGE
============================================================ */
#include "../message_queue.c"

/*============================================================
SETTINGS
============================================================ */
#include "../settings.c"

/*============================================================
REWIND
============================================================ */
#include "../rewind.c"

/*============================================================
FRONTEND
============================================================ */

#include "../frontend/frontend_context.c"
#include "../frontend/platform/platform_gx.c"
#ifdef HW_RVL
#include "../frontend/platform/platform_wii.c"
#endif
#include "../frontend/info/core_info.c"

/*============================================================
MAIN
============================================================ */
#include "../frontend/frontend.c"

/*============================================================
RETROARCH
============================================================ */
#include "../retroarch.c"

/*============================================================
SCREENSHOTS
============================================================ */
#ifdef HAVE_SCREENSHOTS
#include "../screenshot.c"
#endif

/*============================================================
MENU
============================================================ */
#ifdef HAVE_MENU
#include "../frontend/menu/menu_common.c"
#include "../frontend/menu/menu_navigation.c"
#include "../frontend/menu/menu_display.c"
#include "../frontend/menu/menu_settings.c"
#include "../frontend/menu/history.c"
#include "../frontend/menu/file_list.c"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================
RZLIB
============================================================ */
#ifdef WANT_MINIZ
#include "../deps/rzlib/adler32.c"
#include "../deps/rzlib/compress.c"
#include "../deps/rzlib/crc32.c"
#include "../deps/rzlib/deflate.c"
#include "../deps/rzlib/gzclose.c"
#include "../deps/rzlib/gzlib.c"
#include "../deps/rzlib/gzread.c"
#include "../deps/rzlib/gzwrite.c"
#include "../deps/rzlib/inffast.c"
#include "../deps/rzlib/inflate.c"
#include "../deps/rzlib/inftrees.c"
#include "../deps/rzlib/trees.c"
#include "../deps/rzlib/uncompr.c"
#include "../deps/rzlib/zutil.c"
#include "../deps/rzlib/ioapi.c"
#include "../deps/rzlib/unzip.c"
#endif

#ifdef __cplusplus
}
#endif
