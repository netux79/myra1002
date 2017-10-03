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

#include "../driver.h"
#include "../general.h"
#include "../gfx/fonts/bitmap.h"
#include "../frontend/menu/menu_common.h"
#include "../gfx/gfx_common.h"

#ifdef HW_RVL
#include "../wii/mem2_manager.h"
#endif

#include "gx_video.h"
#include <gccore.h>
#include <ogcsys.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#define SYSMEM1_SIZE 0x01800000

static unsigned gx_resolutions[][2] = {
    { 256, 192 },
    { 299, 200 },
    { 320, 200 },
    { 170, 224 },
    { 192, 224 },
    { 224, 224 },
    { 240, 224 },
    { 256, 224 },
    { 304, 224 },
    { 320, 224 },
    { 384, 224 },
    { 256, 232 },
    { 256, 236 },
    { 192, 240 },
    { 256, 240 },
    { 265, 240 },
    { 288, 240 },
    { 320, 240 },
    { 512, 384 },
    { 598, 400 },
    { 640, 400 },
    { 340, 448 },
    { 384, 448 },
    { 448, 448 },
    { 480, 448 },
    { 512, 448 },
    { 608, 448 },
    { 640, 448 },
    { 512, 464 },
    { 512, 472 },
    { 384, 480 },
    { 512, 480 },
    { 530, 480 },
    { 576, 448 },
    { 640, 480 },
    { 0, 0 },
};

static void *g_fb[2];
static unsigned g_curfb;

static bool g_vsync_state;
static volatile bool g_vsync;
#define WAIT_VBLANK true
#define NO_WAIT false

extern rgui_handle_t *rgui;

static struct
{
   uint32_t *data;
   GXTexObj obj;
} game_tex;

static struct
{
   uint32_t data[240 * 200];
   GXTexObj obj;
} menu_tex ATTRIBUTE_ALIGN(32);

static uint8_t gx_fifo[GX_FIFO_MINSIZE] ATTRIBUTE_ALIGN(32);
static uint8_t display_list[1024] ATTRIBUTE_ALIGN(32);
static size_t display_list_size;

static float verts[16] ATTRIBUTE_ALIGN(32) = {
   -1,  1, -0.5,
    1,  1, -0.5,
   -1, -1, -0.5,
    1, -1, -0.5,
};

static float vertex_ptr[8] ATTRIBUTE_ALIGN(32) = {
   0, 0,
   1, 0,
   0, 1,
   1, 1,
};

static u8 color_ptr[16] ATTRIBUTE_ALIGN(32)  = {
   0xFF, 0xFF, 0xFF, 0xFF,
   0xFF, 0xFF, 0xFF, 0xFF,
   0xFF, 0xFF, 0xFF, 0xFF,
   0xFF, 0xFF, 0xFF, 0xFF,
};

static void vblank_cb(u32 retrace_count)
{
   (void)retrace_count;
   g_vsync = NO_WAIT;
}

static void gx_set_refresh_rate(void *data, unsigned res_index)
{
   unsigned modetype, fbLines;
   gx_video_t *gx = (gx_video_t*)data;
   
   fbLines = gx_resolutions[res_index][1] ? gx_resolutions[res_index][1] : 480;

   if (fbLines <= gx->tvinfo.max_height / 2)
      modetype = VI_NON_INTERLACE;
   else if (gx->tvinfo.progressive)
      modetype = VI_PROGRESSIVE;
   else
      modetype = VI_INTERLACE;

   if (gx->tvinfo.tvmode == VI_PAL)
   {
      if (modetype == VI_NON_INTERLACE)
         driver_set_monitor_refresh_rate(50.0801f);
      else
         driver_set_monitor_refresh_rate(50.0f);
   }
   else
   {
      if (modetype == VI_NON_INTERLACE)
         driver_set_monitor_refresh_rate(59.8261f);
      else
         driver_set_monitor_refresh_rate(59.94f);
   }
}

static void gx_get_tvinfo(void *data)
{
   gx_video_t *gx = (gx_video_t*)data;
#ifdef HW_RVL
   gx->tvinfo.progressive = CONF_GetProgressiveScan() > 0 && VIDEO_HaveComponentCable();
   
   switch (CONF_GetVideo())
   {
      case CONF_VIDEO_PAL:
         if (CONF_GetEuRGB60() > 0)
            gx->tvinfo.tvmode = VI_EURGB60;
         else
            gx->tvinfo.tvmode = VI_PAL;
         break;
      case CONF_VIDEO_MPAL:
         gx->tvinfo.tvmode = VI_MPAL;
         break;
      default:
         gx->tvinfo.tvmode = VI_NTSC;
         break;
   }
#else
   gx->tvinfo.progressive = VIDEO_HaveComponentCable();
   gx->tvinfo.tvmode = VIDEO_GetCurrentTvMode();
#endif

   switch (gx->tvinfo.tvmode)
   {
      case VI_PAL:
         gx->tvinfo.max_width = VI_MAX_WIDTH_PAL;
         gx->tvinfo.max_height = VI_MAX_HEIGHT_PAL;
         break;
      case VI_MPAL:
         gx->tvinfo.max_width = VI_MAX_WIDTH_MPAL;
         gx->tvinfo.max_height = VI_MAX_HEIGHT_MPAL;
         break;
      case VI_EURGB60:
         gx->tvinfo.max_width = VI_MAX_WIDTH_EURGB60;
         gx->tvinfo.max_height = VI_MAX_HEIGHT_EURGB60;
         break;
      default:
         gx->tvinfo.max_width = VI_MAX_WIDTH_NTSC;
         gx->tvinfo.max_height = VI_MAX_HEIGHT_NTSC;
         break;
   }
}

