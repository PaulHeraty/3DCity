#include "WaylandWindow.h"

/*****************************************************/
/***************** WAYLAND ***************************/
/*****************************************************/

EGLint config_attribs[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RED_SIZE, 0x8,
    EGL_GREEN_SIZE, 0x8,
    EGL_BLUE_SIZE, 0x8,
    EGL_ALPHA_SIZE, 0x8,
    EGL_DEPTH_SIZE, 0x18,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    // EGL_STENCIL_SIZE, 0x8,
    EGL_STENCIL_SIZE, 0x8,
    //EGL_SAMPLES, 0x4,
    EGL_SAMPLES, MSAA_FACTOR,
    //EGL_SAMPLE_BUFFERS, 0x1,
    EGL_NONE
};

EGLint surf_attribs[] = {
    EGL_RENDER_BUFFER, EGL_BACK_BUFFER,      // Double Buffered
    EGL_NONE
};

static const EGLint context_attribs[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
};

static void init_egl(struct display *display, int opaque) {
    EGLint major, minor, n;
    EGLBoolean ret;

    if (opaque)
        config_attribs[9] = 0;

    //printf("eglGetDisplay()\n");
    display->egl.dpy = eglGetDisplay(display->display);
    assert(display->egl.dpy);

    //printf("eglInitialize()\n");
    ret = eglInitialize(display->egl.dpy, &major, &minor);
    assert(ret == EGL_TRUE);
    //printf("eglBindAPI()\n");
    ret = eglBindAPI(EGL_OPENGL_ES_API);
    assert(ret == EGL_TRUE);

    //printf("eglChooseConfig()\n");
    ret = eglChooseConfig(display->egl.dpy, config_attribs, &display->egl.conf, 1, &n);
    assert(ret && n == 1);

    // Match Replay (in frame0())
    EGLint id = 0;
    //printf("eglGetConfigAttrib()\n");
    eglGetConfigAttrib(display->egl.dpy, display->egl.conf, EGL_CONFIG_ID, &id);
    printf("eglChooseConfig:%s; id=0x%x\n", (ret && n!=0)?"OK":"FAIL", id);

    display->egl.ctx = eglCreateContext(display->egl.dpy, display->egl.conf, EGL_NO_CONTEXT, context_attribs);
    //assert(display->egl.ctx);

}

/************************* Shell Surface Listeners ***************/

static void handle_ping(void *data, struct wl_shell_surface *shell_surface,
                        uint32_t serial) {
    wl_shell_surface_pong(shell_surface, serial);
}

static void handle_configure(void *data, struct wl_shell_surface *shell_surface,
                             uint32_t edges, int32_t width, int32_t height) {
    struct window *window = (struct window*) data;

    if (window->native) {
        //printf("wl_egl_window_resize()\n");
        wl_egl_window_resize(window->native, width, height, 0, 0);
    }

    window->geometry.width = width;
    window->geometry.height = height;

    if (!window->fullscreen)
        window->window_size = window->geometry;

}

static void handle_popup_done(void *data, struct wl_shell_surface *shell_surface) {
}

struct wl_shell_surface_listener shell_surface_listener = {
    handle_ping,
    handle_configure,
    handle_popup_done
};

/********************* Callback Listeners ***********/

static void redraw(void *data, struct wl_callback *callback, uint32_t time);

static void configure_callback(void *data, struct wl_callback *callback, uint32_t  time) {
    struct window *window = (struct window*) data;

    //printf("configure_callback\n");
    //printf("wl_callback_destroy()\n");
    wl_callback_destroy(callback);

    window->configured = 1;

    if (window->callback == NULL)
        redraw(data, NULL, time);
}

static struct wl_callback_listener configure_callback_listener = {
    configure_callback,
};

/******************* Toggle Fullscreen *****************/

static void toggle_fullscreen(struct window *window, int fullscreen) {
    struct wl_callback *callback;

    //printf("Toggle fullscreen\n");
    window->fullscreen = fullscreen;
    window->configured = 0;


    if (fullscreen) {
        //printf("wl_shell_surface_set_fullscreen()\n");
        wl_shell_surface_set_fullscreen(window->shell_surface,
                                        WL_SHELL_SURFACE_FULLSCREEN_METHOD_DEFAULT,
                                        0, NULL);
    } else {
        //printf("wl_shell_surface_set_toplevel()\n");
        wl_shell_surface_set_toplevel(window->shell_surface);
        //printf("handle_configure()\n");
        handle_configure(window, window->shell_surface, 0,
                         window->window_size.width,
                         window->window_size.height);
    }


    //printf("wl_display_sync()\n");
    callback = wl_display_sync(window->display->display);

    //printf("wl_callback_add_listener()\n");
    wl_callback_add_listener(callback, &configure_callback_listener, window);
}

