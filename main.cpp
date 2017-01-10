#include "Common.h"

#include "Util.h"
#include "Random.h"
#include "Application.h"
#include "Config.h"

extern int g_PAUL;

// Application Global
Application       g_Application;
int g_MaxFps = 0;
int g_SwapI = 0;
int g_PX = 0;
int g_PY = 0;
extern int serverPort;
extern int recvPort;
char g_FPSString[255] = {""};
double minFrameTime = 1.0; 
bool g_StatsHud = false;

void SetFps(int val)
{
	if (g_MaxFps) {
		minFrameTime = 1.0 / (double)g_MaxFps;
	}
}

static void usage(void)
{
	printf("Usage:\n");
	printf("  -x <xsize>                  Set Width\n");
	printf("  -y <xsize>                  Set Height\n");
	printf("  -px <posx>                  Set X Position\n");
	printf("  -py <posy>                  Set Y Position\n");
	printf("  -sw <interval>              Swap interval\n");
	printf("  -p <udp_port>               UDP Port (default: 0  OFF)\n");
	printf("  -m <max_fps>                Target maximum FPS (throttling)\n");
}


static int parsecmdline(int argc, char *argv[])
{
	int i;
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-x") == 0) {
			//      g_WinWidth = atoi(argv[i+1]);
			i++;
		}
		else if (strcmp(argv[i], "-y") == 0) {
			//      g_WinHeight = atoi(argv[i+1]);
			i++;
		}
		else if (strcmp(argv[i], "-m") == 0) {
			g_MaxFps = atoi(argv[i+1]);
			i++;
		}
		else if (strcmp(argv[i], "-sw") == 0) {
			g_SwapI = atoi(argv[i+1]);
			i++;
		}
		else if (strcmp(argv[i], "-px") == 0) {
			g_PX  = atoi(argv[i+1]);
			i++;
		}
		else if (strcmp(argv[i], "-py") == 0) {
			g_PY  = atoi(argv[i+1]);
			i++;
		}
		else if (strcmp(argv[i], "-p") == 0) {
			serverPort  = atoi(argv[i+1]);
			i++;
		}
                else if (strcmp(argv[i], "-P") == 0) {
                        recvPort  = atoi(argv[i+1]);
                        i++;
                }
		else {
			printf("Don't understand '%s', please check this:\n", argv[i]);
			usage();
			return -1;
		}
	}
	return 0;
}