static void gx_set_video_mode(void *data, unsigned res_index)
{
   unsigned modetype, viHeightMultiplier;
   unsigned i, fbWidth, fbLines;
   GXRModeObj gx_mode;
   gx_video_t *gx = (gx_video_t*)data;

   /* Get the selected resolution values */
   fbWidth = gx_resolutions[res_index][0];
   fbLines = gx_resolutions[res_index][1];

   if (fbLines == 0 || fbWidth == 0)
   {
      GXRModeObj tmp_mode;
      VIDEO_GetPreferredMode(&tmp_mode);
      fbWidth = tmp_mode.fbWidth;
      fbLines = tmp_mode.xfbHeight;
   }

   if (fbLines <= gx->tvinfo.max_height / 2)
   {
      modetype = VI_NON_INTERLACE;
      viHeightMultiplier = 2;
   }
   else
   {
      modetype = (gx->tvinfo.progressive) ? VI_PROGRESSIVE : VI_INTERLACE;
      viHeightMultiplier = 1;
   }

   if (fbLines > gx->tvinfo.max_height)
      fbLines = gx->tvinfo.max_height;

   if (fbWidth > gx->tvinfo.max_width)
      fbWidth = gx->tvinfo.max_width;

   gx_mode.viTVMode = VI_TVMODE(gx->tvinfo.tvmode, modetype);
   gx_mode.fbWidth = fbWidth;
   gx_mode.efbHeight = min(fbLines, 480);

   if (modetype == VI_NON_INTERLACE && fbLines > gx->tvinfo.max_height / 2)
      gx_mode.xfbHeight = gx->tvinfo.max_height / 2;
   else if (modetype != VI_NON_INTERLACE && fbLines > gx->tvinfo.max_height)
      gx_mode.xfbHeight = gx->tvinfo.max_height;
   else
      gx_mode.xfbHeight = fbLines;

#ifdef HW_RVL
   gx_mode.viWidth = (CONF_GetAspectRatio() == CONF_ASPECT_16_9) ? 678 : 640;
#else
   gx_mode.viWidth = 640;
#endif
   gx_mode.viHeight = gx_mode.xfbHeight * viHeightMultiplier;
   gx_mode.viXOrigin = (gx->tvinfo.max_width - gx_mode.viWidth) / 2;
   gx_mode.viYOrigin = (gx->tvinfo.max_height - gx_mode.viHeight) / (2 * viHeightMultiplier);
   gx_mode.xfbMode = modetype == VI_INTERLACE ? VI_XFBMODE_DF : VI_XFBMODE_SF;
   gx_mode.field_rendering = GX_FALSE;
   gx_mode.aa = GX_FALSE;

   for (i = 0; i < 12; i++)
      gx_mode.sample_pattern[i][0] = gx_mode.sample_pattern[i][1] = 6;

   if (modetype == VI_INTERLACE)
   {
      gx_mode.vfilter[0] = 8;
      gx_mode.vfilter[1] = 8;
      gx_mode.vfilter[2] = 10;
      gx_mode.vfilter[3] = 12;
      gx_mode.vfilter[4] = 10;
      gx_mode.vfilter[5] = 8;
      gx_mode.vfilter[6] = 8;
   }
   else
   {
      gx_mode.vfilter[0] = 0;
      gx_mode.vfilter[1] = 0;
      gx_mode.vfilter[2] = 21;
      gx_mode.vfilter[3] = 22;
      gx_mode.vfilter[4] = 21;
      gx_mode.vfilter[5] = 0;
      gx_mode.vfilter[6] = 0;
   }

   gx->vp.full_width = gx_mode.fbWidth;
   gx->vp.full_height = gx_mode.xfbHeight;
   gx->double_strike = (modetype == VI_NON_INTERLACE);

   if (rgui)
   {
      rgui->height = gx_mode.efbHeight / (gx->double_strike ? 1 : 2);
      rgui->height &= ~3;
      if (rgui->height > 240)
         rgui->height = 240;

      rgui->width = gx_mode.fbWidth / (gx_mode.fbWidth < 400 ? 1 : 2);
      rgui->width &= ~3;
      if (rgui->width > 400)
         rgui->width = 400;
   }

   GX_SetViewportJitter(0, 0, gx_mode.fbWidth, gx_mode.efbHeight, 0, 1, 1);
   GX_SetDispCopySrc(0, 0, gx_mode.fbWidth, gx_mode.efbHeight);

   f32 y_scale = GX_GetYScaleFactor(gx_mode.efbHeight, gx_mode.xfbHeight);
   u16 xfbWidth = VIDEO_PadFramebufferWidth(gx_mode.fbWidth);
   u16 xfbHeight = GX_SetDispCopyYScale(y_scale);
   GX_SetDispCopyDst(xfbWidth, xfbHeight);

   GX_SetCopyFilter(gx_mode.aa, gx_mode.sample_pattern, (gx_mode.xfbMode == VI_XFBMODE_SF) ? GX_FALSE : GX_TRUE, gx_mode.vfilter);
   GXColor color = { 0, 0, 0, 0xff };
   GX_SetCopyClear(color, GX_MAX_Z24);
   GX_SetFieldMode(gx_mode.field_rendering, (gx_mode.viHeight == 2 * gx_mode.xfbHeight) ? GX_ENABLE : GX_DISABLE);

   /* clear vsync callback */
   VIDEO_SetPreRetraceCallback(NULL);

   VIDEO_SetBlack(true);
   VIDEO_Flush();

   /* wait for next even field, this prevents screen artefacts
    * when switching between interlaced & non-interlaced modes */
   do VIDEO_WaitVSync();
   while (!VIDEO_GetNextField());

   VIDEO_Configure(&gx_mode);
   VIDEO_Flush();
   
   VIDEO_ClearFrameBuffer(&gx_mode, g_fb[0], COLOR_BLACK);
   VIDEO_ClearFrameBuffer(&gx_mode, g_fb[1], COLOR_BLACK);
   g_curfb = 0;
   VIDEO_SetNextFramebuffer(g_fb[g_curfb]);

   /* workaround for the artifacts when 
    * switching between modes 250ms */
   usleep(150000);

   VIDEO_SetBlack(false);
   VIDEO_Flush();

   do VIDEO_WaitVSync();
   while (!VIDEO_GetNextField());
   
   /* restore vsync callback */
   VIDEO_SetPreRetraceCallback(vblank_cb);
}