/****************** Create surface *********************/

static void create_surface(struct window *window) {
    struct display *display = window->display;
    EGLBoolean ret;

    //printf("wl_compositor_create_surface()\n");
    window->surface = wl_compositor_create_surface(display->compositor);
    //printf("wl_shell_get_shell_surface()\n");
    window->shell_surface = wl_shell_get_shell_surface(display->shell, window->surface);

    //printf("wl_shell_surface_add_listener()\n");
    wl_shell_surface_add_listener(window->shell_surface, &shell_surface_listener, window);

    //printf("wl_egl_window_create()\n");
    window->native = wl_egl_window_create(window->surface, window->window_size.width,
                                 window->window_size.height);
    //printf("eglCreateWindowSurface()\n");
    window->egl_surface = eglCreateWindowSurface(display->egl.dpy, display->egl.conf,
                                   window->native, surf_attribs);

    wl_shell_surface_set_title(window->shell_surface, "simple-egl");

    // Replay stuff
    //printf("wl_shell_surface_set_toplevel()\n");
    wl_shell_surface_set_toplevel(window->shell_surface);
    //printf("eglCreateContext()\n");
    display->egl.ctx = eglCreateContext(display->egl.dpy, display->egl.conf, EGL_NO_CONTEXT, context_attribs);


    //printf("eglMakeCurrent()\n");
    ret = eglMakeCurrent(window->display->egl.dpy, window->egl_surface,
                         window->egl_surface, window->display->egl.ctx);
    assert(ret == EGL_TRUE);

    toggle_fullscreen(window, window->fullscreen);
}

static void destroy_surface(struct window *window) {
    wl_egl_window_destroy(window->native);

    wl_shell_surface_destroy(window->shell_surface);
    wl_surface_destroy(window->surface);

    if (window->callback)
        wl_callback_destroy(window->callback);
}

/*************************** Redraw routine ******************/
const struct wl_callback_listener frame_listener = {
    redraw
};

extern Application       g_Application;

void redraw(void *data, struct wl_callback *callback, uint32_t time) {
    static bool firstTime = true;
    struct window *window = (struct window*) data;
    struct wl_region *region;
    EGLint major, minor, n;
    struct timeval t1, t2, t3, t4;
    struct timezone tz;
    float deltatime;

    // reset player direction (so that there is not continuous movement)
    g_Application.direction = 0.0f;
    g_Application.mouseX = 0.0f;
    g_Application.mouseY = 0.0f;

    // Validation mode stats
    if(g_validationMode) {
        gettimeofday ( &t1 , &tz );
        printf("  Frame time before GLdraw calls: %6.2f uS\n", (t1.tv_sec * 1e6) + t1.tv_usec);
    }

    // Draw City
    g_Application.m_city.Draw(g_Application.camera);

    // Validation mode stats
    if(g_validationMode) {
        gettimeofday(&t2, &tz);
        deltatime = (float)(((t2.tv_sec - t1.tv_sec) * 1e6) + (t2.tv_usec - t1.tv_usec));
        printf("  Frame time after GLdraw calls : %6.2f uS\n", (t2.tv_sec * 1e6) + t2.tv_sec);
        printf("     Elapsed time for GLdraw calls: %6.2f uS\n", deltatime);
    }

    //printf("Wayland redraw() wayland callbacks.\...\n");

    if (callback) {
        //printf("wl_callback_destroy()\n");
        wl_callback_destroy(callback);
    }

    if (!window->configured)
        return;

    // Set opaque region for entire window to prevent alpha belnding in compositor
    region = wl_compositor_create_region(window->display->compositor);
    wl_region_add(region, 0, 0, window->geometry.width, window->geometry.height);
    wl_surface_set_opaque_region(window->surface, region);
    wl_region_destroy(region);

    //printf("wl_surface_frame()\n");
    window->callback = wl_surface_frame(window->surface);
    //printf("wl_callback_add_listener()\n");
    wl_callback_add_listener(window->callback, &frame_listener, window);

    // Validation mode stats
    if(g_validationMode) {
        gettimeofday ( &t3 , &tz );
        printf("  Frame time before eglSwapBuffers: %6.2f uS\n", (t3.tv_sec * 1e6) + t3.tv_sec);
    }

    //printf("Wayland redraw() eglSwapBuffers()\n");
    eglSwapBuffers(window->display->egl.dpy, window->egl_surface);

    // Validation mode stats
    if(g_validationMode) {
        gettimeofday(&t4, &tz);
        deltatime = (float)(((t4.tv_sec - t3.tv_sec) * 1e6) + (t4.tv_usec - t3.tv_usec));
        printf("  Frame time after eglSwapBuffers: %6.2f uS\n", (t4.tv_sec * 1e6) + t4.tv_sec);
        printf("    Elapsed time for eglSwapBuffers: %6.2f uS\n", deltatime);
    }

    // Update redrawHappened
    g_Application.m_redrawHappened = true;
}

