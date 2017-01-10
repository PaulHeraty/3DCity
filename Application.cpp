#include "Application.h"

#ifdef __linux__
#include "UDPSocket.h"
UDPSocket   *g_Sock_p =  NULL;
std::string serverAddr = "localhost";
unsigned int serverPort = 0;
unsigned int recvPort = 0;
#endif

int got_Master = 0;
// Global window input var
winInput g_WindowInput;

// Wireframe modes: 0=regular, 1=solid, 2=transparent
unsigned int g_wireframe = 0;
unsigned int g_funcTest = 0;
bool g_depthComplexity = false;
bool g_validationMode = false;
bool g_drawValBars = false;
bool g_reportDC = false;
bool g_drawAABB = false;
APP_MODE g_appMode = HOME_MODE;
bool g_appModeSwitching = false;
bool g_showHotzones = false;
bool g_HudSwitch = false;
//bool g_StatsHud = false;
bool g_ShowHudMenu = false;
bool g_VirtualPos = true;
HUD_TYPE g_navHud = NO_HUD;
unsigned int g_numBuildingDraws = 0;
unsigned int g_numSignDraws = 0;
unsigned int g_numLightDraws = 0;
unsigned int g_numStreetlightDraws = 0;
unsigned int g_numRoadDraws = 0;
unsigned int g_numOtherDraws = 0;
int g_DoDraw = 1;
float g_winWidth;
float g_winHeight;
bool wasOnAutoPilot = true;

// CES HUD Globals
bool g_CESSeatHeatL = true;
bool g_CESSeatHeatR = false;
bool g_CESHeatMax = false;
bool g_CESHeatRear = true;
bool g_CESHeatFront = false;
bool g_CESAc = true;
bool g_CESAuto = false;
unsigned int g_CESAir = 1;
int g_CESFan = 5;
int g_CESTempL = 6;
int g_CESTempR = 3;

extern int g_MaxFps;
extern void SetFps(int val);