static const char *gx_get_resolution(unsigned res_index)
{
   static char format[16];
   
   if (res_index == GX_RESOLUTIONS_AUTO)
      return "AUTO";
   
   snprintf(format, sizeof(format), "%.3ux%.3u", gx_resolutions[res_index][0], gx_resolutions[res_index][1]);
   return format;
}

static void gx_get_resolution_size(unsigned res_index, unsigned *width, unsigned *height)
{
    *width  = gx_resolutions[res_index][0];
    *height = gx_resolutions[res_index][1];
}

static void gx_set_aspect_ratio(void *data, unsigned aspect_ratio_idx)
{
   gx_video_t *gx = (gx_video_t*)data;

   if (aspect_ratio_idx == ASPECT_RATIO_SQUARE)
      gfx_set_square_pixel_viewport(g_extern.system.av_info.geometry.base_width, g_extern.system.av_info.geometry.base_height);
   else if (aspect_ratio_idx == ASPECT_RATIO_CORE)
      gfx_set_core_viewport();
   else if (aspect_ratio_idx == ASPECT_RATIO_CONFIG)
      gfx_set_config_viewport();

   gx->aspect_ratio_idx = aspect_ratio_idx;
   gx->aspect_ratio = aspectratio_lut[aspect_ratio_idx].value;
}

static void gx_overlay_enable(void *data, bool state)
{
   gx_video_t *gx = (gx_video_t*)data;
   /* include the state of the system's overlay, 
    * never enable it if it is not set */
   gx->overlay_enable = (!driver.overlay) ? false : state;
}

static void gx_set_rotation(void *data, unsigned orientation)
{
   gx_video_t *gx = (gx_video_t*)data;
   gx->orientation = orientation;
}

static void gx_update_screen_config(void *data, unsigned res_idx, unsigned aspect_idx, bool scale_integer, unsigned orientation, bool show_overlay)
{
   gx_video_t *gx = (gx_video_t*)data;
   static unsigned actual_res_index = GX_RESOLUTIONS_AUTO;
   static unsigned actual_aspect_ratio_index = ASPECT_RATIO_END;

   if (res_idx != actual_res_index)
   {
     gx_set_video_mode(data, res_idx);
     actual_res_index = res_idx;
   }

   if (aspect_idx != actual_aspect_ratio_index)
   {
     gx_set_aspect_ratio(data, aspect_idx);
     actual_aspect_ratio_index = aspect_idx;
   }

   gx_overlay_enable(data, show_overlay);
   gx_set_rotation(data, orientation);

   gx->scale_integer = scale_integer;

   /* reset FPS counting when switching 
    * between menu & game screens */
   g_extern.frame_count = 0;
   g_extern.start_frame_time = gettime();

   /* Apply changes... */
   gx->should_resize = true;
}

static void gx_alloc_textures(void *data, const video_info_t *video)
{
   gx_video_t *gx = (gx_video_t*)data;

   if (game_tex.data) free(game_tex.data);
   game_tex.data = memalign(32, RARCH_SCALE_BASE * RARCH_SCALE_BASE * video->input_scale * video->input_scale * (video->rgb32 ? 4 : 2));

   if (!game_tex.data)
   {
      RARCH_ERR("[GX] Error allocating video texture\n");
      exit(1);
   }

   gx->rgb32 = video->rgb32;
}

