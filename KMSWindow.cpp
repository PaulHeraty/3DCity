/*
Based on KMSCube.c 
Ref: https://github.com/robclark/kmscube/blob/master/kmscube.c

Minimal version - no input support - just gfx enablement
*/

#include "KMSWindow.h"

#ifdef SPACENAV
#include "spnav_ctrl.h"
#endif

#ifdef USE_PERFQ
#include "PerfQueries.h"
PerfQuery *g_PQ = NULL;
#endif
/*****************************************************/
/********************* KMS ***************************/
/*****************************************************/

struct kms {
   drmModeConnector *connector;
   drmModeEncoder *encoder;
   drmModeModeInfo mode;
   uint32_t fb_id;
};


struct drm_fb {
	struct gbm_bo *bo;
	uint32_t fb_id;
};



fd_set fds;
static const char device_name[] = "/dev/dri/card0";
uint32_t handle, stride;
struct kms kms;
int ret, fd;
struct gbm_device *gbm;
struct gbm_bo *bo;
drmModeCrtcPtr saved_crtc;
struct gbm_surface *gs;
int full_modes = 0;

static void
drm_fb_destroy_callback(struct gbm_bo *bo, void *data)
{
	struct drm_fb *fb = (struct drm_fb *)data;
	struct gbm_device *gbm = gbm_bo_get_device(bo);

	if (fb->fb_id)
		drmModeRmFB(fd, fb->fb_id);

	free(fb);
}

static struct drm_fb * drm_fb_get_from_bo(struct gbm_bo *bo)
{
	struct drm_fb *fb = (struct drm_fb *)gbm_bo_get_user_data(bo);
	uint32_t width, height, stride, handle;
	int ret;

	if (fb)
		return fb;

	fb = (struct drm_fb *) calloc(1, sizeof *fb);
	fb->bo = bo;

	width = gbm_bo_get_width(bo);
	height = gbm_bo_get_height(bo);
	stride = gbm_bo_get_stride(bo);
	handle = gbm_bo_get_handle(bo).u32;

	ret = drmModeAddFB(fd, width, height, 24, 32, stride, handle, &fb->fb_id);
	if (ret) {
		printf("failed to create fb: %s\n", strerror(errno));
		free(fb);
		return NULL;
	}

	gbm_bo_set_user_data(bo, fb, drm_fb_destroy_callback);

	return fb;
}

static void page_flip_handler(int fd, unsigned int frame,
		  unsigned int sec, unsigned int usec, void *data)
{
	int *waiting_for_flip = (int*)data;
	*waiting_for_flip = 0;
}


int printMode(int a, struct drm_mode_modeinfo *mode)
{
        if (full_modes) {
                printf("Mode: %s\n", mode->name);
                printf("\tclock       : %i\n", mode->clock);
                printf("\thdisplay    : %i\n", mode->hdisplay);
                printf("\thsync_start : %i\n", mode->hsync_start);
                printf("\thsync_end   : %i\n", mode->hsync_end);
                printf("\thtotal      : %i\n", mode->htotal);
                printf("\thskew       : %i\n", mode->hskew);
                printf("\tvdisplay    : %i\n", mode->vdisplay);
                printf("\tvsync_start : %i\n", mode->vsync_start);
                printf("\tvsync_end   : %i\n", mode->vsync_end);
                printf("\tvtotal      : %i\n", mode->vtotal);
                printf("\tvscan       : %i\n", mode->vscan);
                printf("\tvrefresh    : %i\n", mode->vrefresh);
                printf("\tflags       : %i\n", mode->flags);
        } else {
                printf("%3d Mode: \"%s\" %ix%i %i\n", a, mode->name,
                                mode->hdisplay, mode->vdisplay, mode->vrefresh);
        }
        return 0;
}