void DispatchCmd(const char *cmd)
{
	printf("DispatchCmd=%s\n", cmd);
	if (cmd[0] == ':') {
		if (strcmp(cmd, ":TrafficCB") ==0) {
			if(g_Application.m_autoPilot == false)
				g_Application.m_autoPilot = true;
			else
				g_Application.m_autoPilot = false;
			return;
		}
		if (strcmp(cmd, ":ExitCB") ==0) {
			g_Application.exitApp = true;
			return;
		}
		if (strcmp(cmd, ":Stats") ==0) {
			if(!g_StatsHud) {
				g_StatsHud = true;
				if(g_ShowHudMenu)
					g_ShowHudMenu  = false ;

				// CPU & GPU Util
				g_Application.m_city.m_huds_updater[3]->UpdateText(2,27,30.0, "WIP");
				g_Application.m_city.m_huds_updater[2]->UpdateText(2,27,30.0, "WIP");

#ifdef __USE_SYSTEM
#ifdef __linux__
				// Find the process number
				unsigned int procNum = 0;
				float cpuUtil = 0.0;
				char getCpuUtil[50] = "";
				char cpuUtilStr[10];
				std::ifstream infile;
				std::string procIdString, cpuUtilString;
				system("/usr/bin/ps -a | grep grfxEmul > ./.grfxEmulProcID");
				infile.open("./.grfxEmulProcID");
				getline(infile, procIdString);
				std::istringstream issProcId(procIdString);
				issProcId >> procNum;
				infile.close();
				sprintf(getCpuUtil, "/usr/bin/ps -p %d -o \\%%cpu= > ./.grfxCpuUtil\n", procNum);


				// CPU Util
				system(getCpuUtil);
				infile.open("./.grfxCpuUtil");
				getline(infile, cpuUtilString);
				std::istringstream issCpuUtil(cpuUtilString);
				issCpuUtil >> cpuUtil;
				sprintf(cpuUtilStr, "%4.2f%%", cpuUtil);
				infile.close();
				g_Application.m_city.m_huds_updater[2]->UpdateText(2,27,30.0, cpuUtilStr);
#endif
#endif
			} else {
				g_StatsHud = false;
			}
			return;
		}
		if (strcmp(cmd, ":QuadTree") ==0) {
			if(g_drawAABB)
				g_drawAABB = false;
			else
				g_drawAABB = true;
			return;
		}
		if (strcmp(cmd, ":NavCB") ==0) {
			g_appMode = NAV_MODE;
			g_appModeSwitching = true;
			g_Application.m_autoPilot = true;
			g_Application.m_city.m_baseTree.m_cars.m_draw = false;
			return;
		}
		if (strcmp(cmd, ":HomeCB") ==0) {
			if (g_appMode == HOME_MODE) {
				//	g_HudSwitch = true;
				g_ShowHudMenu  = !g_ShowHudMenu ;
				if(g_ShowHudMenu)
					g_StatsHud = false;
			} else {
				g_appMode = HOME_MODE;
				g_appModeSwitching = true;
			}
			g_Application.m_city.m_baseTree.m_cars.m_draw = true;
			return;
		}
		if (strcmp(cmd, ":WfCB") ==0) {
			if(g_wireframe == 2)
				g_wireframe = 0;
			else if(g_wireframe == 0)
				g_wireframe = 1;
			else
				g_wireframe = 2;
			return;
		}
		if (strcmp(cmd, ":DCCB") ==0) {
			if(g_depthComplexity)
				g_depthComplexity = false;
			else
				g_depthComplexity = true;
			return;
		}
		if (strcmp(cmd, ":Plus") ==0) {
			if (g_MaxFps) {
				g_MaxFps += 5;
			} else {
				g_MaxFps = 30; // Jump by default to 30
			}
			SetFps(g_MaxFps);
			return;
		}
		if (strcmp(cmd, ":Minus") ==0) {
			g_MaxFps -= 5;
			if (g_MaxFps<0) g_MaxFps = 0;
			SetFps(g_MaxFps);
			return;
		}
		if (strcmp(cmd, ":CESSeatHeatL") ==0) {
			g_CESSeatHeatL = !g_CESSeatHeatL;
			g_Application.m_city.m_huds[CES_HUD]->UpdateCES_HUD();
			return;
		}
		if (strcmp(cmd, ":CESSeatHeatR") ==0) {
			g_CESSeatHeatR = !g_CESSeatHeatR;
			g_Application.m_city.m_huds[CES_HUD]->UpdateCES_HUD();
			return;
		}
		if (strcmp(cmd, ":CESHeatMax") ==0) {
			g_CESHeatMax = !g_CESHeatMax;
			g_Application.m_city.m_huds[CES_HUD]->UpdateCES_HUD();
			return;
		}
		if (strcmp(cmd, ":CESHeatRear") ==0) {
			g_CESHeatRear = !g_CESHeatRear;
			g_Application.m_city.m_huds[CES_HUD]->UpdateCES_HUD();
			return;
		}
		if (strcmp(cmd, ":CESHeatFront") ==0) {
			g_CESHeatFront = !g_CESHeatFront;
			g_Application.m_city.m_huds[CES_HUD]->UpdateCES_HUD();
			return;
		}
		if (strcmp(cmd, ":CESAc") ==0) {
			g_CESAc = !g_CESAc;
			g_Application.m_city.m_huds[CES_HUD]->UpdateCES_HUD();
			return;
		}
		if (strcmp(cmd, ":CESAuto") ==0) {
			g_CESAuto = !g_CESAuto;
			g_Application.m_city.m_huds[CES_HUD]->UpdateCES_HUD();
			return;
		}
		if (strcmp(cmd, ":CESAirDown") ==0) {
			g_CESAir = 0;
			g_Application.m_city.m_huds[CES_HUD]->UpdateCES_HUD();
			return;
		}
		if (strcmp(cmd, ":CESAirStraight") ==0) {
			g_CESAir = 1;
			g_Application.m_city.m_huds[CES_HUD]->UpdateCES_HUD();
			return;
		}
		if (strcmp(cmd, ":CESAirUp") ==0) {
			g_CESAir = 2;
			g_Application.m_city.m_huds[CES_HUD]->UpdateCES_HUD();
			return;
		}
		if (strcmp(cmd, ":CESFanUp") ==0) {
			g_CESFan++;
			if(g_CESFan > 10)
				g_CESFan = 10;
			g_Application.m_city.m_huds[CES_HUD]->UpdateCES_HUD();
			return;
		}
		if (strcmp(cmd, ":CESFanDown") ==0) {
			g_CESFan--;
			if(g_CESFan < 0)
				g_CESFan = 0;
			g_Application.m_city.m_huds[CES_HUD]->UpdateCES_HUD();
			return;
		}
		if (strcmp(cmd, ":CESTempRUp") ==0) {
			g_CESTempR++;
			if(g_CESTempR > 10)
				g_CESTempR = 10;
			g_Application.m_city.m_huds[CES_HUD]->UpdateCES_HUD();
			return;
		}
		if (strcmp(cmd, ":CESTempRDown") ==0) {
			g_CESTempR--;
			if(g_CESTempR < 0)
				g_CESTempR = 0;
			g_Application.m_city.m_huds[CES_HUD]->UpdateCES_HUD();
			return;
		}
		if (strcmp(cmd, ":CESTempLUp") ==0) {
			g_CESTempL++;
			if(g_CESTempL > 10)
				g_CESTempL = 10;
			g_Application.m_city.m_huds[CES_HUD]->UpdateCES_HUD();
			return;
		}
		if (strcmp(cmd, ":CESTempLDown") ==0) {
			g_CESTempL--;
			if(g_CESTempL < 0)
				g_CESTempL = 0;
			g_Application.m_city.m_huds[CES_HUD]->UpdateCES_HUD();
			return;
		}
		if (strcmp(cmd, ":CESShowHMI") ==0) {
			if(!g_ShowHudMenu)
				g_ShowHudMenu  = true ;
			return;
		}
		if (strcmp(cmd, ":CESHideHMI") ==0) {
			if(g_ShowHudMenu)
				g_ShowHudMenu  = false ;
			return;
		}
		if (strcmp(cmd, ":DoDraw") ==0) {
			g_DoDraw = 1;
			return;
		}
		if (strncmp(cmd, ":Touch", 6) ==0) {
			int t_X, t_Y;
			if (sscanf(cmd, ":Touch.%d.%d", &t_X, &t_Y) == 2) {
				g_WindowInput.but1 = true;
				g_WindowInput.mouseX = t_X;
				g_WindowInput.mouseY = t_Y;
				if ((g_winWidth == 1920.0) && (g_winHeight == 1080.0)) {
				} else {
					g_WindowInput.mouseX = (int)((float)t_X*(float)g_winWidth/1920.0);
					g_WindowInput.mouseY = (int)((float)t_Y*(float)g_winHeight/1080.0);
				}
				printf("Touch: %d %d - %d %d\n", t_X, t_Y, g_WindowInput.mouseX, g_WindowInput.mouseY);
			}
			return;
		}

#ifdef __linux__
		if (strncmp(cmd, ":Master.",8) ==0) {
			char cmd2[255];
			if (!got_Master) {
				got_Master = 1;
				serverAddr = &cmd[8];
			}
			sprintf(cmd2, ":Alive");
			printf("%s at %s:%d\n", cmd2, serverAddr.c_str(), serverPort);
			if (g_Sock_p) {
				g_Sock_p->send(cmd2, strlen(cmd2),  serverAddr, serverPort);
			}
			return ;
		}
#endif

		printf("ERROR: Undefine internal cmd: %s\n", cmd);
		return;
	}


#ifdef __linux__
	if (g_Sock_p) {
#if 0 // CES Hack - mplayer full screen - we don't draw anymore...
		if (strcmp(cmd, "VideoCmd2") ==0) {
			g_DoDraw = 0;    
		}
#endif
		g_Sock_p->send(cmd, strlen(cmd),  serverAddr, serverPort);
	}
#endif
}



