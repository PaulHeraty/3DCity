//#ifndef COMMON_H
//#define COMMON_H
#if defined(_WIN32) || defined(WIN32)
    #define WIN_XML
#elif defined(__unix__)
    #define USE_XML
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdexcept>
#include <time.h>
#include <string.h>
#include <string>
#include "vector"
#include "math.h"
#include <iostream>
#include <fstream>
#include <deque>
#include <assert.h>
#include <signal.h>
#include <sstream>
#include <locale>
#ifdef USE_XML
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#endif
#ifdef WIN_XML
#include <Shlwapi.h>
#include <xmllite.h>
#endif

#ifdef __unix__
	#include <unistd.h>
	#include <sys/time.h>
	#include <linux/input.h>
#ifdef USE_GL
	#define GL_GLEXT_PROTOTYPES
	#include <GL/gl.h>
	#include <GL/glext.h>
	#include <GL/glx.h>
#else
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#include <EGL/egl.h>
	#include <EGL/eglext.h>
#endif
	#include <netinet/in.h>	// for IPPROTO_UDP, sockadd_in
	#include <cairo/cairo.h>
	#include <linux/input.h>
	#include <stdbool.h>
#elif defined(_WIN32) || defined(WIN32)

extern void debugprintf(char* msg, ...);
#define printf debugprintf
	#include <Windows.h>
	#define CAIRO_WIN32_STATIC_BUILD
	extern "C"{
		#include <GL/gl.h>
		#include "Windows\glext.h"
		#include "Windows\wglext.h"
		#include <cairo.h>
	}
    #include "Windows/OGLDefs.h"

	// Disable warning about conversion from double to float
	#pragma warning(disable:4244)
	// Disable warning about conversion from double to GLfloat
	#pragma warning(disable:4305)
#endif

//#endif // COMMON_H