static void gx_setup_textures(void *data, unsigned width, unsigned height)
{
   unsigned g_filter, rgui_w, rgui_h;
   gx_video_t *gx = (gx_video_t*)data;

   width &= ~3;
   height &= ~3;
   g_filter = g_settings.video.smooth ? GX_LINEAR : GX_NEAR;
   rgui_w = 320;
   rgui_h = 240;

   if (rgui)
   {
      rgui_w = rgui->width;
      rgui_h = rgui->height;
   }

   GX_InitTexObj(&game_tex.obj, game_tex.data, width, height, (gx->rgb32) ? GX_TF_RGBA8 : GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
   GX_InitTexObjFilterMode(&game_tex.obj, g_filter, g_filter);
   GX_InitTexObj(&menu_tex.obj, menu_tex.data, rgui_w, rgui_h, GX_TF_RGB5A3, GX_CLAMP, GX_CLAMP, GX_FALSE);
   GX_InitTexObjFilterMode(&menu_tex.obj, g_filter, g_filter);
}

static void gx_init_vtx(void *data)
{
   GX_SetCullMode(GX_CULL_NONE);
   GX_SetClipMode(GX_CLIP_DISABLE);
   GX_SetZMode(GX_ENABLE, GX_ALWAYS, GX_ENABLE);
   GX_SetColorUpdate(GX_TRUE);
   GX_SetAlphaUpdate(GX_FALSE);

   Mtx44 m;
   guOrtho(m, 1, -1, -1, 1, 0.4, 0.6);
   GX_LoadProjectionMtx(m, GX_ORTHOGRAPHIC);

   GX_ClearVtxDesc();
   GX_SetVtxDesc(GX_VA_POS, GX_INDEX8);
   GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX8);
   GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX8);

   GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
   GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
   GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
   GX_SetArray(GX_VA_POS, verts, 3 * sizeof(float));
   GX_SetArray(GX_VA_TEX0, vertex_ptr, 2 * sizeof(float));
   GX_SetArray(GX_VA_CLR0, color_ptr, 4 * sizeof(u8));

   GX_SetNumTexGens(1);
   GX_SetNumChans(1);
   GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_NONE, GX_AF_NONE);
   GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
   GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
   GX_InvVtxCache();

   GX_SetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
   GX_Flush();
}

static void build_disp_list(void)
{
   DCInvalidateRange(display_list, sizeof(display_list));
   GX_BeginDispList(display_list, sizeof(display_list));
   GX_Begin(GX_TRIANGLESTRIP, GX_VTXFMT0, 4);
   for (unsigned i = 0; i < 4; i++)
   {
      GX_Position1x8(i);
      GX_Color1x8(i);
      GX_TexCoord1x8(i);
   }
   GX_End();
   display_list_size = GX_EndDispList();
}

static void gx_apply_state_changes(void *data)
{
   (void)data;
#ifdef HW_RVL
   VIDEO_SetTrapFilter(g_extern.console_screen.soft_filter_enable);
#endif
   GX_SetDispCopyGamma(g_extern.console_screen.gamma_correction);
}

static bool gx_init(void **data, const video_info_t *video, const input_driver_t **input, void **input_data)
{
   gx_video_t *gx = (gx_video_t*)*data;
   
   if (!gx) /* setup if no driver data */
   {
      gx = (gx_video_t*)calloc(1, sizeof(gx_video_t));
      if (!gx)
      {
         *data = NULL;
         return false;
      }

      VIDEO_Init();
      GX_Init(gx_fifo, sizeof(gx_fifo));
      g_fb[0] = MEM_K0_TO_K1(memalign(32, 640 * 576 * VI_DISPLAY_PIX_SZ));
      g_fb[1] = MEM_K0_TO_K1(memalign(32, 640 * 576 * VI_DISPLAY_PIX_SZ));
      GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
      gx_get_tvinfo(gx);
      gx_set_video_mode(gx, GX_RESOLUTIONS_AUTO);
      gx_init_vtx(gx);
      build_disp_list();

      /* return the pointer to the driver data */
      *data = (void*)gx;
   }
   
   gx_alloc_textures(gx, video);
   gx_apply_state_changes(gx);
   gx->should_resize = true;

   /* Set input driver to null so 
    * the system set it up separately */
   *input = NULL;
   *input_data = NULL;   
   
   return true;
}