Application::Application() {
	m_winWidth = 1920;
	m_winHeight = 1080;
	m_rotate = NONE;
	m_turning = false;
	m_compareSnap = false;
	m_dumpSnap = false;
	m_redrawHappened = false;
	m_fullScreen = 0;
	m_FXAApostProc = 0;
//    m_benchMarkMode = false;

#ifdef USE_X11
	g_Framework_X11Display = 0;
	g_Framework_X11Window = 0;
#endif

#ifdef USE_WAY
	g_WaylandDisplay = {0};
	g_WaylandWindow  = {0};
#endif

#if defined(_WIN32) || defined(WIN32)
	m_hDC = NULL;     // GDI Device Context
	m_hRC = NULL;     // Permanent Rendering Context
	m_hWnd = NULL;    // Our Window Handle
#endif
}

Application::~Application() {
}

int Application::Init() {
#if defined(WIN_XML) || defined(USE_XML)
	Config *t_Config = Config::GetInstance();
	t_Config->Helper("/grfxEmul/application", &m_AppHelper);
	m_winWidth = m_AppHelper.m_Width;
	m_winHeight = m_AppHelper.m_Height;
	g_winWidth =  (float)m_winWidth;
	g_winHeight = (float)m_winHeight;
	if (m_AppHelper.m_VP == 0)
		g_VirtualPos = false;
    m_benchMarkMode = m_AppHelper.m_BMM != 0 ? true : false;

    if(m_benchMarkMode)
    {
        m_city.m_drawHud = true;
        g_ShowHudMenu = true;
    }

#else
	m_winWidth  = 800;
	m_winHeight = 600;
	g_winWidth =  (float)m_winWidth;
	g_winHeight = (float)m_winHeight;
	g_VirtualPos = false;
#endif


	exitApp = false;
	g_WindowInput.key = 0;
	g_WindowInput.mouseX = 0;
	g_WindowInput.mouseY = 0;
	g_WindowInput.but1 = false;
	g_WindowInput.but2 = false;
	m_autoPilot = true;
	m_destination = glVector(0.0, 0.0, 0.0);

#ifdef __linux__
	if (serverPort) {
		if (recvPort) {
			g_Sock_p = new UDPSocket(recvPort);
		} else {
			g_Sock_p = new UDPSocket();
		}
		if (!g_Sock_p) {
			printf("Socket error\n");
			return 0;
		}
		printf("UDP enabled ! (%d) %d\n", serverPort, recvPort);
	} else {
		printf("No UDP support\n");
	}
#endif

	// Initialize Windows (X11 or Walyand)
	if(!InitWin())
		return 0;

	// Initialize FBO for post-processing
	m_FBO_texWidth = m_winWidth;
	m_FBO_texHeight = m_winHeight;
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &m_FBO_maxRenderbufferSize);
	// check if GL_MAX_RENDERBUFFER_SIZE is >= window width/height
	if((m_FBO_maxRenderbufferSize <= m_FBO_texWidth) ||
		(m_FBO_maxRenderbufferSize <= m_FBO_texHeight)) {
			// cannot use framebuffer objects as we need to create
			// a depth buffer as a renderbuffer object
			// return with appropriate error
			printf("Cannot create FBO objects as big as window size - ERROR");
			return 0;
	}
	// generate the framebuffer, renderbuffer and texture object names
	glGenFramebuffers(1, &m_FBO_framebuffer);
	glGenRenderbuffers(1, &m_FBO_depthRenderbuffer);
	glGenTextures(1, &m_FBO_texture);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO_framebuffer);
	// bind textures and load the texture mip-level 0
	// texels are RGB888
	// no texels need to be specified as we are going to draw into the texture
	glBindTexture(GL_TEXTURE_2D, m_FBO_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_FBO_texWidth, m_FBO_texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// bind renderbuffer and create a 24-bit depth buffer
	glBindRenderbuffer(GL_RENDERBUFFER, m_FBO_depthRenderbuffer);
#ifdef __unix__
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES, m_FBO_texWidth, m_FBO_texHeight);
#elif defined(_WIN32) || defined(WIN32)
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_FBO_texWidth, m_FBO_texHeight);
#endif
	// Init vertices for screen-quad (x, y, z, u, v)
	m_FBO_verts[0] = -1.0f; m_FBO_verts[1] = -1.0f; m_FBO_verts[2] = 0.0f; m_FBO_verts[3] = 0.0f; m_FBO_verts[4] = 0.0f;
	m_FBO_verts[5] = -1.0f; m_FBO_verts[6] =  1.0f; m_FBO_verts[7] = 0.0f; m_FBO_verts[8] = 0.0f; m_FBO_verts[9] = 1.0f;
	m_FBO_verts[10] =  1.0f; m_FBO_verts[11] =  1.0f; m_FBO_verts[12] = 0.0f; m_FBO_verts[13] = 1.0f; m_FBO_verts[14] = 1.0f;
	m_FBO_verts[15] =  1.0f; m_FBO_verts[16] = -1.0f; m_FBO_verts[17] = 0.0f; m_FBO_verts[18] = 1.0f; m_FBO_verts[19] = 0.0f;
	// Init indices for screen-quad
	m_FBO_index[0] = 0; m_FBO_index[1] = 2; m_FBO_index[2] = 1;
	m_FBO_index[3] = 0; m_FBO_index[4] = 3; m_FBO_index[5] = 2;
	m_FBO_vtxStride = 5 * sizeof(GLfloat);
	// Shaders
    m_FBO_material.CreateProgram(FXAA_SHADER, "shaders/fxaa2.v", "shaders/fxaa2.f", TEXTURE_NONE);

	// specify texture as color attachment
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FBO_texture, 0);
	// specify depth_renderbuffer as depth attachement
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_FBO_depthRenderbuffer);
	// check for framebuffer complete
	m_FBO_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(m_FBO_status != GL_FRAMEBUFFER_COMPLETE) {
		printf("Problem with FBO completeness... exiting");
		exit(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

	// Create City
	m_city.Create(m_AppHelper.m_World, m_AppHelper.m_World, m_AppHelper.m_Tess, m_AppHelper.m_quadTreeSize);

	// Update Statics HUDs with static data
	// Stats Hud Text
	char buffer[1024];
	setlocale(LC_NUMERIC, "");
	// Resolution
	HUD_Updater *statText1 = new HUD_Updater(m_city.m_huds[8]);    // m_huds[8] => Stats HUD
	statText1->Create(450,120,350,30); // No check on size for now - carefull please
	sprintf(buffer, "%d x %d", m_winWidth, m_winHeight);
	statText1->UpdateText(2,27,30.0, buffer);
	// GL vendor
	HUD_Updater *statText2 = new HUD_Updater(m_city.m_huds[8]);    // m_huds[8] => Stats HUD
	statText2->Create(450,170,350,30); // No check on size for now - carefull please
	sprintf(buffer, "%s", (char *) glGetString(GL_RENDERER));
	char* glVers = strtok(buffer, "b");
	statText2->UpdateText(2,27,30.0, glVers);
	// GL version
	HUD_Updater *statText3 = new HUD_Updater(m_city.m_huds[8]);    // m_huds[8] => Stats HUD
	statText3->Create(450,220,350,30); // No check on size for now - carefull please
	sprintf(buffer, "%s", (char *) glGetString(GL_VERSION));
	glVers = strtok(buffer, "-M");
	statText3->UpdateText(2,27,30.0, glVers);
	// Driver version
	HUD_Updater *statText4 = new HUD_Updater(m_city.m_huds[8]);    // m_huds[8] => Stats HUD
	statText4->Create(450,270,350,30); // No check on size for now - carefull please
	glVers = strtok(NULL, " ");
	glVers = strtok(NULL, " ");
	statText4->UpdateText(2,27,30.0, glVers);
	// Tess factor
	HUD_Updater *statText5 = new HUD_Updater(m_city.m_huds[8]);    // m_huds[8] => Stats HUD
	statText5->Create(450,370,350,30); // No check on size for now - carefull please
	sprintf(buffer, "%d", m_AppHelper.m_Tess);
	statText5->UpdateText(2,27,30.0, buffer);
	// QuadTree Size
	HUD_Updater *statText6 = new HUD_Updater(m_city.m_huds[8]);    // m_huds[8] => Stats HUD
	statText6->Create(450,420,350,30); // No check on size for now - carefull please
	sprintf(buffer, "%d x %d", m_AppHelper.m_quadTreeSize, m_AppHelper.m_quadTreeSize);
	statText6->UpdateText(2,27,30.0, buffer);
	// # Polygons
	HUD_Updater *statText7 = new HUD_Updater(m_city.m_huds[8]);    // m_huds[8] => Stats HUD
	statText7->Create(1200,120,300,30); // No check on size for now - carefull please
	sprintf(buffer, "%d", m_city.m_cityIndices/3);
	statText7->UpdateText(2,27,30.0, buffer);
	// # Vertices
	HUD_Updater *statText8 = new HUD_Updater(m_city.m_huds[8]);    // m_huds[8] => Stats HUD
	statText8->Create(1200,170,300,30); // No check on size for now - carefull please
	sprintf(buffer, "%d", m_city.m_cityVertices);
	statText8->UpdateText(2,27,30.0, buffer);
	// # Indices
	HUD_Updater *statText9 = new HUD_Updater(m_city.m_huds[8]);    // m_huds[8] => Stats HUD
	statText9->Create(1200,220,300,30); // No check on size for now - carefull please
	sprintf(buffer, "%d", m_city.m_cityIndices);
	statText9->UpdateText(2,27,30.0, buffer);
	// # VBOs
	HUD_Updater *statText10 = new HUD_Updater(m_city.m_huds[8]);    // m_huds[8] => Stats HUD
	statText10->Create(1200,270,300,30); // No check on size for now - carefull please
	sprintf(buffer, "%d", m_city.m_baseTree.m_numVBOs +1);
	statText10->UpdateText(2,27,30.0, buffer);
	// Vertex Memory
	HUD_Updater *statText11 = new HUD_Updater(m_city.m_huds[8]);    // m_huds[8] => Stats HUD
	statText11->Create(1200,370,300,30); // No check on size for now - carefull please
	sprintf(buffer, "%4.2f", m_city.m_cityVertices * sizeof(BaryVertexFormat2) / 1000000.0);
	statText11->UpdateText(2,27,30.0, buffer);
	// Index Memory
	HUD_Updater *statText12 = new HUD_Updater(m_city.m_huds[8]);    // m_huds[8] => Stats HUD
	statText12->Create(1200,420,300,30); // No check on size for now - carefull please
	sprintf(buffer, "%4.2f", m_city.m_cityIndices * sizeof(GLuint) / 1000000.0);
	statText12->UpdateText(2,27,30.0, buffer);
	// Bytes per Vertex
	HUD_Updater *statText13 = new HUD_Updater(m_city.m_huds[8]);    // m_huds[8] => Stats HUD
	statText13->Create(1200,470,300,30); // No check on size for now - carefull please
	sprintf(buffer, "%d", sizeof(BaryVertexFormat2));
	statText13->UpdateText(2,27,30.0, buffer);

	// Add player object
	//player.SetCameraMode(CCamera::MODE_FPS);
	player.SetCameraMode(CCamera::MODE_THIRDPERSON);
	camera = player.GetCamera();
	direction = 0;
	mouseX = 0.0f;
	mouseY = 0.0f;
	player.setFriction(250.0f);
	//g_Application->player.SetGravity( glVector(0, -400.0f, 0));
	player.SetGravity( glVector(0, 0.0f, 0));
	player.SetMaxVelocityXZ(125.0f);
	player.SetMaxVelocityY(400.0f);
	GLvector position = glVector(50.0f, 50.0f, 50.0f);
	//GLvector position = glVector(512.0f, 90.0f, 512.0f);
	player.SetPosition(position);
	player.Rotate(0.0f, 45.0, 0);
	player.SetCamOffset( glVector(0.0f, 10.0f, -40.0f));
	player.SetCamLag(0.5f);
	VOLUME_INFO Volume;
	Volume.Min = glVector(-3, -10, -3);
	Volume.Max = glVector(3, 10, 3);
	//g_Application->player.SetVolumeInfo(Volume);
	camera->SetFOV(60.0f);
	camera->SetViewport(0, 0, m_winWidth, m_winHeight, 1.0f, 2000.0f);
	camera->SetVolumeInfo(Volume);


	// Set autoPilot routes
	// Flight Path
	//m_flightPath.push_back(glVector(200.0f, 50.0f, 200.0f));
	m_flightPath.push_back(glVector(250.0f, 70.0f, 250.0f));     //0
	m_flightPath.push_back(glVector(550.0f, 120.0f, 512.0f));    //1
	m_flightPath.push_back(glVector(850.0f, 50.0f, 750.0f));     //2
	m_flightPath.push_back(glVector(850.0f, 50.0f, 800.0f));     //3
	m_flightPath.push_back(glVector(800.0f, 50.0f, 850.0f));     //4
	m_flightPath.push_back(glVector(750.0f, 50.0f, 850.0f));     //5
	m_flightPath.push_back(glVector(700.0f, 70.0f, 800.0f));     //6
	m_flightPath.push_back(glVector(560.0f, 110.0f, 512.0f));    //7
	m_flightPath.push_back(glVector(450.0f, 70.0f, 150.0f));     //8
	m_flightPath.push_back(glVector(300.0f, 50.0f, 100.0f));     //9
	m_flightPath.push_back(glVector(250.0f, 50.0f, 100.0f));     //10
	m_flightPath.push_back(glVector(200.0f, 50.0f, 150.0f));     //11
	m_flightPath.push_back(glVector(200.0f, 50.0f, 200.0f));     //12

	m_destination = m_flightPath.front();
	m_flightPath.pop_front();

#ifdef GENERATE_XML
	{
		for (int k=0; k<m_flightPath.size(); k++) {
			printf("<flighpath coord3d=\"%.3f,%.3f,%.3f\"/>\n", m_flightPath[k].x, m_flightPath[k].y, m_flightPath[k].z);
		}
	}
#endif

	return 1;
}

void Application::Update(float deltaTime) {
	navPath npTemp;
	HUD_TYPE nh;

	// Update if we have moved
	if(direction > 0) {
		player.Move(direction, 1.0f, false);
	}

	// Handle rotation
	if(m_rotate == RIGHT) {
		player.Rotate(0.0f, 45.0f, 0.0f);
		m_rotate = NONE;
	} else if(m_rotate == LEFT) {
		player.Rotate(0.0f, -45.0f, 0.0f);
		m_rotate = NONE;
	}
	if (g_HudSwitch) {
		g_HudSwitch = false;
		if(m_city.m_drawHud == false)
			m_city.m_drawHud = true;
		else
			m_city.m_drawHud = false;
	}
	// Is auto pilot engaged?
	if(m_autoPilot) {
		if(g_appMode == HOME_MODE) {
			if(g_appModeSwitching) {
				g_appModeSwitching = false;
				player.SetCamLag(0.5f);
				m_destination = m_flightPath.front();
				m_flightPath.pop_front();
                m_flightPath.push_back(m_destination);
				player.SetPosition(m_destination);
				camera->SetPosition(m_destination);
				m_destination = m_flightPath.front();
				m_flightPath.pop_front();
				m_flightPath.push_back(m_destination);
				player.SetCamOffset( glVector(0.0f, 10.0f, -40.0f));
				g_appModeSwitching = false;
				g_navHud = NO_HUD;
				m_turning = false;
			}

			// Check if next waypoint reached
			if(WaypointReached()) {
				m_turning = true;
                if(m_flightPath.size() > 0) {
                    m_destination = m_flightPath.front();
                    m_flightPath.pop_front();
                } else
                    exitApp = true;
                if(!m_benchMarkMode)
                    m_flightPath.push_back(m_destination);
			}

			// Moving
			player.SetLookAt(m_destination);
			//camera->SetLookAt(m_destination);

			GLvector remainingDist = m_destination - player.GetPosition();
			GLvector moveVector = glVectorNormalize(remainingDist);
			float speed = 20 * deltaTime;
			moveVector = moveVector * speed;

			GLvector newPos = moveVector + player.GetPosition();
			player.SetPosition(newPos);
		} else if(g_appMode == NAV_MODE) {
			if(g_appModeSwitching) {
				player.SetCamLag(0.1f);
				m_navPath.clear();
				navPath np2;
				np2.coord = glVector(805.5f, 2.0f, 264.0f);     // PosZ
				np2.hud = NAV_HUD1;
				m_navPath.push_back(np2);
				np2.coord = glVector(805.5f, 2.0f, 532.0f);
				np2.hud = NAV_HUD4;
				m_navPath.push_back(np2);
				np2.coord = glVector(805.5f, 2.0f, 669.0f);
				np2.hud = NAV_HUD5;
				m_navPath.push_back(np2);
				np2.coord = glVector(805.5f, 2.0f, 805.5f);     // NegX
				np2.hud = NAV_HUD2;
				m_navPath.push_back(np2);
				navPath np3;
				np3.coord = glVector(531.0f, 2.0f, 805.5f);
				np3.hud = NAV_HUD4;
				m_navPath.push_back(np3);
				np3.coord = glVector(394.0f, 2.0f, 805.5f);
				np3.hud = NAV_HUD5;
				m_navPath.push_back(np3);
				np3.coord = glVector(226.5f, 2.0f, 805.5f);     // NegZ
				np3.hud = NAV_HUD3;
				m_navPath.push_back(np3);
				navPath np4;
				np4.coord = glVector(226.5f, 2.0f, 531.5f);
				np4.hud = NAV_HUD4;
				m_navPath.push_back(np4);
				np4.coord = glVector(226.5f, 2.0f, 395.0f);
				np4.hud = NAV_HUD5;
				m_navPath.push_back(np4);
				np4.coord = glVector(226.5f, 2.0f, 226.5f);     // PosX
				np4.hud = NAV_HUD0;
				m_navPath.push_back(np4);
				navPath np1;
				np1.coord = glVector(532.0f, 2.0f, 226.5f);
				np1.hud = NAV_HUD4;
				m_navPath.push_back(np1);
				np1.coord = glVector(669.0f, 2.0f, 226.5f);
				np1.hud = NAV_HUD5;
				m_navPath.push_back(np1);
				np1.coord = glVector(805.5f, 2.0f, 226.5f);
				np1.hud = NAV_HUD1;
				m_navPath.push_back(np1);
				m_destination = m_navPath.front().coord;
				nh = m_navPath.front().hud;
				m_navPath.pop_front();
				navPath npTemp;
				npTemp.coord = m_destination;
				npTemp.hud = nh;
				m_navPath.push_back(npTemp);
				player.SetPosition(m_destination);
				camera->SetPosition(m_destination);
				m_destination = m_navPath.front().coord;
				nh = m_navPath.front().hud;
				m_navPath.pop_front();
				npTemp.coord = m_destination;
				npTemp.hud = nh;
				m_navPath.push_back(npTemp);
				player.SetLookAt(m_destination);
				camera->SetLookAt(m_destination);
				g_navHud = nh;
				player.SetCamOffset( glVector(0.0f, 0.0f, -0.1f));
				g_appModeSwitching = false;
			}
			// Check if next waypoint reached
			if(WaypointReached()) {
				m_destination = m_navPath.front().coord;
				nh = m_navPath.front().hud;
				m_navPath.pop_front();
				npTemp.coord = m_destination;
				npTemp.hud = nh;
				g_navHud = nh;
				m_navPath.push_back(npTemp);
			}

			player.SetLookAt(m_destination);
			//camera->SetLookAt(m_destination);

			GLvector remainingDist = m_destination - player.GetPosition();
			GLvector moveVector = glVectorNormalize(remainingDist);
			float speed = 20 * deltaTime;
			moveVector = moveVector * speed;

			// Check for overshoot

			GLvector newPos = moveVector + player.GetPosition();
			player.SetPosition(newPos);
		} // NAV_MODE
	} // if autoPilot

	if(g_appMode == FUNCTEST_MODE) {
		player.SetCamLag(0.0f);
		GLvector3 funcPos = glVector(512.0f, 70.0f, 512.0f);
		camera->SetPosition(funcPos);
		player.SetPosition(funcPos);
		GLvector3 funcLook = glVector(0.0f, 50.0f, 0.0f);
		player.SetLookAt(funcLook);
		camera->SetLookAt(funcLook);
		player.SetCamOffset( glVector(0.0f, 0.0f, 0.0f));
	}

	// Update our camera (updates velocity etc)
	player.Update(deltaTime);

	// update the device matrix
	camera->GetViewMatrix();

	// Update City (blinking lights etc)
	m_city.Update(camera, deltaTime);

	// Redraw has not yet happened
	m_redrawHappened = false;
}

void Application::PostDraw() {
	// Only test if in FUNCTEST_MODE and a redraw has happened (i.e. correct scene has been drawn)
	if(g_appMode == FUNCTEST_MODE && m_redrawHappened) {
		// Dump snapshot?
		if(m_dumpSnap) {
			DumpSnapShot();
			m_dumpSnap = false;
		}

		// If a matching file exists, then compare
		if(m_compareSnap) {
			CompareSnapShot();
			m_compareSnap = false;
		}
	}
}

bool Application::WaypointReached() {
	float xDiff, yDiff, zDiff;

	float epsilon = 1.0f;

	xDiff = fabs(m_destination.x - player.GetPosition().x);
	yDiff = fabs(m_destination.y - player.GetPosition().y);
	zDiff = fabs(m_destination.z - player.GetPosition().z);

	if(xDiff < epsilon &&
		yDiff < epsilon &&
		zDiff < epsilon)
		return true;
	else
		return false;
}

void Application::Mouse () {
	//printf("Saw a mouse! X: %d, Y: %d\n", g_WindowInput.mouseX, g_WindowInput.mouseY);
	//printf("Screen size %d x %d\n", winWidth, winHeight );

	// Check for mouse clicks in hotzones
	if(m_city.m_drawHud) {
		switch(g_appMode) {
		case(HOME_MODE):
			//m_city.m_huds[HOME_HUD]->CheckHotzones(g_WindowInput.mouseX, g_WindowInput.mouseY, m_winWidth, m_winHeight);   // HOME HUD
			m_city.m_huds[CES_HUD]->CheckHotzones(g_WindowInput.mouseX, g_WindowInput.mouseY, m_winWidth, m_winHeight);   // CES HUD
			break;
		case(NAV_MODE):
			m_city.m_huds[2]->CheckHotzones(g_WindowInput.mouseX, g_WindowInput.mouseY, m_winWidth, m_winHeight);   // NAV HUD
			break;
		}

		// Always check BAR HUD
		m_city.m_huds[BAR_HUD]->CheckHotzones(g_WindowInput.mouseX, g_WindowInput.mouseY, m_winWidth, m_winHeight);   // HOME HUD
	}
}

int Application::Key() {
	unsigned long Direction = 0;

	//printf("Entering App::Key(), g_WindowInput.key = %c\n", g_WindowInput.key);

	switch ( g_WindowInput.key )
	{
	case 'w': // Forward
		//printf( "Saw a 'w'\n");
		Direction |= CPlayer::DIR_FORWARD ;
		break;
	case 's': // Backward
		//printf( "Saw an 's'\n");
		Direction |= CPlayer::DIR_BACKWARD;
		break;
	case 'a': // Left
		//printf( "Saw an 'a'\n");
		Direction |= CPlayer::DIR_LEFT;
		break;
	case 'd': // Right
		//printf( "Saw a 'd'\n");
		Direction |= CPlayer::DIR_RIGHT;
		break;
	case 'q': // Down
		//printf( "Saw an 'q'\n");
		Direction |= CPlayer::DIR_DOWN;
		break;
	case 'e': // Up
		//printf( "Saw a 'e'\n");
		Direction |= CPlayer::DIR_UP;
		break;
	case 'z': // Rotate Left
		//printf( "Saw a 'z'\n");
		m_rotate = LEFT;
		break;
	case 'c': // Rotate Right
		//printf( "Saw a 'c'\n");
		m_rotate = RIGHT;
		break;
	case 'b': // Rotate Right
		if(g_reportDC == false)
			g_reportDC = true;
		else
			g_reportDC = false;
		break;
	case 'h': // HUD toggle
		g_HudSwitch = true;
		break;
	case 't': // Stats HUD
		if(!g_StatsHud) {
			g_StatsHud = true;
			if(g_ShowHudMenu)
				g_ShowHudMenu  = false ;

			// CPU & GPU Util
			m_city.m_huds_updater[3]->UpdateText(2,27,30.0, "WIP");
			m_city.m_huds_updater[2]->UpdateText(2,27,30.0, "WIP");

#ifdef __linux__
			// Find the process number
			unsigned int procNum = 0;
			float cpuUtil = 0.0;
			char getCpuUtil[50] = "";
			char cpuUtilStr[10];
			std::ifstream infile;
			std::string procIdString, cpuUtilString;
			system("/usr/bin/ps -a | grep grfxEmul > ./.grfxEmulProcID");
			infile.open("./.grfxEmulProcID");
			getline(infile, procIdString);
			std::istringstream issProcId(procIdString);
			issProcId >> procNum;
			infile.close();
			sprintf(getCpuUtil, "/usr/bin/ps -p %d -o \\%%cpu= > ./.grfxCpuUtil\n", procNum);


			// CPU Util
			system(getCpuUtil);
			infile.open("./.grfxCpuUtil");
			getline(infile, cpuUtilString);
			std::istringstream issCpuUtil(cpuUtilString);
			issCpuUtil >> cpuUtil;
			sprintf(cpuUtilStr, "%4.2f%%", cpuUtil);
			infile.close();
			m_city.m_huds_updater[2]->UpdateText(2,27,30.0, cpuUtilStr);
#endif
		} else {
			g_StatsHud = false;
		}
		break;
	case 'l': // Hotspots toggle
		if(g_showHotzones == false)
			g_showHotzones = true;
		else
			g_showHotzones = false;
		break;
	case 'f': // Functional test
		if(g_appMode == HOME_MODE || g_appMode == NAV_MODE) {
			g_appMode = FUNCTEST_MODE;
			m_autoPilot = false;
			g_wireframe = 0;
			g_showHotzones = false;
			g_depthComplexity = false;
			g_drawAABB = false;
			g_appModeSwitching = false;
			m_compareSnap = true;
		} else {
			g_appMode = HOME_MODE;
			m_autoPilot = true;
			g_appModeSwitching = true;
			m_compareSnap = false;
		}
		break;
	case 'j':   // Dump snapshot (for val purposes)
		if(g_appMode == FUNCTEST_MODE) {
			m_dumpSnap = true;
		}
		break;
	case 'g':
		// only switch mode if autoPilot is enabled
		if(m_autoPilot) {
			if(g_appMode == HOME_MODE) {
				g_appMode = NAV_MODE;
				g_appModeSwitching = true;
				camera->SetFOV(45.0f);
				g_Application.m_city.m_baseTree.m_cars.m_draw = false;
			} else {
				g_appMode = HOME_MODE;
				g_appModeSwitching = true;
				camera->SetFOV(60.0f);
				g_Application.m_city.m_baseTree.m_cars.m_draw = true;
			}
		}
		break;
	case 'i': // AutoPilot toggle
		if(m_autoPilot == false)
			m_autoPilot = true;
		else
			m_autoPilot = false;
		break;
	case 033: // ASCII Escape Key
		//printf( "Saw ESC - exiting...\n");
		exitApp = true;
		break;
	case 'p': // Toggle wireframe
		if(g_wireframe == 2)
			g_wireframe = 0;
		else if(g_wireframe == 0)
			g_wireframe = 1;
		else
			g_wireframe = 2;
		break;
	case 'm': // Toggle depth complexity
		if(g_depthComplexity)
			g_depthComplexity = false;
		else
			g_depthComplexity = true;
		break;
	case 'v': // Toggle validation mode
		if(g_validationMode) {
			printf("Exiting validation mode...\n");
			g_validationMode = false;
		} else {
			printf("Entering validation mode...\n");
			g_validationMode = true;
		}
		break;
	case 'x': // Draw validation bars
		if(g_drawValBars) {
			g_drawValBars = false;
		} else {
			g_drawValBars = true;
		}
		break;
	case 'o': // Toggle wireframe
		if(g_drawAABB)
			g_drawAABB = false;
		else
			g_drawAABB = true;
		break;
	case 'r': // SWitch to post-processing mode (render to FBO)
		if(m_FXAApostProc) {
			printf("FXAA OFF\n");
			m_FXAApostProc = false;
		} else {
			printf("FXAA ON\n");
			m_FXAApostProc = true;
		}
		break;
	}

	// Update direction based on movement keys
	direction = Direction;

	return 0;
}

void Application::DumpSnapShot() {
	printf("Dumping snapshot...\n");
	char snapName[100];
	sprintf(snapName, "./val/snap_w_%d_h_%d_qtsize_%d_tess_%d", m_AppHelper.m_Width,
		m_AppHelper.m_Height, m_AppHelper.m_quadTreeSize, m_AppHelper.m_Tess);

	// Get the data from frame buffer
	unsigned int w = camera->GetViewPort().Width;
	unsigned int h = camera->GetViewPort().Height;
	GLubyte* fb = new GLubyte[w*h*3];
	glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, fb);

	//Now write these to a file in val directory
	ofstream snapFile(snapName, ios_base::binary);
	for(unsigned int i=0; i<w*h*3; i++) {
		snapFile.write((char*) &fb[i], sizeof(GLubyte));
	}

	snapFile.close();
	delete [] fb;
}