EGLBoolean setup_kms(int fd, struct kms *kms)
{
   struct drm_mode_modeinfo *mode = NULL;
   drmModeRes *resources;
   drmModeConnector *connector;
   drmModeEncoder *encoder;
   int i;
   int selected_mode = -1;
   resources = drmModeGetResources(fd);
   if (!resources) {
      fprintf(stderr, "drmModeGetResources failed\n");
      return EGL_FALSE;
   }

   for (i = 0; i < resources->count_connectors; i++) {
      connector = drmModeGetConnector(fd, resources->connectors[i]);
      if (connector == NULL)
         continue;
      if (connector->connection == DRM_MODE_CONNECTED &&
          connector->count_modes > 0)
         break;
      drmModeFreeConnector(connector);
   }
#if 1
     for (i = 0; i < connector->count_modes; i++) {
                        mode = (struct drm_mode_modeinfo *)&connector->modes[i];
                        printMode(i, mode);
#if 0
//if ((mode->hdisplay == 1024) && (mode->vdisplay == 768)) {
if ((mode->hdisplay == 800) && (mode->vdisplay == 600)) {
if (selected_mode == -1) {
selected_mode = i;
printf(" * Selected\n");
}
}
#endif
    }
#endif
if (selected_mode == -1) selected_mode = 0;
   if (i == resources->count_connectors) {
      fprintf(stderr, "No currently active connector found.\n");
      return EGL_FALSE;
   }
   printf("resources->count_encoders=%d \n", resources->count_encoders);
   for (i = 0; i < resources->count_encoders; i++) {
      encoder = drmModeGetEncoder(fd, resources->encoders[i]);
      if (encoder == NULL)
         continue;
      if (encoder->encoder_id == connector->encoder_id)
         break;
      drmModeFreeEncoder(encoder);
   }
   kms->connector = connector;
   kms->encoder = encoder;
   kms->mode = connector->modes[selected_mode];
   return EGL_TRUE;
}



EGLint config_attribs[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_BUFFER_SIZE, 0x20,      // 32bit
    EGL_RED_SIZE, 0x8,
    EGL_GREEN_SIZE, 0x8,
    EGL_BLUE_SIZE, 0x8,
    EGL_ALPHA_SIZE, 0x8,
    EGL_DEPTH_SIZE, 0x18,       // 24bit
    //EGL_STENCIL_SIZE, 0x8,
    EGL_STENCIL_SIZE, 0x8,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    //EGL_SAMPLES, 0x4,
    EGL_SAMPLES, MSAA_FACTOR,
    //EGL_SAMPLE_BUFFERS, 0x1,
    EGL_NONE
};

EGLint context_attribs[] = {
    EGL_CONTEXT_CLIENT_VERSION, 0x2,
    EGL_NONE
};

EGLint surf_attribs[] = {
    EGL_RENDER_BUFFER, EGL_BACK_BUFFER,      // Double Buffered
    EGL_NONE
};

Colormap g_Framework_X11Colormap = 0;

void Application::ResizeScene(unsigned int width, unsigned int height)
{
    if (height == 0)
        height = 1;
    m_winWidth = width;
    m_winHeight = height;
}

void Application::ProcessWinInput() {
}

/*****************************************************/
/********************* EGL ***************************/
/*****************************************************/