static void update_texture_asm(const uint32_t *src, const uint32_t *dst,
      unsigned width, unsigned height, unsigned pitch)
{
   register uint32_t tmp0, tmp1, tmp2, tmp3, line2, line2b, line3, line3b, line4, line4b, line5;

   asm volatile (
      "     srwi     %[width],   %[width],   2           \n"
      "     srwi     %[height],  %[height],  2           \n"
      "     subi     %[tmp3],    %[dst],     4           \n"
      "     mr       %[dst],     %[tmp3]                 \n"
      "     subi     %[dst],     %[dst],     4           \n"
      "     mr       %[line2],   %[pitch]                \n"
      "     addi     %[line2b],  %[line2],   4           \n"
      "     mulli    %[line3],   %[pitch],   2           \n"
      "     addi     %[line3b],  %[line3],   4           \n"
      "     mulli    %[line4],   %[pitch],   3           \n"
      "     addi     %[line4b],  %[line4],   4           \n"
      "     mulli    %[line5],   %[pitch],   4           \n"

      "2:   mtctr    %[width]                            \n"
      "     mr       %[tmp0],    %[src]                  \n"

      "1:   lwz      %[tmp1],    0(%[src])               \n"
      "     stwu     %[tmp1],    8(%[dst])               \n"
      "     lwz      %[tmp2],    4(%[src])               \n"
      "     stwu     %[tmp2],    8(%[tmp3])              \n"

      "     lwzx     %[tmp1],    %[line2],   %[src]      \n"
      "     stwu     %[tmp1],    8(%[dst])               \n"
      "     lwzx     %[tmp2],    %[line2b],  %[src]      \n"
      "     stwu     %[tmp2],    8(%[tmp3])              \n"

      "     lwzx     %[tmp1],    %[line3],   %[src]      \n"
      "     stwu     %[tmp1],    8(%[dst])               \n"
      "     lwzx     %[tmp2],    %[line3b],  %[src]      \n"
      "     stwu     %[tmp2],    8(%[tmp3])              \n"

      "     lwzx     %[tmp1],    %[line4],   %[src]      \n"
      "     stwu     %[tmp1],    8(%[dst])               \n"
      "     lwzx     %[tmp2],    %[line4b],  %[src]      \n"
      "     stwu     %[tmp2],    8(%[tmp3])              \n"

      "     addi     %[src],     %[src],     8           \n"
      "     bdnz     1b                                  \n"

      "     add      %[src],     %[tmp0],    %[line5]    \n"
      "     subic.   %[height],  %[height],  1           \n"
      "     bne      2b                                  \n"
      :  [tmp0]   "=&b" (tmp0),
         [tmp1]   "=&b" (tmp1),
         [tmp2]   "=&b" (tmp2),
         [tmp3]   "=&b" (tmp3),
         [line2]  "=&b" (line2),
         [line2b] "=&b" (line2b),
         [line3]  "=&b" (line3),
         [line3b] "=&b" (line3b),
         [line4]  "=&b" (line4),
         [line4b] "=&b" (line4b),
         [line5]  "=&b" (line5),
         [dst]    "+&b"  (dst)
      :  [src]    "b"   (src),
         [width]  "b"   (width),
         [height] "b"   (height),
         [pitch]  "b"   (pitch)
      :  "cc"
   );
}

#define BLIT_LINE_16(off) \
{ \
   const uint32_t *tmp_src = src; \
   uint32_t *tmp_dst = dst; \
   for (unsigned x = 0; x < width2 >> 1; x++, tmp_src += 2, tmp_dst += 8) \
   { \
      tmp_dst[ 0 + off] = BLIT_LINE_16_CONV(tmp_src[0]); \
      tmp_dst[ 1 + off] = BLIT_LINE_16_CONV(tmp_src[1]); \
   } \
   src += tmp_pitch; \
}

#define BLIT_LINE_32(off) \
{ \
   const uint16_t *tmp_src = src; \
   uint16_t *tmp_dst = dst; \
   for (unsigned x = 0; x < width2 >> 3; x++, tmp_src += 8, tmp_dst += 32) \
   { \
      tmp_dst[  0 + off] = tmp_src[0] | 0xFF00; \
      tmp_dst[ 16 + off] = tmp_src[1]; \
      tmp_dst[  1 + off] = tmp_src[2] | 0xFF00; \
      tmp_dst[ 17 + off] = tmp_src[3]; \
      tmp_dst[  2 + off] = tmp_src[4] | 0xFF00; \
      tmp_dst[ 18 + off] = tmp_src[5]; \
      tmp_dst[  3 + off] = tmp_src[6] | 0xFF00; \
      tmp_dst[ 19 + off] = tmp_src[7]; \
   } \
   src += tmp_pitch; \
}

static void convert_texture16(const uint32_t *_src, uint32_t *_dst,
      unsigned width, unsigned height, unsigned pitch)
{
   width &= ~3;
   height &= ~3;
   update_texture_asm(_src, _dst, width, height, pitch);
}

static void convert_texture32(const uint32_t *_src, uint32_t *_dst,
      unsigned width, unsigned height, unsigned pitch)
{
   width &= ~3;
   height &= ~3;
   unsigned tmp_pitch = pitch >> 1;
   unsigned width2 = width << 1;

   const uint16_t *src = (uint16_t *) _src;
   uint16_t *dst = (uint16_t *) _dst;
   for (unsigned i = 0; i < height; i += 4, dst += 4 * width2)
   {
      BLIT_LINE_32(0)
      BLIT_LINE_32(4)
      BLIT_LINE_32(8)
      BLIT_LINE_32(12)
   }
}