// Linux main()
#ifdef __linux__
int main( int argc, char *argv[] ) {
	struct timeval t1, t2, t3;
	struct timezone tz;
	float deltatime, frametime;
	float totaltime = 0.0f;
	unsigned int frames = 0;
    unsigned int maxFrameRate = 0;
    unsigned int minFrameRate = 100000;
    unsigned int benchMarkFrames = 0;
    float benchMarkTime = 0;
    float averageFrameRate = 0.0;

	if (parsecmdline(argc, argv)) {
		return -1;
	}

	Config *t_Config = Config::GetInstance();

	if (t_Config->LoadXml("grfxEmul.xml")) {
		printf("Error with grfxEmul.xml !\n");
		return -1;
	}

	//RandomInit(time(NULL));
	RandomInit(1);

	gettimeofday ( &t1 , &tz );
	printf("## Start gfxEmu: %s\n", GetTimeString());

	// Init application
	if ( !g_Application.Init() )
		return 0;

	printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
	printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
	printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
	printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
#ifndef USE_GL
	printf("EGL_VENDOR    = %s\n", (char *) eglQueryString(g_Application.g_EglDisplay, EGL_VENDOR));
	printf("EGL_VERSION   = %s\n", (char *) eglQueryString(g_Application.g_EglDisplay, EGL_VERSION));
	printf("EGL_EXTENSIONS= %s\n", (char *) eglQueryString(g_Application.g_EglDisplay, EGL_EXTENSIONS));
#endif
	SetFps(g_MaxFps);

    if(g_Application.m_benchMarkMode)
    {
        printf("\n\n*** BENCHMARK MODE ***\n\n");
    }



	// Enter Main Render Loop
	while (!g_Application.exitApp) {
		//ring_reset(&render_ring);
		//ring_sample(&render_ring);

		// Get current time
		gettimeofday(&t2, &tz);
		deltatime = (float)(t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) * 1e-6);
		if (deltatime>1.0) {
			printf(" Abnormal Delta time = %8.4f\n", deltatime);
			deltatime = 1.0;
		}
		t1 = t2;

		// Check for input
		g_Application.ProcessInput();

		// Call Update function
		g_Application.Update(deltatime);

		// Call Draw function
		g_Application.Draw();

		// Call Post Draw function (compare for validation)
		g_Application.PostDraw();

		// Lock FPS
		if (g_MaxFps) {
			gettimeofday(&t3, &tz);
			frametime = (float)(t3.tv_sec - t1.tv_sec + (t3.tv_usec - t1.tv_usec) * 1e-6);
			if(frametime < minFrameTime) {
				//printf("frametime = %1.4f\n", frametime);
				//printf("sleeping %d useconds\n", static_cast<unsigned int>(1000000.0*(minFrameTime-frametime)));
				usleep(static_cast<unsigned int>(1000000.0*(minFrameTime-frametime)));
			}
		}

		// Calculate FPS
		// Ensure that time lapses do not cause problems (if more than an hour has passed, don't increase totaltime)
		// This can happen when system time gets updated by network after app starts
		// May cause a lower FPS report than actual for the instance where time catchs up
		if(deltatime < 360.0) {
			totaltime += deltatime;
            benchMarkTime += deltatime;
		}
		frames++;
        benchMarkFrames++;

        if (totaltime >  1.0f)
		{
			if (g_MaxFps) {
				sprintf(g_FPSString, "%6.2f / %d", frames/totaltime, g_MaxFps);
			} else {
				sprintf(g_FPSString, "%6.2f", frames/totaltime);
			}
            if(!g_Application.m_benchMarkMode)
                printf("%4d frames rendered in %1.4f seconds -> FPS=%3.4f\n", frames, totaltime, frames/totaltime);
            if(frames/totaltime > maxFrameRate)
                maxFrameRate = frames/totaltime;
            // For minFrameRate, only start measuring after 3 seconds to allow for setup
            if(frames/totaltime < minFrameRate && benchMarkTime > 3.0)
                minFrameRate = frames/totaltime;

			totaltime -= 1.0f;
			frames = 0;
		}

	}

    // Dump benchmark stats
    if(g_Application.m_benchMarkMode) {
        averageFrameRate = benchMarkFrames / benchMarkTime;
        printf("Run Statistics:\n");
        printf("\tMax FPS                : %d\n", maxFrameRate);
        printf("\tMin FPS                : %d\n", minFrameRate);
        printf("\tTotal Frames Processed : %d\n", benchMarkFrames);
        printf("\tTime Elapsed           : %f.2s\n", benchMarkTime);
        printf("\tAverage FPS            : %f\n", averageFrameRate);
    }

	// Shutdown
	g_Application.Shutdown();
#ifdef __USE_SYSTEM
    // Delete procID file
//    system("/usr/bin/rm -f ./.grfxEmulProcID");
//    system("/usr/bin/rm -f ./.grfxCpuUtil");
    system("/usr/bin/unlink ./.grfxEmulProcID");
    system("/usr/bin/unlink ./.grfxCpuUtil");
#endif
}
#endif

// Windows WinMain()
#if defined(_WIN32) || defined(WIN32)

// Define OpenGL functions
#include "Windows/OGLDefs.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Initialize the gl functions
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = NULL;
PFNGLUNIFORM4FVPROC glUniform4fv = NULL;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = NULL;
PFNGLUNIFORM1IPROC glUniform1i = NULL;
PFNGLACTIVETEXTUREPROC glActiveTexture = NULL;
PFNGLUSEPROGRAMPROC glUseProgram = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NULL;
PFNGLBUFFERSUBDATAPROC glBufferSubData = NULL;
PFNGLBUFFERDATAPROC glBufferData = NULL;
PFNGLBINDBUFFERPROC glBindBuffer = NULL;
PFNGLGENBUFFERSPROC glGenBuffers = NULL;
PFNGLDELETESHADERPROC glDeleteShader = NULL;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
PFNGLGETPROGRAMIVPROC glGetProgramiv = NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation = NULL;
PFNGLATTACHSHADERPROC glAttachShader = NULL;
PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
PFNGLGETSHADERIVPROC glGetShaderiv = NULL;
PFNGLCOMPILESHADERPROC glCompileShader = NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NULL;
PFNGLCREATESHADERPROC glCreateShader = NULL;
PFNGLGENERATEMIPMAPEXTPROC glGenerateMipmap = NULL;
PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffers = NULL;
PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebuffer = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2D = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatus = NULL;
PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB = NULL;
PFNGLCLEARDEPTHFPROC glClearDepthf = NULL;
PFNGLGETATTRIBLOCATIONARBPROC glGetAttribLocation = NULL;
PFNGLDELETEPROGRAMPROC glDeleteProgram = NULL;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = NULL;

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//int   g_TargetFrameTime_us;  
	//int   g_SleepGranularity = 1;
	unsigned __int64 t1;   
	unsigned __int64 t2;   
	unsigned __int64 t3;  
	unsigned __int64 tCountsPerSec;
	float deltatime, frametime;
	float totaltime = 0.0f;
	unsigned int frames = 0;

	/*
	// Some Default Values and Parse Command Line Arguments
	LPTSTR argv[20] = { TEXT("") };
	int argc = 20;
	g_DisplayWidth  = GetSystemMetrics(SM_CXSCREEN);
	g_DisplayHeight = GetSystemMetrics(SM_CYSCREEN);
	myprintf("Display %dx%d\n", g_DisplayWidth, g_DisplayHeight);
	g_WinWidth = GetDefaultWidth();
	g_WinHeight = GetDefaultHeight();
	CommandLineToArgs(lpCmdLine, &argc, &argv[0]);  
	if (parsecmdline(argc, argv)) {
	return -1;
	}
	*/

	// Load config file
	Config *t_Config = Config::GetInstance();
	if (t_Config->LoadXml("grfxEmul.xml")) {
		printf("Error with grfxEmul.xml !\n");
		return -1;
	}

	// Init application

	printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
	printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
	printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
	printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
