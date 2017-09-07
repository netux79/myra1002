/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2013-2014 - Jason Fetters
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

#import "RetroArch_Apple.h"
#include "../common/rarch_wrapper.h"

#include "../../general.h"
#include "../../gfx/gfx_common.h"
#include "../../gfx/gfx_context.h"

// Define compatibility symbols and categories
#ifdef IOS

#define APP_HAS_FOCUS ([[UIApplication sharedApplication] applicationState] == UIApplicationStateActive)

#define GLContextClass EAGLContext
#define GLAPIType GFX_CTX_OPENGL_ES_API
#define GLFrameworkID CFSTR("com.apple.opengles")
#define RAScreen UIScreen

@interface EAGLContext (OSXCompat) @end
@implementation EAGLContext (OSXCompat)
+ (void)clearCurrentContext { [EAGLContext setCurrentContext:nil];  }
- (void)makeCurrentContext  { [EAGLContext setCurrentContext:self]; }
@end

#elif defined(OSX)
#define APP_HAS_FOCUS ([NSApp isActive])

#define GLContextClass NSOpenGLContext
#define GLAPIType GFX_CTX_OPENGL_API
#define GLFrameworkID CFSTR("com.apple.opengl")
#define RAScreen NSScreen

#define g_view g_instance // < RAGameView is a container on iOS; on OSX these are both the same object

@interface NSScreen (IOSCompat) @end
@implementation NSScreen (IOSCompat)
- (CGRect)bounds
{
	CGRect cgrect  = NSRectToCGRect([self frame]);
	return CGRectMake(0, 0, CGRectGetWidth(cgrect), CGRectGetHeight(cgrect));
}
- (float) scale  { return 1.0f; }
@end

#endif

#ifdef IOS

#include <GLKit/GLKit.h>
#import "views.h"
static const float ALMOST_INVISIBLE = .021f;
static GLKView* g_view;
static UIView* g_pause_indicator_view;

#elif defined(OSX)

#include "apple_input.h"

static bool g_has_went_fullscreen;
static NSOpenGLPixelFormat* g_format;

#endif

static bool g_initialized;
static RAGameView* g_instance;
static GLContextClass* g_context;

static int g_fast_forward_skips;
static bool g_is_syncing = true;


@implementation RAGameView
+ (RAGameView*)get
{
   if (!g_instance)
      g_instance = [RAGameView new];
   
   return g_instance;
}

#ifdef OSX