static void gx_resize_viewport(void *data)
{
   gx_video_t *gx = (gx_video_t*)data;
   
   /* Default to full screen size. */
   gx->vp.x      = 0;
   gx->vp.y      = 0;
   gx->vp.width  = gx->vp.full_width;
   gx->vp.height = gx->vp.full_height;

   if (gx->scale_integer)
   {
      gfx_scale_integer(&gx->vp, gx->vp.width, gx->vp.height, 
                       (gx->double_strike ? 0.0f : gx->aspect_ratio), 
                        true);
   }
   else if (gx->aspect_ratio_idx == ASPECT_RATIO_CUSTOM)
   {
      if (!g_extern.console_screen.custom_vp.width || !g_extern.console_screen.custom_vp.height)
      {
         g_extern.console_screen.custom_vp.x = 0;
         g_extern.console_screen.custom_vp.y = 0;
         g_extern.console_screen.custom_vp.width = gx->vp.full_width;
         g_extern.console_screen.custom_vp.height = gx->vp.full_height;
      }

      gx->vp.x      = g_extern.console_screen.custom_vp.x;
      gx->vp.y      = g_extern.console_screen.custom_vp.y;
      gx->vp.width  = g_extern.console_screen.custom_vp.width;
      gx->vp.height = g_extern.console_screen.custom_vp.height;
   }
   else if (!gx->double_strike) /* Apply aspect ratio just to non-240p resolutions */
   {
      float delta;
      float desired_aspect = gx->aspect_ratio > 0.0f ? gx->aspect_ratio : 1.3333f; // 4:3
#ifdef HW_RVL
      float device_aspect = CONF_GetAspectRatio() == CONF_ASPECT_4_3 ? 4.0 / 3.0 : 16.0 / 9.0;
#else
      float device_aspect = 4.0 / 3.0;
#endif
      if (gx->orientation == ORIENTATION_VERTICAL || gx->orientation == ORIENTATION_FLIPPED_ROTATED)
         desired_aspect = 1.0 / desired_aspect;

      if (fabs(device_aspect - desired_aspect) < 0.0001)
      {
        /* If the aspect ratios of screen and desired aspect ratio are 
           sufficiently equal (floating point stuff),
           assume they are actually equal. */
      }
      else if (device_aspect > desired_aspect)
      {
         delta        = (desired_aspect / device_aspect - 1.0) / 2.0 + 0.5;
         gx->vp.x     = (unsigned)(gx->vp.width * (0.5 - delta));
         gx->vp.width = (unsigned)(2.0 * gx->vp.width * delta);
      }
      else
      {
         delta         = (device_aspect / desired_aspect - 1.0) / 2.0 + 0.5;
         gx->vp.y      = (unsigned)(gx->vp.height * (0.5 - delta));
         gx->vp.height = (unsigned)(2.0 * gx->vp.height * delta);
      }
   }

   GX_SetViewportJitter(gx->vp.x, gx->vp.y, gx->vp.width, gx->vp.height, 0, 1, 1);

   Mtx44 m1, m2;
   float top = 1, bottom = -1, left = -1, right = 1;

   guOrtho(m1, top, bottom, left, right, 0, 1);
   GX_LoadPosMtxImm(m1, GX_PNMTX1);

   unsigned degrees;
   switch(gx->orientation)
   {
      case ORIENTATION_VERTICAL:
         degrees = 90;
         break;
      case ORIENTATION_FLIPPED:
         degrees = 180;
         break;
      case ORIENTATION_FLIPPED_ROTATED:
         degrees = 270;
         break;
      default:
         degrees = 0;
         break;
   }
   
   guMtxIdentity(m2);
   guMtxRotDeg(m2, 'Z', degrees);
   guMtxConcat(m1, m2, m1);
   GX_LoadPosMtxImm(m1, GX_PNMTX0);

   gx->should_resize = false;
}

static void gx_blit_line(void *data, unsigned x, unsigned y, const char *message)
{
   gx_video_t *gx = (gx_video_t*)data;

   const GXColor c = {
      .r = 0xff,
      .g = 0xff,
      .b = 0xff,
      .a = 0x00
   };

   if (!*message)
      return;

   bool double_width = gx->vp.full_width > 400;
   unsigned width = (double_width ? 2 : 1);

   while (*message)
   {
      for (unsigned j = 0; j < FONT_HEIGHT; j++)
      {
         for (unsigned i = 0; i < FONT_WIDTH; i++)
         {
            uint8_t rem = 1 << ((i + j * FONT_WIDTH) & 7);
            unsigned offset = (i + j * FONT_WIDTH) >> 3;

            /* if blank pixel, skip it */
            if (!(bitmap_bin[FONT_OFFSET((unsigned char) *message) + offset] & rem))
               continue;

            if (gx->double_strike)
            {
               GX_PokeARGB(x + (i * width),     y + j, c);
               if (double_width)
               {
                  GX_PokeARGB(x + (i * width) + 1, y + j, c);
               }
            }
            else
            {
               GX_PokeARGB(x + (i * width),     y + (j * 2),     c);
               if (double_width)
               {
                  GX_PokeARGB(x + (i * width) + 1, y + (j * 2),     c);
                  GX_PokeARGB(x + (i * width) + 1, y + (j * 2) + 1, c);
               }
               GX_PokeARGB(x + (i * width),     y + (j * 2) + 1, c);
            }
         }
      }

      x += FONT_WIDTH_STRIDE * (double_width ? 2 : 1);
      message++;
   }
}

#ifdef HAVE_OVERLAY
static void gx_overlay_tex_geom(void *data, unsigned image, float x, float y, float w, float h)
{
   gx_video_t *gx = (gx_video_t*)data;
   struct gx_overlay_data *o = &gx->overlay[image];

   o->tex_coord[0] = x;     o->tex_coord[1] = y;
   o->tex_coord[2] = x + w; o->tex_coord[3] = y;
   o->tex_coord[4] = x;     o->tex_coord[5] = y + h;
   o->tex_coord[6] = x + w; o->tex_coord[7] = y + h;
}