#ifdef __linux__
	printf("EGL_VENDOR    = %s\n", (char *) eglQueryString(g_Application.g_EglDisplay, EGL_VENDOR));
	printf("EGL_VERSION   = %s\n", (char *) eglQueryString(g_Application.g_EglDisplay, EGL_VERSION));
	printf("EGL_EXTENSIONS= %s\n", (char *) eglQueryString(g_Application.g_EglDisplay, EGL_EXTENSIONS));
#endif

	// Initialize RNG
	RandomInit(1);
	
	// Setup timers
	QueryPerformanceFrequency((LARGE_INTEGER *)&tCountsPerSec);
	if (tCountsPerSec == 0) {
		printf("tCountsPerSec issue !");
		return -1;
	}

	// Take initial time reference
	QueryPerformanceCounter((LARGE_INTEGER *) &t1);

	SetFps(g_MaxFps);

	// Init application
	if ( !g_Application.Init() )
		return 0;

	// Enter Main Render Loop
	while (!g_Application.exitApp) {

		// Get current time
		QueryPerformanceCounter((LARGE_INTEGER *) &t2);
		deltatime = (float) (t2 - t1) / tCountsPerSec;
		if (deltatime>1.0) {
			printf(" Abnormal Delta time = %8.4f\n", deltatime);
			deltatime = 1.0;
		}
		t1 = t2;

		// Check for input
		g_Application.ProcessInput();

		// Call Update function
		g_Application.Update(deltatime);

		// Call Draw function
		g_Application.Draw();

		// Call Post Draw function (compare for validation)
		g_Application.PostDraw();

		// Lock FPS
		if (g_MaxFps) {
			QueryPerformanceCounter((LARGE_INTEGER *) &t3);
			frametime = (float) (t3 - t1) / tCountsPerSec;
			if(frametime < minFrameTime) {
				//printf("frametime = %1.4f\n", frametime);
				//printf("sleeping %d useconds\n", static_cast<unsigned int>(1000000.0*(minFrameTime-frametime)));
				//usleep(static_cast<unsigned int>(1000000.0*(minFrameTime-frametime)));
				Sleep(static_cast<DWORD> (minFrameTime - frametime)/1000);
			}
		}

		// Calculate FPS
		// Ensure that time lapses do not cause problems (if more than an hour has passed, don't increase totaltime)
		// This can happen when system time gets updated by network after app starts
		// May cause a lower FPS report than actual for the instance where time catchs up
		if(deltatime < 360.0) {
			totaltime += deltatime;
		}
		frames++;
		if (totaltime >  1.0f)
		{
			if (g_MaxFps) {
				sprintf(g_FPSString, "%6.2f / %d", frames/totaltime, g_MaxFps);
			} else {
				sprintf(g_FPSString, "%6.2f", frames/totaltime);
			}
			printf("%4d frames rendered in %1.4f seconds -> FPS=%3.4f\n", frames, totaltime, frames/totaltime);
			totaltime -= 1.0f;
			frames = 0;
		}
	}

	// Shutdown
	g_Application.Shutdown();

	return 0;
}
#endif