- (id)init
{
   self = [super init];
   [self setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
   return self;
}

- (void)setFrame:(NSRect)frameRect
{
   [super setFrame:frameRect];

   if (g_view && g_context)
      [g_context update];
}

- (void)display
{
   [g_context flushBuffer];
}

// Stop the annoying sound when pressing a key
- (BOOL)acceptsFirstResponder
{
   return YES;
}

- (BOOL)isFlipped
{
   return YES;
}

- (void)keyDown:(NSEvent*)theEvent
{
}

#elif defined(IOS)
// < iOS Pause menu and lifecycle
- (id)init
{
   self = [super init];

   UINib *xib = [UINib nibWithNibName:@"PauseIndicatorView" bundle:nil];
   g_pause_indicator_view = [[xib instantiateWithOwner:[RetroArch_iOS get] options:nil] lastObject];

   g_view = [GLKView new];
   g_view.multipleTouchEnabled = YES;
   g_view.enableSetNeedsDisplay = NO;
   [g_view addSubview:g_pause_indicator_view];

   self.view = g_view;
   
   [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(showPauseIndicator) name:UIApplicationWillEnterForegroundNotification object:nil];
   return self;
}

// Pause Menus
- (void)viewDidAppear:(BOOL)animated
{
   [self showPauseIndicator];
}

- (void)showPauseIndicator
{
   g_pause_indicator_view.alpha = 1.0f;
   [NSObject cancelPreviousPerformRequestsWithTarget:g_instance];
   [g_instance performSelector:@selector(hidePauseButton) withObject:g_instance afterDelay:3.0f];
}

- (void)viewWillLayoutSubviews
{
   UIInterfaceOrientation orientation = self.interfaceOrientation;
   CGRect screenSize = [[UIScreen mainScreen] bounds];
   
   const float width = ((int)orientation < 3) ? CGRectGetWidth(screenSize) : CGRectGetHeight(screenSize);
   const float height = ((int)orientation < 3) ? CGRectGetHeight(screenSize) : CGRectGetWidth(screenSize);

   float tenpctw = width / 10.0f;
   float tenpcth = height / 10.0f;
   
   g_pause_indicator_view.frame = CGRectMake(tenpctw * 4.0f, 0.0f, tenpctw * 2.0f, tenpcth);
   [g_pause_indicator_view viewWithTag:1].frame = CGRectMake(0, 0, tenpctw * 2.0f, tenpcth);
}

- (void)hidePauseButton
{
   [UIView animateWithDuration:0.2
      animations:^{ g_pause_indicator_view.alpha = ALMOST_INVISIBLE; }
      completion:^(BOOL finished) { }
   ];
}

// NOTE: This version only runs on iOS6
- (NSUInteger)supportedInterfaceOrientations
{
   return apple_frontend_settings.orientation_flags;
}

// NOTE: This version runs on iOS2-iOS5, but not iOS6
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
   switch (interfaceOrientation)
   {
      case UIInterfaceOrientationPortrait:
         return (apple_frontend_settings.orientation_flags & UIInterfaceOrientationMaskPortrait);
      case UIInterfaceOrientationPortraitUpsideDown:
         return (apple_frontend_settings.orientation_flags & UIInterfaceOrientationMaskPortraitUpsideDown);
      case UIInterfaceOrientationLandscapeLeft:
         return (apple_frontend_settings.orientation_flags & UIInterfaceOrientationMaskLandscapeLeft);
      case UIInterfaceOrientationLandscapeRight:
         return (apple_frontend_settings.orientation_flags & UIInterfaceOrientationMaskLandscapeRight);
   }
   
   return YES;
}

#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

#endif

@end

static RAScreen* get_chosen_screen(void)
{
#if defined(OSX) && !defined(MAC_OS_X_VERSION_10_6)
#else
   if (g_settings.video.monitor_index >= RAScreen.screens.count)
   {
      RARCH_WARN("video_monitor_index is greater than the number of connected monitors; using main screen instead.\n");
      return [RAScreen mainScreen];
   }
#endif
	
   NSArray *screens = [RAScreen screens];
   return (RAScreen*)[screens objectAtIndex:g_settings.video.monitor_index];
}

bool apple_gfx_ctx_init(void *data)
{
   (void)data;
   // Make sure the view was created
   [RAGameView get];
   g_initialized = true;
   return true;
}

void apple_gfx_ctx_destroy(void *data)
{
   (void)data;
   g_initialized = false;

   [GLContextClass clearCurrentContext];

#ifdef IOS
   g_view.context = nil;
#endif
   [GLContextClass clearCurrentContext];
   g_context = nil;
}

bool apple_gfx_ctx_bind_api(void *data, enum gfx_ctx_api api, unsigned major, unsigned minor)
{
   (void)data;
   if (api != GLAPIType)
      return false;

   [GLContextClass clearCurrentContext];

#ifdef OSX
   [g_context clearDrawable];
   [g_context release], g_context = nil;
   [g_format release], g_format = nil;

   NSOpenGLPixelFormatAttribute attributes [] = {
      NSOpenGLPFADoubleBuffer,	// double buffered
      NSOpenGLPFADepthSize,
     (NSOpenGLPixelFormatAttribute)16, // 16 bit depth buffer
#ifdef MAC_OS_X_VERSION_10_7
     (major || minor) ? NSOpenGLPFAOpenGLProfile : 0,
     (major << 12) | (minor << 8),
#endif
      (NSOpenGLPixelFormatAttribute)nil
   };

   [g_format release];
   [g_context release];

   g_format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
   g_context = [[NSOpenGLContext alloc] initWithFormat:g_format shareContext:nil];
   [g_context setView:g_view];
#else
   g_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
   g_view.context = g_context;
#endif

   [g_context makeCurrentContext];
   return true;
}