int Application::InitWin() {
    // Xwindow Stuff first
    Atom                 wmDelete;
    XSetWindowAttributes attr;
    long                 screen = 0;
    XVisualInfo          visual;
    Window               rootWin;
    unsigned int         mask;
    int                  depth;
    int                  width;
    int                  height;
    extern int           g_PX;
    extern int           g_PY;
    extern int           g_SwapI;
    struct drm_fb *fb;
    printf("Framework_StartKMS\n");
#if 0
    screen = XDefaultScreen(g_Framework_X11Display);
    rootWin = RootWindow(g_Framework_X11Display, screen);
    width = DisplayWidth(g_Framework_X11Display, screen);
    height = DisplayHeight(g_Framework_X11Display, screen);
    depth = DefaultDepth(g_Framework_X11Display, screen);
#endif
   fd = open(device_name, O_RDWR);
   if (fd < 0) {
      /* Probably permissions error */
      fprintf(stderr, "couldn't open %s, skipping\n", device_name);
      return -1;
   }
   gbm = gbm_create_device(fd);
   if (gbm == NULL) {
      fprintf(stderr, "couldn't create gbm device\n");
      ret = -1;
      return -1;
   }
    setup_kms(fd, &kms);

    // Init EGL
    EGLint major;
    EGLint minor;
    EGLint n;
    EGLConfig configs[1];

    g_EglDisplay = eglGetDisplay((EGLNativeDisplayType)gbm);
    eglInitialize(g_EglDisplay, &major, &minor);
    eglBindAPI(EGL_OPENGL_ES_API);
    EGLBoolean res = eglChooseConfig(g_EglDisplay, config_attribs, configs, 0x1, &n);
    EGLint id = 0;
    eglGetConfigAttrib(g_EglDisplay, configs[0], EGL_CONFIG_ID, &id);
    printf("eglChooseConfig:%s; id=0x%x\n", (res && n!=0)?"OK":"FAIL", id);



    width = kms.mode.hdisplay;
    height = kms.mode.vdisplay;

printf("%d %d\n", kms.mode.hdisplay, kms.mode.vdisplay);
   gs = gbm_surface_create(gbm, kms.mode.hdisplay, kms.mode.vdisplay,
                           GBM_BO_FORMAT_XRGB8888,
                           GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);


    g_EglSurface = eglCreateWindowSurface(g_EglDisplay, configs[0], ( EGLNativeWindowType)gs , surf_attribs);
    g_EglContext = eglCreateContext(g_EglDisplay, configs[0], 0, context_attribs);
    eglMakeCurrent(g_EglDisplay, g_EglSurface, g_EglSurface, g_EglContext);
    eglSwapInterval(g_EglDisplay, g_SwapI);


  FD_ZERO(&fds);
  FD_SET(0, &fds);
  FD_SET(fd, &fds);


  glClearColor(0.5, 0.5, 0.5, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  eglSwapBuffers(g_EglDisplay, g_EglSurface);
  bo = gbm_surface_lock_front_buffer(gs);
  fb = drm_fb_get_from_bo(bo);

  saved_crtc = drmModeGetCrtc(fd, kms.encoder->crtc_id);
  ret = drmModeSetCrtc(fd, kms.encoder->crtc_id, fb->fb_id, 0, 0, &kms.connector->connector_id, 1, &kms.mode);
  if (ret) {
     printf("failed to set mode: %s\n", strerror(errno));
     return ret;
  }


#ifdef USE_PERFQ
    g_PQ = PerfQuery::GetInstance();
    g_PQ->Init();
#endif
    return 1;
}

void Application::ShutdownWin() {
    printf("Shutdown Win\n");

    // First EGL
    eglMakeCurrent(g_EglDisplay, 0, 0, 0);
    eglDestroySurface(g_EglDisplay, g_EglSurface);
    eglDestroyContext(g_EglDisplay, g_EglContext);
    eglTerminate(g_EglDisplay);
    // Then Xwindows

   ret = drmModeSetCrtc(fd, saved_crtc->crtc_id, saved_crtc->buffer_id, saved_crtc->x, saved_crtc->y, &kms.connector->connector_id, 1, &saved_crtc->mode);
   if (ret) {
      fprintf(stderr, "failed to restore crtc: %m\n");
   }
   drmModeFreeCrtc(saved_crtc);
   drmModeRmFB(fd, kms.fb_id);
   gbm_device_destroy(gbm);
   close(fd);

}



void Application::Draw() {
   // reset player direction (so that there is not continuous movement)
   direction = 0.0f;
   mouseX = 0.0f;
   mouseY = 0.0f;

#ifdef USE_PERFQ
   g_PQ->Begin();
#endif
  int waiting_for_flip = 1;
  struct gbm_bo *next_bo;
  struct drm_fb *fb;
  drmEventContext evctx;
  memset(&evctx, 0 , sizeof(drmEventContext));
  evctx.version = DRM_EVENT_CONTEXT_VERSION;
  evctx.page_flip_handler = page_flip_handler;


//printf("Draw\n");
   // Draw City
   m_city.Draw(camera);

   // Swap buffers
   eglSwapBuffers(g_EglDisplay, g_EglSurface);

		next_bo = gbm_surface_lock_front_buffer(gs);
		fb = drm_fb_get_from_bo(next_bo);

		ret = drmModePageFlip(fd, kms.encoder->crtc_id, fb->fb_id,
				DRM_MODE_PAGE_FLIP_EVENT, &waiting_for_flip);
		if (ret) {
			printf("failed to queue page flip: %s\n", strerror(errno));
			return ;
		}

		while (waiting_for_flip) {
			ret = select(fd + 1, &fds, NULL, NULL, NULL);
			if (ret < 0) {
				printf("select err: %s\n", strerror(errno));
				return ;
			} else if (ret == 0) {
				printf("select timeout!\n");
				return ;
			} else if (FD_ISSET(0, &fds)) {
				printf("user interrupted!\n");
				break;
			}
			drmHandleEvent(fd, &evctx);
		}

		/* release last buffer to render on again: */
		gbm_surface_release_buffer(gs, bo);
		bo = next_bo;


   // Update redrawHappened
   m_redrawHappened = true;

#ifdef USE_PERFQ
   g_PQ->EndGetDump();
#endif
}
