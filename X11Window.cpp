#include "X11Window.h"

#ifdef SPACENAV
#include "spnav_ctrl.h"
#endif

#ifdef USE_PERFQ
#include "PerfQueries.h"
PerfQuery *g_PQ = NULL;
#endif
/*****************************************************/
/********************* X11 ***************************/
/*****************************************************/
extern int g_DoDraw ;
#ifdef USE_GL

int g_Framework_DisplayAttribute[] = { 
    GLX_RGBA, GLX_DOUBLEBUFFER,
    GLX_RED_SIZE, 8,
    GLX_GREEN_SIZE, 8,
    GLX_BLUE_SIZE, 8,
    GLX_DEPTH_SIZE, 0x18,
    GLX_STENCIL_SIZE, 8,
    None };
GLXContext g_Framework_Context;

#else
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
#endif

Colormap g_Framework_X11Colormap = 0;

void Application::ResizeScene(unsigned int width, unsigned int height)
{
    if (height == 0)
        height = 1;
    m_winWidth = width;
    m_winHeight = height;
}

void Application::ProcessWinInput() {
    XEvent event;
    KeySym key;
    char text;

#ifdef SPACENAV
    int spn = SpNavCheckMessage();
    if (spn>0) {
      g_WindowInput.key = spn;
    }
#endif

    while (XPending(g_Framework_X11Display) > 0) {
        XNextEvent(g_Framework_X11Display, &event);
        switch (event.type) {
        case KeyPress:
            //printf("Keypress\n");
            if (XLookupString(&event.xkey,&text,1,&key,0)==1) {
                //g_Application.Key(text, 0, 0);
                //retVal.key = text;
                g_WindowInput.key = text;
            }
            break;
        case ConfigureNotify:
            if ((event.xconfigure.width != m_winWidth) ||
                    (event.xconfigure.height != m_winWidth)) {
                ResizeScene(event.xconfigure.width, event.xconfigure.height);
                //printf("Resize event (%d,%d) -> (%d,%d)\n", event.xconfigure.width, event.xconfigure.height,winWidth, winHeight);
            }
            break;
        case ButtonPress:
            //printf("Mouse Button Press at %d, %d\n", event.xbutton.x, event.xbutton.y);
            g_WindowInput.but1 = true;
            g_WindowInput.mouseX = event.xbutton.x;
            g_WindowInput.mouseY = event.xbutton.y;
            break;
        case MotionNotify:
            //printf("Mouse movement\n");
            //x = event.xbutton.x;
            //y = event.xbutton.y;
            //if (esContext->mouseFunc != NULL)
            //    esContext->mouseFunc(esContext, x, y);
            break;
        }
    }
    return ;
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
    printf("Framework_StartX11\n");
    if (!(g_Framework_X11Display = XOpenDisplay(":0"))) {
        printf("Error: Unable to open X display\n");
        return -1;
    }
    memset(&attr, 0, sizeof(XSetWindowAttributes));
    screen = XDefaultScreen(g_Framework_X11Display);
    rootWin = RootWindow(g_Framework_X11Display, screen);
    width = DisplayWidth(g_Framework_X11Display, screen);
    height = DisplayHeight(g_Framework_X11Display, screen);
    depth = DefaultDepth(g_Framework_X11Display, screen);
#ifdef USE_GL
    XVisualInfo *vi;
    int glxMajorVersion, glxMinorVersion;
    int vidModeMajorVersion, vidModeMinorVersion;
    vi = glXChooseVisual(g_Framework_X11Display, screen, g_Framework_DisplayAttribute);
    if (vi == NULL) {
      printf("glXChooseVisual failed\n");
      return -1;
    }
    printf("VID=0x%x\n", vi->visualid);
    glXQueryVersion(g_Framework_X11Display, &glxMajorVersion, &glxMinorVersion);
    printf("glX-Version %d.%d\n", glxMajorVersion, glxMinorVersion);
    g_Framework_Context = glXCreateContext(g_Framework_X11Display, vi, 0, GL_TRUE);
    g_Framework_X11Colormap = XCreateColormap(g_Framework_X11Display, rootWin, vi->visual, AllocNone);
#else
    if (!XMatchVisualInfo(g_Framework_X11Display, screen, depth, TrueColor, &visual)) {
        printf("Error: Unable to acquire visual\n");
        XCloseDisplay(g_Framework_X11Display);
        return -1;
    }

    printf("Current Screen: Resolution: %dx%d  Depth: %d BitsPerRGB : %d Config: %lu\n",
           width, height, depth, visual.bits_per_rgb, visual.visualid);
    g_Framework_X11Colormap = XCreateColormap(g_Framework_X11Display, rootWin, visual.visual, AllocNone);
#endif
    attr.colormap = g_Framework_X11Colormap;
    attr.event_mask = StructureNotifyMask | ExposureMask | PointerMotionMask | KeyPressMask | ButtonPressMask;
    mask = CWBackPixel | CWBorderPixel | CWEventMask | CWColormap; // CWOverrideRedirect;

#if 0 // Fullscreen
     attr.override_redirect = GL_TRUE;
     mask |= CWOverrideRedirect;
#endif

#ifdef USE_GL
    g_Framework_X11Window = XCreateWindow(g_Framework_X11Display, rootWin, g_PX, g_PY, m_winWidth, m_winHeight, 0, CopyFromParent, InputOutput, vi->visual, mask, &attr);
#else
    g_Framework_X11Window = XCreateWindow(g_Framework_X11Display, rootWin, g_PX, g_PY, m_winWidth, m_winHeight, 0, CopyFromParent, InputOutput, CopyFromParent, mask, &attr);
#endif
    wmDelete = XInternAtom(g_Framework_X11Display, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(g_Framework_X11Display, g_Framework_X11Window, &wmDelete, 1);
    XMapWindow(g_Framework_X11Display, g_Framework_X11Window);
    XFlush(g_Framework_X11Display);

    XMoveWindow(g_Framework_X11Display,  g_Framework_X11Window, g_PX, g_PY);
#ifdef USE_GL
    glXMakeCurrent(g_Framework_X11Display, g_Framework_X11Window, g_Framework_Context);
#else
    // Init EGL
    EGLint major;
    EGLint minor;
    EGLint n;
    EGLConfig configs[1];

    g_EglDisplay = eglGetDisplay(g_Framework_X11Display);
    eglInitialize(g_EglDisplay, &major, &minor);
    eglBindAPI(EGL_OPENGL_ES_API);
    EGLBoolean res = eglChooseConfig(g_EglDisplay, config_attribs, configs, 0x1, &n);
    EGLint id = 0;
    eglGetConfigAttrib(g_EglDisplay, configs[0], EGL_CONFIG_ID, &id);
    printf("eglChooseConfig:%s; id=0x%x\n", (res && n!=0)?"OK":"FAIL", id);
    g_EglSurface = eglCreateWindowSurface(g_EglDisplay, configs[0], g_Framework_X11Window, surf_attribs);
    g_EglContext = eglCreateContext(g_EglDisplay, configs[0], 0, context_attribs);
    eglMakeCurrent(g_EglDisplay, g_EglSurface, g_EglSurface, g_EglContext);
    eglSwapInterval(g_EglDisplay, g_SwapI);
#endif
#ifdef USE_PERFQ
    g_PQ = PerfQuery::GetInstance();
    g_PQ->Init();
#endif
    return 1;
}

void Application::ShutdownWin() {
    printf("Shutdown Win\n");

#ifdef USE_GL
#else
    // First EGL
    eglMakeCurrent(g_EglDisplay, 0, 0, 0);
    eglDestroySurface(g_EglDisplay, g_EglSurface);
    eglDestroyContext(g_EglDisplay, g_EglContext);
    eglTerminate(g_EglDisplay);
#endif
    // Then Xwindows
    if (g_Framework_X11Window)
        XDestroyWindow(g_Framework_X11Display, g_Framework_X11Window);
    if (g_Framework_X11Colormap)
        XFreeColormap(g_Framework_X11Display, g_Framework_X11Colormap);
    if (g_Framework_X11Display)
        XCloseDisplay(g_Framework_X11Display);
}

void Application::Draw() {

  if (g_DoDraw) {
   // reset player direction (so that there is not continuous movement)
   direction = 0.0f;
   mouseX = 0.0f;
   mouseY = 0.0f;

#ifdef USE_PERFQ
   g_PQ->Begin();
#endif

   // If we are post-processing, then render to FBO instead of FB
   if(m_FXAApostProc) {
       // bind the frame buffer
       glBindFramebuffer(GL_FRAMEBUFFER, m_FBO_framebuffer);
   }

   // Draw City
   m_city.Draw(camera);

   // If we are post-processing, then do it here & render FBO to screen
   if(m_FXAApostProc) {
       // render to window system provided framebuffer
       glBindFramebuffer(GL_FRAMEBUFFER, 0);
       // use m_FBO_texture to draw to framebuffer. We draw a quad that is the size of the viewport

       // Setup drawing state
       glUseProgram(m_FBO_material.m_shader.m_programObject);

       glBindBuffer(GL_ARRAY_BUFFER, 0);
       glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

       // Bind attributes
       glEnableVertexAttribArray(m_FBO_material.m_shader.m_positionLoc );
       glEnableVertexAttribArray(m_FBO_material.m_shader.m_texCoordLoc );
       glVertexAttribPointer(m_FBO_material.m_shader.m_positionLoc, 3, GL_FLOAT, GL_FALSE, m_FBO_vtxStride, m_FBO_verts);
       glVertexAttribPointer(m_FBO_material.m_shader.m_texCoordLoc, 2, GL_FLOAT, GL_FALSE, m_FBO_vtxStride, (m_FBO_verts + 3));

       // Bind the texture
       glActiveTexture( GL_TEXTURE0 );
       //glEnable(GL_TEXTURE_2D); DEPRECATED
       glBindTexture( GL_TEXTURE_2D, m_FBO_texture );

       // Set the sampler texture unit to 0
       glUniform1i(m_FBO_material.m_shader.m_samplerLoc, 0 );

       // Set texCoordOffset uniforms
       glUniform1f(m_FBO_material.m_shader.u_texCoordOffsetX, 1.0/m_FBO_texWidth);
       glUniform1f(m_FBO_material.m_shader.u_texCoordOffsetY, 1.0/m_FBO_texHeight);

       glDisable(GL_DEPTH_TEST);

       // Don't write to depth buffer
       glDepthMask(GL_FALSE);

       // Draw the screen quad
       glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, m_FBO_index);

       // Turn back on depth test, and disable stencil test
       glEnable(GL_DEPTH_TEST);
       glDepthMask(GL_TRUE);
   }


#ifdef USE_GL
   glXSwapBuffers(g_Framework_X11Display, g_Framework_X11Window);
#else
   // Swap buffers
   eglSwapBuffers(g_EglDisplay, g_EglSurface);
#endif
   // Update redrawHappened
   m_redrawHappened = true;

#ifdef USE_PERFQ
   g_PQ->EndGetDump();
#endif
  } else {
   usleep(500);
  }
}