static void gx_overlay_vertex_geom(void *data, unsigned image, float x, float y, float w, float h)
{
   gx_video_t *gx = (gx_video_t*)data;
   struct gx_overlay_data *o = &gx->overlay[image];

   // Flipped, so we preserve top-down semantics.
   y = 1.0f - y;
   h = -h;

   // expand from 0 - 1 to -1 - 1
   x = (x * 2.0f) - 1.0f;
   y = (y * 2.0f) - 1.0f;
   w = (w * 2.0f);
   h = (h * 2.0f);

   o->vertex_coord[0] = x;     o->vertex_coord[1] = y;
   o->vertex_coord[2] = x + w; o->vertex_coord[3] = y;
   o->vertex_coord[4] = x;     o->vertex_coord[5] = y + h;
   o->vertex_coord[6] = x + w; o->vertex_coord[7] = y + h;
}

static void gx_overlay_free(void *data)
{
   gx_video_t *gx = (gx_video_t*)data;
   if (gx->overlay)
   { 
      free(gx->overlay);
      gx->overlay = NULL;
   }
   gx->overlays = 0;
}

static bool gx_overlay_load(void *data, const struct texture_image *images, unsigned num_images)
{
   unsigned i, g_filter;
   gx_video_t *gx = (gx_video_t*)data;

   gx_overlay_free(data);
   gx->overlay = (struct gx_overlay_data*)calloc(num_images, sizeof(*gx->overlay));
   if (!gx->overlay)
      return false;

   gx->overlays = num_images;
   g_filter = g_settings.video.smooth ? GX_LINEAR : GX_NEAR;

   for (i = 0; i < num_images; i++)
   {
      struct gx_overlay_data *o = &gx->overlay[i];
      GX_InitTexObj(&o->tex, images[i].pixels, images[i].width, images[i].height, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
      GX_InitTexObjFilterMode(&o->tex, g_filter, g_filter);
      DCFlushRange(images[i].pixels, images[i].width * images[i].height * sizeof(uint32_t));
      gx_overlay_tex_geom(gx, i, 0, 0, 1, 1); // Default. Stretch to whole screen.
      gx_overlay_vertex_geom(gx, i, 0, 0, 1, 1);
      gx->overlay[i].alpha_mod = 1.0f;
   }

   return true;
}

static void gx_overlay_set_alpha(void *data, unsigned image, float mod)
{
   gx_video_t *gx = (gx_video_t*)data;
   gx->overlay[image].alpha_mod = mod;
}

static void gx_overlay_render(void *data)
{
   gx_video_t *gx = (gx_video_t*)data;

   /* Only render if an overlay is available */
   if (!gx->overlay) return;

   GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
   GX_SetCurrentMtx(GX_PNMTX1);
   GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
   GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
   GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

   for (unsigned i = 0; i < gx->overlays; i++)
   {
      GX_LoadTexObj(&gx->overlay[i].tex, GX_TEXMAP0);

      GX_Begin(GX_TRIANGLESTRIP, GX_VTXFMT0, 4);
      GX_Position3f32(gx->overlay[i].vertex_coord[0], gx->overlay[i].vertex_coord[1],  -0.5);
      GX_Color4u8(255, 255, 255, (u8)(gx->overlay[i].alpha_mod * 255.0f));
      GX_TexCoord2f32(gx->overlay[i].tex_coord[0], gx->overlay[i].tex_coord[1]);

      GX_Position3f32(gx->overlay[i].vertex_coord[2], gx->overlay[i].vertex_coord[3],  -0.5);
      GX_Color4u8(255, 255, 255, (u8)(gx->overlay[i].alpha_mod * 255.0f));
      GX_TexCoord2f32(gx->overlay[i].tex_coord[2], gx->overlay[i].tex_coord[3]);

      GX_Position3f32(gx->overlay[i].vertex_coord[4], gx->overlay[i].vertex_coord[5],  -0.5);
      GX_Color4u8(255, 255, 255, (u8)(gx->overlay[i].alpha_mod * 255.0f));
      GX_TexCoord2f32(gx->overlay[i].tex_coord[4], gx->overlay[i].tex_coord[5]);

      GX_Position3f32(gx->overlay[i].vertex_coord[6], gx->overlay[i].vertex_coord[7],  -0.5);
      GX_Color4u8(255, 255, 255, (u8)(gx->overlay[i].alpha_mod * 255.0f));
      GX_TexCoord2f32(gx->overlay[i].tex_coord[6], gx->overlay[i].tex_coord[7]);
      GX_End();
   }
   
   GX_SetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
   GX_SetVtxDesc(GX_VA_POS, GX_INDEX8);
   GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX8);
   GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX8);
}

static void gx_overlay_full_screen(void *data, bool enable)
{
   (void)data;
   (void)enable;
}

static const video_overlay_interface_t gx_overlay_interface = {
   gx_overlay_enable,
   gx_overlay_load,
   gx_overlay_tex_geom,
   gx_overlay_vertex_geom,
   gx_overlay_full_screen,
   gx_overlay_set_alpha,
   gx_overlay_free
};

static void gx_get_overlay_interface(void *data, const video_overlay_interface_t **iface)
{
   (void)data;
   *iface = &gx_overlay_interface;
}
#endif