/*********************** Mouse Listeners *************************/

static void pointer_handle_enter(void *data, struct wl_pointer *pointer,
                                 uint32_t serial, struct wl_surface *surface,
                                 wl_fixed_t sx, wl_fixed_t sy) {
    struct display *display = (struct display*) data;
    struct wl_buffer *buffer;
    struct wl_cursor *cursor = display->default_cursor;
    struct wl_cursor_image *image;

    if (display->window->fullscreen)
        wl_pointer_set_cursor(pointer, serial, NULL, 0, 0);
    else if (cursor) {
        image = display->default_cursor->images[0];
        buffer = wl_cursor_image_get_buffer(image);
        wl_pointer_set_cursor(pointer, serial,
                              display->cursor_surface,
                              image->hotspot_x,
                              image->hotspot_y);
        wl_surface_attach(display->cursor_surface, buffer, 0, 0);
        wl_surface_damage(display->cursor_surface, 0, 0,
                          image->width, image->height);
        wl_surface_commit(display->cursor_surface);
    }
}

static void pointer_handle_leave(void *data, struct wl_pointer *pointer,
                                 uint32_t serial, struct wl_surface *surface) {
}

static void pointer_handle_motion(void *data, struct wl_pointer *pointer,
                                  uint32_t time, wl_fixed_t sx, wl_fixed_t sy) {
    // For some reason, we have to divide the coordinates by 256
    // Don;t know why, but it works...
    g_WindowInput.mouseX = sx/256;
    g_WindowInput.mouseY = sy/256;
    //printf("Mouse at %d, %d\n", sx, sy);
}

static void pointer_handle_button(void *data, struct wl_pointer *wl_pointer,
                                  uint32_t serial, uint32_t time, uint32_t button,
                                  uint32_t state) {
    struct display *display = (struct display*) data;

    struct wl_pointer *paul = (struct wl_pointer*) display->pointer;
    //unsigned int ph = paul->grab_button;

    if (button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED)
        wl_shell_surface_move(display->window->shell_surface,
                              display->seat, serial);

    if (button == BTN_LEFT) {
        g_WindowInput.but1 = true;
        //g_WindowInput.mouseX = wl_pointer->enter.surface_x;
        //g_WindowInput.mouseY = event.xbutton.y;
    }
}

static void pointer_handle_axis(void *data, struct wl_pointer *wl_pointer,
                                uint32_t time, uint32_t axis, wl_fixed_t value) {
}

static const struct wl_pointer_listener pointer_listener = {
    pointer_handle_enter,
    pointer_handle_leave,
    pointer_handle_motion,
    pointer_handle_button,
    pointer_handle_axis,
};

/************************** Keyboard Listeners **************************/

static void keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard,
                                   uint32_t format, int fd, uint32_t size) {
}

static void keyboard_handle_enter(void *data, struct wl_keyboard *keyboard,
                                  uint32_t serial, struct wl_surface *surface,
                                  struct wl_array *keys) {
}

static void keyboard_handle_leave(void *data, struct wl_keyboard *keyboard,
                                  uint32_t serial, struct wl_surface *surface) {
}

