#ifndef APPLICATION_H
#define APPLICATION_H

#include "Common.h"

#include "glTypes.h"
#include "Camera.h"
#include "City.h"
#include "AppMode.h"

#include "Config.h"
#include "app_helper.h"

#define MSAA_FACTOR 0x0

using namespace std;

extern bool g_StatsHud;

#ifdef USE_X11
#include "X11Window.h"
#endif

#if defined(_WIN32) || defined(WIN32)
#include "WindowsWindow.h"
#endif 

#ifdef USE_WAY
#include "WaylandWindow.h"
struct window;
struct seat;

struct display {
    struct wl_display *display;
    struct wl_registry *registry;
    struct wl_compositor *compositor;
    struct wl_shell *shell;
    struct wl_seat *seat;
    struct wl_pointer *pointer;
    struct wl_keyboard *keyboard;
    struct wl_shm *shm;
    struct wl_cursor_theme *cursor_theme;
    struct wl_cursor *default_cursor;
    struct wl_surface *cursor_surface;
    struct {
            EGLDisplay dpy;
            EGLContext ctx;
            EGLConfig conf;
    } egl;
    struct window *window;
};

struct geometry {
        int width, height;
};

struct window {
        struct display *display;
        struct geometry geometry, window_size;
        struct wl_egl_window *native;
        struct wl_surface *surface;
        struct wl_shell_surface *shell_surface;
        EGLSurface egl_surface;
        struct wl_callback *callback;
        int fullscreen, configured, opaque;
};
#endif

struct winInput {
    unsigned char key;
    int mouseX;
    int mouseY;
    bool but1;
    bool but2;
} ;

// Global window input var
extern winInput g_WindowInput;

enum ROTATE {
    LEFT,
    RIGHT,
    NONE
};

struct navPath {
    GLvector3 coord;
    HUD_TYPE hud;
};

// Define a context that contains all of the global variables needed to pass to
//  the code framework functions. This will be instanciated by main()
class Application {
public:
    Application();
    ~Application();

    int Init();
    void Draw();
    void Update(float deltaTime);
    void PostDraw();
    void Mouse();
    int Key();
    void ProcessInput();
    void Shutdown();
    void DumpSnapShot();
    void CompareSnapShot();

    virtual int InitWin();
    virtual void ShutdownWin();
    virtual void ProcessWinInput();
   // virtual void DrawWin();
    virtual void ResizeScene(unsigned int width, unsigned int height);
    bool WaypointReached();

//private:
    // Player class
    CPlayer player;

    // Cached copy of camera attached to player
    CCamera* camera;

    // App viewport
    VIEWPORT viewport;

    // Player Direction for movement
    unsigned long  direction;
    ROTATE m_rotate;

    // Mouse Cursor Pos
    GLvector2 oldCursorPos;
    float mouseX, mouseY;

    // City Map
    City m_city;

    // When to exit
    bool exitApp;

    // Compare
    bool m_compareSnap;
    bool m_dumpSnap;
    bool m_redrawHappened;

    // Benchmark Mode
    bool m_benchMarkMode;

    // Auto Pilot
    bool m_autoPilot;
    std::deque<GLvector3> m_flightPath;
    //std::deque<GLvector3> m_navPath;
    std::deque<navPath> m_navPath;
    GLvector3 m_destination;
    bool m_turning;

    int   m_winWidth ;
    int   m_winHeight ;
    int   m_fullScreen;

    // FXAA Post Processing (render to FBO)
    unsigned int m_FXAApostProc;
    GLuint m_FBO_framebuffer;
    GLuint m_FBO_depthRenderbuffer;
    GLuint m_FBO_texture;
    GLuint m_FBO_texWidth;
    GLint  m_FBO_texHeight;
    GLint  m_FBO_maxRenderbufferSize;
    GLenum m_FBO_status;
    GLfloat m_FBO_verts[20];    // x, y, z, u, v
    GLushort m_FBO_index[6];
    GLuint m_FBO_vtxStride;
    Material m_FBO_material;


	#ifdef __linux__
#ifndef USE_GL
    EGLDisplay  g_EglDisplay;
    EGLContext  g_EglContext;
    EGLSurface  g_EglSurface;
#endif
	#endif

    #ifdef USE_X11
    Display* g_Framework_X11Display;
    Window g_Framework_X11Window;
    #endif

    #ifdef USE_WAY
    struct display g_WaylandDisplay;
    struct window  g_WaylandWindow;
    #endif

	#if defined(_WIN32) || defined(WIN32)
	HDC       m_hDC;     // GDI Device Context
	HGLRC     m_hRC;     // Permanent Rendering Context
	HWND      m_hWnd;    // Our Window Handle
	HINSTANCE m_hInstance;      // Our Application Instance
	#endif

	// XML helper
    ApplicationHelper m_AppHelper;
};

extern Application g_Application;

#endif // APP_H