static inline void gx_onscreen_display(gx_video_t *gx, const char *msg)
{
   g_extern.frame_count++;

   if (!gx->rgui_texture_enable) /* only show in-game */
   {
      if (msg || g_settings.fps_show)
      {
         const char *tmp;
         unsigned x = 8 * (gx->double_strike ? 1 : 2);
         unsigned y = gx->vp.full_height - (22 * (gx->double_strike ? 1 : 2));

         if (msg)
            tmp = msg;
         else
         {
            static char fps_txt[16];
            gfx_get_fps(NULL, 0, fps_txt, sizeof(fps_txt));
            tmp = fps_txt;
         }
         
         gx_blit_line(gx, x, y, tmp);
      }
   }
}

static bool gx_frame(void *data, const void *frame,
      unsigned width, unsigned height, unsigned pitch,
      const char *msg)
{
   gx_video_t *gx = (gx_video_t*)data;
   
   if (!frame && !gx->rgui_texture_enable)
      return true;

   if (gx->should_resize)
   {
      gx_setup_textures(data, width, height);
      gx_resize_viewport(gx);
      
      /* when returning from 
       * menu force to sync */
      g_vsync = g_vsync_state;
   }
   
   /* Load menu if enabled */
   if (gx->rgui_texture_enable && gx->menu_data)
   {
      convert_texture16(gx->menu_data, menu_tex.data, rgui->width, rgui->height, rgui->width * 2);
      DCStoreRange(menu_tex.data, rgui->width * rgui->height * 2);
      GX_SetCurrentMtx(GX_PNMTX1);
      GX_LoadTexObj(&menu_tex.obj, GX_TEXMAP0);
   }
   else if (frame) /* Load the frame if any */
   {
      if (gx->rgb32)
         convert_texture32(frame, game_tex.data, width, height, pitch);
      else
         convert_texture16(frame, game_tex.data, width, height, pitch);
      DCStoreRange(game_tex.data, height * (width << (gx->rgb32 ? 2 : 1)));
      GX_SetCurrentMtx(GX_PNMTX0);
      GX_LoadTexObj(&game_tex.obj, GX_TEXMAP0);
   }

   GX_InvalidateTexAll();
   GX_CallDispList(display_list, display_list_size);

#ifdef HAVE_OVERLAY
   if (gx->overlay_enable)
      gx_overlay_render(gx);
#endif

   GX_DrawDone();

   /* OSD */
   gx_onscreen_display(gx, msg);
   
   /* wait vertical sync */
   while (g_vsync == WAIT_VBLANK);
   g_vsync = g_vsync_state;

   GX_CopyDisp(g_fb[g_curfb], GX_TRUE);
   GX_Flush();
   VIDEO_SetNextFramebuffer(g_fb[g_curfb]);
   VIDEO_Flush();
   
   g_curfb ^= 1;

   return true;
}

static void gx_set_nonblock_state(void *data, bool state)
{
   (void)data;
   g_vsync_state = !state;
}

static bool gx_alive(void *data)
{
   (void)data;
   return true;
}

static bool gx_focus(void *data)
{
   (void)data;
   return true;
}

static void gx_free(void *data)
{
   /* game screen texture */
   if (game_tex.data)
      free(game_tex.data);

   /* screen xfb buffers */
   if (g_fb[0])
   {
      free(g_fb[0]);
      free(g_fb[1]);
   }

   /* video driver data */
   if (data)
      free(data);
}

static void gx_set_texture_frame(void *data, const void *frame,
      bool rgb32, unsigned width, unsigned height, float alpha)
{
   (void)rgb32;
   (void)width;
   (void)height;
   (void)alpha;

   gx_video_t *gx = (gx_video_t*)data;
   gx->menu_data = (uint32_t*)frame;
}

static void gx_set_texture_enable(void *data, bool enable, bool full_screen)
{
   (void)full_screen;
   gx_video_t *gx = (gx_video_t*)data;
   gx->rgui_texture_enable = enable;
}

static void gx_viewport_info(void *data, struct rarch_viewport *vp)
{
   gx_video_t *gx = (gx_video_t*)data;
   *vp = gx->vp;
}

static void gx_set_filtering(void *data, unsigned index, bool smooth)
{
    gx_video_t *gx = (gx_video_t*)data;
    (void)index;
    (void)smooth;
    
    //It is applied on the resize method, so we just ensure it is called
    gx->should_resize = true;
}

static const video_poke_interface_t gx_poke_interface = {
   gx_set_filtering,
   gx_set_aspect_ratio,
   gx_apply_state_changes,
   gx_set_texture_frame,
   gx_set_texture_enable,
   NULL,
   NULL,
   NULL,
   gx_update_screen_config,
   gx_get_resolution,
   gx_get_resolution_size,
   gx_set_refresh_rate
};

static void gx_get_poke_interface(void *data, const video_poke_interface_t **iface)
{
   (void)data;
   *iface = &gx_poke_interface;
}

const video_driver_t video_gx = {
   .init = gx_init,
   .frame = gx_frame,
   .alive = gx_alive,
   .set_nonblock_state = gx_set_nonblock_state,
   .focus = gx_focus,
   .free = gx_free,
   .ident = "gx",
   .set_rotation = gx_set_rotation,
   .viewport_info = gx_viewport_info,
#ifdef HAVE_OVERLAY
   .overlay_interface = gx_get_overlay_interface,
#endif
   .poke_interface = gx_get_poke_interface,
};