void apple_gfx_ctx_swap_interval(void *data, unsigned interval)
{
   (void)data;
#ifdef IOS // < No way to disable Vsync on iOS?
           //   Just skip presents so fast forward still works.
   g_is_syncing = interval ? true : false;
   g_fast_forward_skips = interval ? 0 : 3;
#elif defined(OSX)
   GLint value = interval ? 1 : 0;
   [g_context setValues:&value forParameter:NSOpenGLCPSwapInterval];
#endif
}

bool apple_gfx_ctx_set_video_mode(void *data, unsigned width, unsigned height, bool fullscreen)
{
   (void)data;
#ifdef OSX
   // TODO: Sceen mode support
   
   if (fullscreen && !g_has_went_fullscreen)
   {
      [g_view enterFullScreenMode:get_chosen_screen() withOptions:nil];
      [NSCursor hide];
   }
   else if (!fullscreen && g_has_went_fullscreen)
   {
      [g_view exitFullScreenModeWithOptions:nil];
      [[g_view window] makeFirstResponder:g_view];
      [NSCursor unhide];
   }
   
   g_has_went_fullscreen = fullscreen;
   if (!g_has_went_fullscreen)
      [[g_view window] setContentSize:NSMakeSize(width, height)];
#endif

   // TODO: Maybe iOS users should be apple to show/hide the status bar here?

   return true;
}

void apple_gfx_ctx_get_video_size(void *data, unsigned* width, unsigned* height)
{
   (void)data;
   RAScreen* screen = get_chosen_screen();
   CGRect size;
	
   if (g_initialized)
   {
#if defined(OSX)
      CGRect cgrect = NSRectToCGRect([g_view frame]);
      size = CGRectMake(0, 0, CGRectGetWidth(cgrect), CGRectGetHeight(cgrect));
#else
      size = [g_view bounds];
#endif
   }
   else
      size = [screen bounds];


   *width  = CGRectGetWidth(size)  * [screen scale];
   *height = CGRectGetHeight(size) * [screen scale];
}

void apple_gfx_ctx_update_window_title(void *data)
{
   (void)data;
   static char buf[128], buf_fps[128];
   bool fps_draw = g_settings.fps_show;
   bool got_text = gfx_get_fps(buf, sizeof(buf), fps_draw ? buf_fps : NULL, sizeof(buf_fps));
   static const char* const text = buf; // < Can't access buf directly in the block
    (void)got_text;
    (void)text;
#ifdef OSX
   if (got_text)
       [[g_view window] setTitle:[NSString stringWithCString:text encoding:NSUTF8StringEncoding]];
#endif
   if (fps_draw)
      msg_queue_push(g_extern.msg_queue, buf_fps, 1, 1);
}

bool apple_gfx_ctx_has_focus(void *data)
{
   (void)data;
   return APP_HAS_FOCUS;
}

void apple_gfx_ctx_swap_buffers(void *data)
{
   (void)data;
   bool swap = --g_fast_forward_skips < 0;

   if (!swap)
      return;

   [g_view display];
   g_fast_forward_skips = g_is_syncing ? 0 : 3;
}

gfx_ctx_proc_t apple_gfx_ctx_get_proc_address(const char *symbol_name)
{
#ifdef MAC_OS_X_VERSION_10_7
   return (gfx_ctx_proc_t)CFBundleGetFunctionPointerForName(CFBundleGetBundleWithIdentifier(GLFrameworkID),
                                                            (__bridge CFStringRef)BOXSTRING(symbol_name));
#else
	return (gfx_ctx_proc_t)CFBundleGetFunctionPointerForName(CFBundleGetBundleWithIdentifier(GLFrameworkID),
															 (CFStringRef)BOXSTRING(symbol_name));
#endif
}

#ifdef IOS
void apple_bind_game_view_fbo(void)
{
   if (g_context)
      [g_view bindDrawable];
}

#endif