void Application::CompareSnapShot() {
	char snapName[100];
	sprintf(snapName, "./val/snap_w_%d_h_%d_qtsize_%d_tess_%d", m_AppHelper.m_Width,
		m_AppHelper.m_Height, m_AppHelper.m_quadTreeSize, m_AppHelper.m_Tess);

	// Check if a matching reference snapShot exists in val directory
	ifstream snapFile(snapName, ios_base::binary);
	if(snapFile.good()) {
		// Get the data from frame buffer
		unsigned int w = camera->GetViewPort().Width;
		unsigned int h = camera->GetViewPort().Height;
		GLubyte* fb = new GLubyte[w*h*3];
		glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, fb);

		//Now compare each RGB value per pixel to the reference
		GLubyte thisByte;
		bool compare = true;
		printf("Comparing snap, please wait...\n");
		for(unsigned int i=0; i<w*h*3; i++) {
			snapFile.read((char*) &thisByte, sizeof(GLubyte));
			if(thisByte != fb[i]) {
				printf("**WARNING** found a discrepency for byte %d\n", i);
				compare = false;
			}
		}

		if(compare) {
			printf("Byte comparision of frame buffer PASSED\n");
		} else {
			printf("Byte comparision of frame buffer FAILED\n");
		}

		snapFile.close();
		delete [] fb;

	} else {
		printf("**ERROR** No reference snapshot matching %s found in ./val directory.\n", snapName);
		printf("\tEither file doesn't exist or it is not readable.\n");
		printf("\tRequest one from Paul Heraty.\n");
	}
}