static void keyboard_handle_key(void *data, struct wl_keyboard *keyboard,
                                uint32_t serial, uint32_t time, uint32_t key,
                                uint32_t state) {
    struct display *d = (struct display*) data;
    unsigned char inkey;

    //printf("Wayland found key press %d %c\n", key, key);

    // Temp hack to map Wayland keys to ASCII
    if(key == KEY_Q && state)
        inkey = 'q';
    else if(key == KEY_W)
        inkey = 'w';
    else if(key == KEY_E)
        inkey = 'e';
    else if(key == KEY_A)
        inkey = 'a';
    else if(key == KEY_S)
        inkey = 's';
    else if(key == KEY_D)
        inkey = 'd';
    else if(key == KEY_Z && state)
        inkey = 'z';
    else if(key == KEY_C && state)
        inkey = 'c';
    // && state take kepress on key down press event only
    else if(key == KEY_P && state)
        inkey = 'p';
    else if(key == KEY_F && state)
        inkey = 'f';
    else if(key == KEY_V && state)
        inkey = 'v';
    else if(key == KEY_X && state)
        inkey = 'x';
    else if(key == KEY_J && state)
        inkey = 'j';
    else if(key == KEY_T && state)
        inkey = 't';
    else if(key == KEY_B && state)
        inkey = 'b';
    else if(key == KEY_G && state)
        inkey = 'g';
    else if(key == KEY_M && state)
        inkey = 'm';
    else if(key == KEY_L && state)
        inkey = 'l';
    else if(key == KEY_O && state)
        inkey = 'o';
    else if(key == KEY_H && state)
        inkey = 'h';
    else if(key == KEY_I && state)
        inkey = 'i';
    else if(key == KEY_ESC && state)
        inkey = 033; // ASCII for ESC
    else
        inkey = 0;

    g_WindowInput.key = inkey;
}

static void keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard,
                                      uint32_t serial, uint32_t mods_depressed,
                                      uint32_t mods_latched, uint32_t mods_locked,
                                      uint32_t group)
{
}

static const struct wl_keyboard_listener keyboard_listener = {
    keyboard_handle_keymap,
    keyboard_handle_enter,
    keyboard_handle_leave,
    keyboard_handle_key,
    keyboard_handle_modifiers,
};

/*********************** Seat Listeners ********************/

static void seat_handle_capabilities(void *data, struct wl_seat *seat,
                                     uint32_t caps) {
//                                     enum wl_seat_capability caps) {
    struct display *d = (struct display*) data;

    if ((caps & WL_SEAT_CAPABILITY_POINTER) && !d->pointer) {
        d->pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(d->pointer, &pointer_listener, d);
    } else if (!(caps & WL_SEAT_CAPABILITY_POINTER) && d->pointer) {
        wl_pointer_destroy(d->pointer);
        d->pointer = NULL;
    }

    if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !d->keyboard) {
        d->keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(d->keyboard, &keyboard_listener, d);
    } else if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && d->keyboard) {
        wl_keyboard_destroy(d->keyboard);
        d->keyboard = NULL;
    }
}

static const struct wl_seat_listener seat_listener = {
    seat_handle_capabilities,
};

/*********************** Registry Listeners ********************/

static void registry_handle_global(void *data, struct wl_registry *registry,
                                   uint32_t name, const char *interface, uint32_t version) {
    struct display *d = (struct display*) data;

    if (strcmp(interface, "wl_compositor") == 0) {
        d->compositor = (wl_compositor*) wl_registry_bind(registry, name, &wl_compositor_interface, 1);
    } else if (strcmp(interface, "wl_shell") == 0) {
        d->shell = (wl_shell*) wl_registry_bind(registry, name, &wl_shell_interface, 1);
    } else if (strcmp(interface, "wl_seat") == 0) {
        d->seat = (wl_seat*) wl_registry_bind(registry, name, &wl_seat_interface, 1);
        wl_seat_add_listener(d->seat, &seat_listener, d);
    } else if (strcmp(interface, "wl_shm") == 0) {
        d->shm = (wl_shm*) wl_registry_bind(registry, name, &wl_shm_interface, 1);
        d->cursor_theme = wl_cursor_theme_load(NULL, 32, d->shm);
        d->default_cursor = wl_cursor_theme_get_cursor(d->cursor_theme, "left_ptr");
    }
}

