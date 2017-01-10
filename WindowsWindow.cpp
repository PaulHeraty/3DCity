#include "WindowsWindow.h"

#ifdef SPACENAV
#include "spnav_ctrl.h"
#endif

#ifdef USE_PERFQ
#include "PerfQueries.h"
PerfQuery *g_PQ = NULL;
#endif

//OutputDebugString(work_string);
void debugprintf(char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	char buf[10000];
	vsprintf(buf, msg, args);
	OutputDebugStringA(buf);
	va_end(args);
}

void ShowError(const wchar_t* msg, ...)
{
	va_list args;
	va_start(args, msg);
	wchar_t buf[1000];
	wvsprintf(buf, msg, args);
	printf("%s\n", buf);
	MessageBox(NULL, buf, L"ERROR", MB_OK | MB_ICONEXCLAMATION);
	va_end(args);
}

void Application::ResizeScene(unsigned int width, unsigned int height) {
	printf("ReSizeGLScene(%d, %d)\n", width, height);
	if (height == 0)
		height = 1;                     // Avoid dividing by zero
	viewport.Width = m_winWidth = width;
	viewport.Height = m_winHeight = height;
}

/*
void Application::ProcessWinInput() {
XEvent event;
KeySym key;
char text;

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
*/

void Application::ProcessWinInput() {
	MSG msg; 
	if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
		//if (msg.message==WM_QUIT || msg.message==WM_KEYDOWN) {
		if (msg.message==WM_QUIT) {
			exitApp = true;
		} else { 
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT currMousePos;

	switch (uMsg) {
	case WM_ACTIVATE:             // Window Activate
		break;
	case WM_CLOSE:                // Window Close
		PostQuitMessage(0);         // => Nicely Quit
		//break;
		return 0;
	case WM_KEYDOWN:              // Key pressed
		{
			switch(wParam) {
			case VK_ESCAPE:
				g_Application.exitApp = true;
				break;
			case 'p':
			case 'P':
				g_WindowInput.key = 'p';
				break;
			case 'w':
			case 'W':
				g_WindowInput.key = 'w';
				break;
			case 'a':
			case 'A':
				g_WindowInput.key = 'a';
				break;
			case 's':
			case 'S':
				g_WindowInput.key = 's';
				break;
			case 'd':
			case 'D':
				g_WindowInput.key = 'd';
				break;
			case 'q':
			case 'Q':
				g_WindowInput.key = 'q';
				break;
			case 'e':
			case 'E':
				g_WindowInput.key = 'e';
				break;
			case 'z':
			case 'Z':
				g_WindowInput.key = 'z';
				break;
			case 'c':
			case 'C':
				g_WindowInput.key = 'c';
				break;
			case 'f':
			case 'F':
				g_WindowInput.key = 'f';
				break;
			case 'b':
			case 'B':
				g_WindowInput.key = 'b';
				break;
			case 'g':
			case 'G':
				g_WindowInput.key = 'g';
				break;
			case 'm':
			case 'M':
				g_WindowInput.key = 'm';
				break;
			case 'l':
			case 'L':
				g_WindowInput.key = 'l';
				break;
			case 'o':
			case 'O':
				g_WindowInput.key = 'o';
				break;
			case 'h':
			case 'H':
				g_WindowInput.key = 'h';
				break;
			case 'i':
			case 'I':
				g_WindowInput.key = 'i';
				break;
			default:
				g_WindowInput.key = 0;
				break;
			}
		}
		break;
	case WM_KEYUP:                // Key released
		return 0;
	case WM_LBUTTONDOWN:
		GetCursorPos(&currMousePos);
		ScreenToClient(hWnd,&currMousePos);
		g_WindowInput.mouseX = currMousePos.x;
		g_WindowInput.mouseY = currMousePos.y;
		g_WindowInput.but1 = true;
		break;
	case WM_RBUTTONDOWN:
		GetCursorPos(&currMousePos);
		ScreenToClient(hWnd,&currMousePos);
		g_WindowInput.mouseX = currMousePos.x;
		g_WindowInput.mouseY = currMousePos.y;
		g_WindowInput.but2 = true;
		break;
	case WM_PAINT:
		ValidateRect(hWnd,NULL);
		break; 
	case WM_SIZE:                 // Window Resize
		g_Application.ResizeScene(LOWORD(lParam), HIWORD(lParam));
		break;
	}

	// forward any other messages to default window procedure
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

void SetupGLFunctions() {
	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) wglGetProcAddress("glDisableVertexAttribArray");
	glUniform4fv = (PFNGLUNIFORM4FVPROC) wglGetProcAddress("glUniform4fv");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) wglGetProcAddress("glUniformMatrix4fv");
	glUniform1i = (PFNGLUNIFORM1IPROC) wglGetProcAddress("glUniform1i");
	glActiveTexture = (PFNGLACTIVETEXTUREPROC) wglGetProcAddress("glActiveTexture");
	glUseProgram = (PFNGLUSEPROGRAMPROC) wglGetProcAddress("glUseProgram");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) wglGetProcAddress("glEnableVertexAttribArray");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) wglGetProcAddress("glVertexAttribPointer");
	glBufferSubData = (PFNGLBUFFERSUBDATAPROC) wglGetProcAddress("glBufferSubData");
	glBufferData = (PFNGLBUFFERDATAPROC) wglGetProcAddress("glBufferData");
	glBindBuffer = (PFNGLBINDBUFFERPROC) wglGetProcAddress("glBindBuffer");
	glGenBuffers = (PFNGLGENBUFFERSPROC) wglGetProcAddress("glGenBuffers");
	glDeleteShader = (PFNGLDELETESHADERPROC) wglGetProcAddress("glDeleteShader");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) wglGetProcAddress("glGetUniformLocation");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC) wglGetProcAddress("glGetProgramiv");
	glLinkProgram = (PFNGLLINKPROGRAMPROC) wglGetProcAddress("glLinkProgram");
	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) wglGetProcAddress("glBindAttribLocation");
	glAttachShader = (PFNGLATTACHSHADERPROC) wglGetProcAddress("glAttachShader");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC) wglGetProcAddress("glCreateProgram");
	glGetShaderiv = (PFNGLGETSHADERIVPROC) wglGetProcAddress("glGetShaderiv");
	glCompileShader = (PFNGLCOMPILESHADERPROC) wglGetProcAddress("glCompileShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC) wglGetProcAddress("glShaderSource");
	glCreateShader = (PFNGLCREATESHADERPROC) wglGetProcAddress("glCreateShader");
	glGenerateMipmap = (PFNGLGENERATEMIPMAPEXTPROC) wglGetProcAddress("glGenerateMipmap");
	glGenFramebuffers = (PFNGLGENFRAMEBUFFERSEXTPROC) wglGetProcAddress("glGenFramebuffers");
	glBindFramebuffer = (PFNGLBINDFRAMEBUFFEREXTPROC) wglGetProcAddress("glBindFramebuffer");
	glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) wglGetProcAddress("glFramebufferTexture2D");
	glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) wglGetProcAddress("glCheckFramebufferStatus");
	glClearDepthf = (PFNGLCLEARDEPTHFPROC) wglGetProcAddress("glClearDepthf");
	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONARBPROC) wglGetProcAddress("glGetAttribLocation");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC) wglGetProcAddress("glDeleteProgram");
	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) wglGetProcAddress("glGetProgramInfoLog");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) wglGetProcAddress("glGetShaderInfoLog");
}