void Application::ProcessInput() {
    // Don't allow user input if in benchmark mode
    if(m_benchMarkMode)
        return;

#ifdef __linux__
	if ((g_Sock_p) && (recvPort)) {

		int recvMsgSize;
		string sourceAddr;
		unsigned short sourcePort;
		char buffer[SERVER_MAX_MSG_LENGTH + 1];        // max. message length + \0
		for (;;)
		{
			// try to receive a message
			recvMsgSize =  g_Sock_p->recv(buffer, SERVER_MAX_MSG_LENGTH, sourceAddr, sourcePort);
			if ((recvMsgSize < 0) && (errno == EWOULDBLOCK))
			{
				break;
			}
			else
			{
				int val;
				buffer[recvMsgSize] = '\0';
				printf("UDP: %s:%d '%s' [%d]\n", sourceAddr.c_str(), sourcePort, buffer, recvMsgSize);
				if (buffer[0] == ':') {
					DispatchCmd(buffer);
				}
			}
		}
	}
#endif

	// Call the WindowSys input handler
	ProcessWinInput();

	if(g_WindowInput.key) {
		Key();
		g_WindowInput.key = 0;
	}

	if(g_WindowInput.but1 || g_WindowInput.but2) {
		Mouse();
		g_WindowInput.but1 = false;
		g_WindowInput.but2 = false;
	}

	return ;
}

void Application::Shutdown() {
	ShutdownWin();

	// Tidy up FBO objects
	glDeleteRenderbuffers(1, &m_FBO_depthRenderbuffer);
	glDeleteFramebuffers(1, &m_FBO_framebuffer);
	glDeleteTextures(1, &m_FBO_texture);

#ifdef __linux__
	if (g_Sock_p) {
		DispatchCmd("BYE");
		printf("Delete UDP\n");
		delete g_Sock_p;
		g_Sock_p = NULL;
	}
#endif
}