static const struct wl_registry_listener registry_listener = {
    registry_handle_global
};

static void signal_int(int signum) {
    g_Application.exitApp = 1;
}

/************************* Interface Routines ********************/


int Application::InitWin() {
    struct sigaction sigint;

    memset(&g_WaylandDisplay, 0, sizeof(display));
    memset(&g_WaylandWindow, 0, sizeof(window));

    g_WaylandDisplay = {0};
    g_WaylandWindow  = {0};

    g_WaylandWindow.display = &g_WaylandDisplay;
    g_WaylandDisplay.window = &g_WaylandWindow;
    g_WaylandWindow.window_size.width = g_Application.m_winWidth;
    g_WaylandWindow.window_size.height = g_Application.m_winHeight;

    //printf("wl_display_connect()\n");
    g_WaylandDisplay.display = wl_display_connect(NULL);
    //assert(g_WaylandDisplay.display);

    //printf("wl_display_get_registry()\n");
    g_WaylandDisplay.registry = wl_display_get_registry(g_WaylandDisplay.display);
    //printf("wl_registry_add_listener()\n");
    wl_registry_add_listener(g_WaylandDisplay.registry, &registry_listener, &g_WaylandDisplay);

    //printf("wl_display_dispatch()\n");
    wl_display_dispatch(g_WaylandDisplay.display);

    init_egl(&g_WaylandDisplay, g_WaylandWindow.opaque);
    create_surface(&g_WaylandWindow);

    //printf("wl_compositor_create_surface()\n");
    g_WaylandDisplay.cursor_surface = wl_compositor_create_surface(g_WaylandDisplay.compositor);

    sigint.sa_handler = signal_int;
    sigemptyset(&sigint.sa_mask);
    sigint.sa_flags = SA_RESETHAND;
    sigaction(SIGINT, &sigint, NULL);

    return 1;
}

void Application::ShutdownWin() {
    printf("Stop_Win Wayland...\n");

    eglMakeCurrent(g_WaylandDisplay.egl.dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    destroy_surface(&g_WaylandWindow);
    eglDestroyContext(g_WaylandDisplay.egl.dpy, g_WaylandDisplay.egl.ctx);
    //eglTerminate(g_WaylandDisplay.egl.dpy);

    //wl_surface_destroy(g_WaylandDisplay.cursor_surface);
    if (g_WaylandDisplay.cursor_theme) {
        wl_cursor_theme_destroy(g_WaylandDisplay.cursor_theme);
    }

    if (g_WaylandDisplay.shell) {
        wl_shell_destroy(g_WaylandDisplay.shell);
    }

    if (g_WaylandDisplay.compositor) {
        wl_compositor_destroy(g_WaylandDisplay.compositor);
    }

    wl_registry_destroy(g_WaylandDisplay.registry);
    wl_display_flush(g_WaylandDisplay.display);
    wl_display_disconnect(g_WaylandDisplay.display);
}

void Application::ResizeScene(unsigned int width, unsigned int height) {

}

void Application::ProcessWinInput() {
    // Processed by keyboard_handle_key() callback in Wayland
}

void Application::Draw() {
    // In Wayland, all drawing is handled by redraw() callback. This is only triggered (by wl_display_dispatch) when the
    // compositor is ready to accept another frame. Otherwise the application loop will draw nothing (save GFX resources)

    struct timeval t1, t2;
    struct timezone tz;
    float deltatime;

    // Validation mode stats
    if(g_validationMode) {
        gettimeofday ( &t1 , &tz );
        printf("FRAME START: Frame time before wl_display_dispatch: %6.2f uS\n", (t1.tv_sec * 1e6) + t1.tv_usec);
    }

    //printf("wl_display_dispatch()\n");
    wl_display_dispatch(g_WaylandDisplay.display);

    // Validation mode stats
    if(g_validationMode) {
        gettimeofday(&t2, &tz);
        deltatime = (float)(((t2.tv_sec - t1.tv_sec) * 1e6) + (t2.tv_usec - t1.tv_usec));
        printf("FRAME END: Frame time after wl_display_dispatch: %6.2f uS\n", (t2.tv_sec * 1e6) + t2.tv_usec);
        printf("FRAME END: Elapsed time: %6.2f uS\n", deltatime);
    }
}