int Application::InitWin() {
	m_hInstance  = GetModuleHandle(NULL);

	// Set up Window Class
	WNDCLASS wc;
	memset(&wc, 0, sizeof(wc));
	wc.lpszClassName = L"OpenGL";
	wc.hInstance     = m_hInstance;
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = (WNDPROC) WndProc;     // Our WndProc to handle messages
	wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO); // Default icon
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW); // Arrow pointer
	if (!RegisterClass(&wc)) {
		ShowError(L"Failed to register Window Class!");
		return FALSE;
	}
	DWORD  dwStyle = WS_OVERLAPPEDWINDOW;       // Window style
	DWORD  dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;         // Extended style
	// Set window size
	RECT windowRect;
	windowRect.left = (long)0;
	windowRect.right = (long)m_winWidth;
	windowRect.top = (long)0;
	windowRect.bottom = (long)m_winHeight;

	if (m_fullScreen == 2) {
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_VISIBLE | WS_SYSMENU | WS_POPUP;
	} else {
		AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);
		dwStyle |=  WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	}
	if (!(m_hWnd = CreateWindowEx(dwExStyle,
		L"OpenGL",                         // Class name
		L"GfxEmu",
		dwStyle,
		0, 0,                             // Window position
		windowRect.right-windowRect.left, // Width
		windowRect.bottom-windowRect.top, // Height
		NULL,                             // No parent
		NULL,                             // No menu
		m_hInstance,
		NULL)))
	{
		ShutdownWin();
		ShowError(L"Failed to Create Window!");
		return FALSE;
	}
	// Set Pixel Format
	if (!(m_hDC = GetDC(m_hWnd))) {
		ShutdownWin();
		ShowError(L"Could not obtain Device Context!");
		return FALSE;
	}
	static PIXELFORMATDESCRIPTOR pfd = {  // pfd tells Windows how we want things to be
		sizeof(PIXELFORMATDESCRIPTOR),
		1,                            // Version Number
		PFD_DRAW_TO_WINDOW |          // Require rendering to window support
		PFD_SUPPORT_OPENGL |          // Require OpenGL support
		PFD_DOUBLEBUFFER,             // Require double buffering
		PFD_TYPE_RGBA,                // Request RGBA format
		8,                         // Color depth
		0, 0, 0, 0, 0, 0,             // Color bits ignored
		0,                            // No alpha buffer
		0,                            // Shift bit ignored
		0,                            // No accumulation buffer
		0, 0, 0, 0,                   // Accumulation bits ignored
		24,                           // 16Bit Z-Buffer (Depth buffer)  
		0,                            // No stencil buffer
		0,                            // No auxiliary buffer
		PFD_MAIN_PLANE,               // Main drawing layer
		0,                            // Reserved
		0, 0, 0                       // Layer masks ignored
	};
	GLuint pixelFormat;              // See if we can get a matching PixelFormat
	if (!(pixelFormat = ChoosePixelFormat(m_hDC, &pfd))) {
		ShutdownWin();
		ShowError(L"No suitable PixelFormat!");
		return FALSE;
	}
	if (!SetPixelFormat(m_hDC, pixelFormat, &pfd)) {
		ShutdownWin();
		ShowError(L"Failed to set PixelFormat!");
		return FALSE;
	}

	// Create and activate GL rendering context
	if (!(m_hRC = wglCreateContext(m_hDC))) {
		ShutdownWin();
		ShowError(L"Failed to create GL rendering context!");
		return FALSE;
	}
	if (!wglMakeCurrent(m_hDC, m_hRC)) {
		ShutdownWin();
		ShowError(L"Failed to activate GL rendering context.");
		return FALSE;
	}
	/*
	if (g_MSAA) {
	wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)wglGetProcAddress("wglGetPixelFormatAttribivARB"); 
	DumpPixelFormat(pixelFormat);

	myprintf("pixelFormat=0x%x\n", pixelFormat);
	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB"); 
	if (wglChoosePixelFormatARB) {

	int pixelFormat_msaa;
	BOOL bStatus;
	UINT numFormats;
	float fAttributes[] = {0,0};
	int iAttributes[] = { 
	WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
	WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
	WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
	WGL_COLOR_BITS_ARB,bits,
	//      WGL_ALPHA_BITS_ARB,8,
	WGL_DEPTH_BITS_ARB,g_DepthBuffer,
	WGL_STENCIL_BITS_ARB,0,
	WGL_DOUBLE_BUFFER_ARB,GL_TRUE,
	WGL_SAMPLE_BUFFERS_ARB,GL_TRUE,
	WGL_SAMPLES_ARB,g_MSAA,
	0,0};
	bStatus = wglChoosePixelFormatARB(hDC,iAttributes, fAttributes,1,&pixelFormat_msaa,&numFormats);
	if ((bStatus == GL_TRUE) && (numFormats>0)) {
	myprintf("pixelFormat=0x%x\n", pixelFormat_msaa);
	ShutdownWin();
	if (!RegisterClass(&wc)) {
	ShowError("Failed to register Window Class!");
	return FALSE;
	}
	if (!(hWnd = CreateWindowEx(dwExStyle,
	"OpenGL",                         // Class name
	GetStringScene(),
	dwStyle,
	0, 0,                             // Window position
	windowRect.right-windowRect.left, // Width
	windowRect.bottom-windowRect.top, // Height
	NULL,                             // No parent
	NULL,                             // No menu
	m_hInstance,
	NULL)))
	{
	ShutdownWin();
	ShowError("Failed to Create Window!");
	return FALSE;
	}
	// Set Pixel Format
	if (!(hDC = GetDC(hWnd))) {
	KillGLWindow();
	ShowError("Could not obtain Device Context!");
	return FALSE;
	}
	pixelFormat = pixelFormat_msaa;
	DumpPixelFormat(pixelFormat);
	if (!SetPixelFormat(hDC, pixelFormat, &pfd)) {
	KillGLWindow();
	ShowError("Failed to set PixelFormat!");
	return FALSE;
	}
	// Create and activate GL rendering context
	if (!(hRC = wglCreateContext(hDC))) {
	KillGLWindow();
	ShowError("Failed to create GL rendering context!");
	return FALSE;
	}
	if (!wglMakeCurrent(hDC, hRC)) {
	KillGLWindow();
	ShowError("Failed to activate GL rendering context.");
	return FALSE;
	}
	if (wglGetPixelFormatAttribivARB) {
	int qu = WGL_SAMPLES_ARB;
	wglGetPixelFormatAttribivARB(hDC, pixelFormat, 0, 1, (const int*)&qu, &g_MSAA);
	}
	glEnable(GL_MULTISAMPLE_ARB);

	}
	}
	}
	*/
	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

	// Load needed OpenGL extension functions
	SetupGLFunctions();

	return 1;
}

void Application::ShutdownWin() {
	if (m_hRC) {
		if (!wglMakeCurrent(NULL, NULL)) {
			ShowError(L"Failed to release DC And RC!");
		}
		if (!wglDeleteContext(m_hRC)) {
			ShowError(L"Failed to release Rendering Context!");
		}
		m_hRC = NULL;
	}
	if (m_hDC && !ReleaseDC(m_hWnd,m_hDC)) {
		ShowError(L"Failed to release Device Context!");
		m_hDC = NULL;
	}
	if (m_hWnd && !DestroyWindow(m_hWnd)) {
		ShowError(L"Could not Destroy Window!");
		m_hWnd = NULL;
	}
	if (!UnregisterClass(L"OpenGL", m_hInstance)) {
		ShowError(L"Could not Unregister Window Class!");
		m_hInstance = NULL;
	}
}

void Application::Draw() {
	// reset player direction (so that there is not continuous movement)
	direction = 0.0f;
	mouseX = 0.0f;
	mouseY = 0.0f;

	// Draw City
	m_city.Draw(g_Application.camera);

	// Swap buffers
	SwapBuffers(m_hDC);

	// Update redrawHappened
	m_redrawHappened = true;
}