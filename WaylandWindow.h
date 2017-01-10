#ifndef WAYLANDWINDOW_H
#define WAYLANDWINDOW_H

//#include "Common.h"

#ifdef __unix__
#include <wayland-client.h>
#include <wayland-egl.h>
#include <wayland-cursor.h>

#include "Application.h"

extern bool g_validationMode;

#endif

#endif // WAYLANDWINDOW_H
