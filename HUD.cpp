#include "HUD.h"
#include "Config.h"

#include "button_helper.h"

//#define DEBUG_HUD
//#define  DEBUG_HUD_LOW
extern float g_winWidth;
extern float g_winHeight;
extern void DispatchCmd(const char *cmd);

HUD_Updater::HUD_Updater(HUD *hud): m_Hud(hud),m_PosX(0), m_PosY(0), m_SizeX(0), m_SizeY(0), m_SurfaceBackup(NULL), m_SurfaceWork(NULL), m_ContextBackup(NULL), m_ContextWork(NULL), m_HotZone(false)
{
}

HUD_Updater::~HUD_Updater()
{
}

int HUD_Updater::Create(int posx, int posy, int sizex, int sizey) {
    if (!m_SurfaceWork) {
        m_PosX = posx;
        m_PosY = posy;
        m_SizeX = sizex;
        m_SizeY = sizey;
        if (m_Hud->m_cairoSurface) {
            m_SurfaceBackup = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, m_SizeX, m_SizeY);
            m_ContextBackup = cairo_create(m_SurfaceBackup);
            cairo_set_operator(m_ContextBackup, CAIRO_OPERATOR_SOURCE);
            cairo_set_source_surface(m_ContextBackup, m_Hud->m_cairoSurface, -m_PosX, -m_PosY);
            cairo_rectangle(m_ContextBackup, 0, 0, m_SizeX, m_SizeY);
            cairo_fill(m_ContextBackup);
            m_SurfaceWork = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, m_SizeX, m_SizeY);
            m_ContextWork = cairo_create(m_SurfaceWork);
        }
        return 0;
    }
    return -1;
}


int HUD_Updater::UpdateText(int dx, int dy, float sz, char *text)
{
	if (m_SurfaceWork) {
		cairo_set_source_surface(m_ContextWork, m_SurfaceBackup, 0,0);
		cairo_set_operator(m_ContextWork, CAIRO_OPERATOR_SOURCE);
		cairo_paint(m_ContextWork);
		if (m_HotZone) {
			cairo_set_line_width(m_ContextWork, 2.0);
			cairo_set_source_rgb(m_ContextWork, 0.9, 0.1, 0.1);
			cairo_rectangle(m_ContextWork, 0,0,  m_SizeX, m_SizeY);
			cairo_stroke(m_ContextWork);
		}
		cairo_select_font_face(m_ContextWork, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(m_ContextWork, sz);
		cairo_set_source_rgb(m_ContextWork, 0.8, 0.8, 0.8);
		cairo_move_to(m_ContextWork, dx, dy);
		cairo_show_text(m_ContextWork, text);
		cairo_surface_flush(m_SurfaceWork);
		unsigned char *data = cairo_image_surface_get_data(m_SurfaceWork);
		glBindTexture ( GL_TEXTURE_2D, m_Hud->m_texture.m_textureId );
		glTexSubImage2D(GL_TEXTURE_2D, 0, m_PosX, m_PosY, m_SizeX, m_SizeY, GL_RGBA, GL_UNSIGNED_BYTE, data);
		return 0;
	}
	return -1;

}


HUD::HUD() {
    m_hudType = NO_HUD;
    m_visible = true;
}

HUD::~HUD() {
    //free(m_surfData);
    //cairo_destroy(m_cairoContext);
    //cairo_surface_destroy(m_cairoSurface);
}

void HUD::CheckHotzones(int mouseX, int mouseY, int winWidth, int winHeight) {
    if(m_visible) {
        for(unsigned int i=0; i<m_callbacks.size(); i++) {
            // Check to see if we are in the hotzone
            // callback coords are in normalized screen space (-1.0 to 1.0)
            // mouseX/Y are in screen resolution space
            // So we need to translate callbacks coords to screen space
            unsigned int topLeftX = (m_callbacks[i].topLeft.x + 1.0) * (winWidth/2);
            unsigned int botRightX = (m_callbacks[i].botRight.x + 1.0) * (winWidth/2);
            unsigned int topLeftY = winHeight - ((m_callbacks[i].topLeft.y + 1.0) * (winHeight/2));
            unsigned int botRightY = winHeight - ((m_callbacks[i].botRight.y + 1.0) * (winHeight/2));
            if(mouseX > topLeftX &&
                    mouseX < botRightX &&
                    mouseY > topLeftY &&
                    mouseY < botRightY) {
                if (m_callbacks[i].message.length()) {
                    DispatchCmd(m_callbacks[i].message.c_str());
                }
            }
        }
    }
}

void HUD::LoadButtonFromConfig(char *path) {
  Config *t_Config = Config::GetInstance();
  char pathname[4096];
  int  Count = t_Config->HelperCount(path);
//  printf("Count=%d\n", Count);
  ButtonHelper2 m_Bt2;
  for (int k=1; k<=Count; k++) {
    sprintf(pathname, "%s[%d]", path,k);
    t_Config->Helper(pathname, &m_Bt2);
    m_Bt2.Dump();
    if (!m_Bt2.IsNotValid()) {
      AddPngButton((char*)m_Bt2.m_FileName.c_str(), m_Bt2.m_PosX, m_Bt2.m_PosY, (char*)m_Bt2.m_Action.c_str(), m_Bt2.m_Visible);
    }
    m_Bt2.Clear();
  }
}

void HUD::AddPngButton(char *filename, int px, int py, char*Message, bool visible) {
  cairo_surface_t *but;
  m_visible = visible;
  but = cairo_image_surface_create_from_png(filename); 
  int bx = cairo_image_surface_get_width(but);
  int by = cairo_image_surface_get_height(but);
  cairo_set_source_surface(m_cairoContext, but, px, py);
  cairo_paint(m_cairoContext);
#ifdef DEBUG_HUD
  cairo_set_line_width(m_cairoContext, 2.0);
  cairo_set_source_rgb(m_cairoContext, 0.0, 1.0, 0.0);
  cairo_rectangle(m_cairoContext, px, py,  bx, by);
  cairo_stroke(m_cairoContext);
#endif

#ifdef GENERATE_XML
   printf("<button posx=\"%d\" posy=\"%d\" image=\"%s\"", px, py, filename);
#endif

if (Message) {
// Hack MODE - correction for Y only - Ideally need a Matrix trsf
    button_callback cb;
    float a, b, ty1, ty2, my, tx1, tx2, mx;
#ifdef DEBUG_HUD_LOW
    printf("POS: %d,%d SIZE: %d,%d TEXTURE: %d,%d\n", px, py, bx, by, m_width, m_height);
#endif
    ty1 = (g_winHeight)-((m_vertex[0].y+1.0)*g_winHeight/2.0);
    ty2 = (g_winHeight)-((m_vertex[2].y+1.0)*g_winHeight/2.0);
    tx1 = (g_winWidth)-((m_vertex[1].x+1.0)*g_winWidth/2.0);
    tx2 = (g_winWidth)-((m_vertex[0].x+1.0)*g_winWidth/2.0);


    my = (ty2-ty1)/m_height;
    mx = (tx2-tx1)/m_width;
#ifdef DEBUG_HUD_LOW
    printf("TY %.4f %.4f %.4f\n", ty1, ty2, ty2-ty1);
    printf("TX %.4f %.4f %.4f\n", tx1, tx2, tx2-tx1);
#endif
    a = (px*mx)+tx1;
    b = (py*my)+ty1;
    cb.topLeft = glVector(a/(g_winWidth/2.0f)-1.0f, 1.0f-b/(g_winHeight/2.0f));
    a = ((px + bx)*mx)+tx1;
    b = ((py + by)*my)+ty1;
    cb.botRight = glVector(a/(g_winWidth/2.0f)-1.0f, 1.0f-b/(g_winHeight/2.0f));
    if (Message) {
        cb.message = Message;
#ifdef GENERATE_XML
        printf(" action=\"%s\"", Message);
#endif
    }
    m_callbacks.push_back(cb);
  }
#ifdef GENERATE_XML
	printf("/>\n");
#endif



  cairo_surface_destroy(but);
}

bool HUD::UpdateCES_HUD() {
    // First clear the surface
    cairo_save(m_cairoContext);
    cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.3);
    cairo_set_operator(m_cairoContext, CAIRO_OPERATOR_SOURCE);
    cairo_rectangle(m_cairoContext, 460.0, 86.0, 1000, 800);
    cairo_stroke_preserve(m_cairoContext);
    cairo_fill(m_cairoContext);
    cairo_restore(m_cairoContext);

    // Draw HMI lines
    cairo_set_line_width(m_cairoContext, 3.0);
    cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.5);
    cairo_move_to(m_cairoContext, 460, 86);
    cairo_line_to(m_cairoContext, 460, 886);
    cairo_move_to(m_cairoContext, 1460, 86);
    cairo_line_to(m_cairoContext, 1460, 886);
    cairo_move_to(m_cairoContext, 960, 111);
    cairo_line_to(m_cairoContext, 960, 498);
    cairo_move_to(m_cairoContext, 500, 498);
    cairo_line_to(m_cairoContext, 1420, 498);
    // Bottom half lines
    cairo_move_to(m_cairoContext, 720, 498);
    cairo_line_to(m_cairoContext, 720, 860);
    cairo_move_to(m_cairoContext, 1200, 498);
    cairo_line_to(m_cairoContext, 1200, 860);
    cairo_stroke(m_cairoContext);

    //Add buttons
    AddPngButton("images/Intel_Logo1.png", 885, 761);
    if(g_CESSeatHeatL)
        AddPngButton("images/SeatHeatLOn.png", 700, 111);
    else
        AddPngButton("images/SeatHeatL.png", 700, 111);
    if(g_CESSeatHeatR)
        AddPngButton("images/SeatHeatROn.png", 990, 111);
    else
        AddPngButton("images/SeatHeatR.png", 990, 111);
    switch(g_CESTempL) {
    case 0:
        AddPngButton("images/SideTempUpL0.png", 500, 111);
        AddPngButton("images/SideTempDownL0.png", 500, 299);
        AddPngButton("images/TempCounter15.png", 700, 236);
        break;
    case 1:
        AddPngButton("images/SideTempUpL0.png", 500, 111);
        AddPngButton("images/SideTempDownL1.png", 500, 299);
        AddPngButton("images/TempCounter16.png", 700, 236);
        break;
    case 2:
        AddPngButton("images/SideTempUpL0.png", 500, 111);
        AddPngButton("images/SideTempDownL2.png", 500, 299);
        AddPngButton("images/TempCounter17.png", 700, 236);
        break;
    case 3:
        AddPngButton("images/SideTempUpL0.png", 500, 111);
        AddPngButton("images/SideTempDownL3.png", 500, 299);
        AddPngButton("images/TempCounter18.png", 700, 236);
        break;
    case 4:
        AddPngButton("images/SideTempUpL0.png", 500, 111);
        AddPngButton("images/SideTempDownL4.png", 500, 299);
        AddPngButton("images/TempCounter19.png", 700, 236);
        break;
    case 5:
        AddPngButton("images/SideTempUpL0.png", 500, 111);
        AddPngButton("images/SideTempDownL5.png", 500, 299);
        AddPngButton("images/TempCounter20.png", 700, 236);
        break;
    case 6:
        AddPngButton("images/SideTempUpL1.png", 500, 111);
        AddPngButton("images/SideTempDownLFull.png", 500, 299);
        AddPngButton("images/TempCounter21.png", 700, 236);
        break;
    case 7:
        AddPngButton("images/SideTempUpL2.png", 500, 111);
        AddPngButton("images/SideTempDownLFull.png", 500, 299);
        AddPngButton("images/TempCounter22.png", 700, 236);
        break;
    case 8:
        AddPngButton("images/SideTempUpL3.png", 500, 111);
        AddPngButton("images/SideTempDownLFull.png", 500, 299);
        AddPngButton("images/TempCounter23.png", 700, 236);
        break;
    case 9:
        AddPngButton("images/SideTempUpL4.png", 500, 111);
        AddPngButton("images/SideTempDownLFull.png", 500, 299);
        AddPngButton("images/TempCounter24.png", 700, 236);
        break;
    case 10:
        AddPngButton("images/SideTempUpL5.png", 500, 111);
        AddPngButton("images/SideTempDownLFull.png", 500, 299);
        AddPngButton("images/TempCounter25.png", 700, 236);
        break;
    }
    switch(g_CESTempR) {
    case 0:
        AddPngButton("images/SideTempUpR0.png", 1270, 111);
        AddPngButton("images/SideTempDownR0.png", 1270, 299);
        AddPngButton("images/TempCounter15.png", 990, 236);
        break;
    case 1:
        AddPngButton("images/SideTempUpR0.png", 1270, 111);
        AddPngButton("images/SideTempDownR1.png", 1270, 299);
        AddPngButton("images/TempCounter16.png", 990, 236);
        break;
    case 2:
        AddPngButton("images/SideTempUpR0.png", 1270, 111);
        AddPngButton("images/SideTempDownR2.png", 1270, 299);
        AddPngButton("images/TempCounter17.png", 990, 236);
        break;
    case 3:
        AddPngButton("images/SideTempUpR0.png", 1270, 111);
        AddPngButton("images/SideTempDownR3.png", 1270, 299);
        AddPngButton("images/TempCounter18.png", 990, 236);
        break;
    case 4:
        AddPngButton("images/SideTempUpR0.png", 1270, 111);
        AddPngButton("images/SideTempDownR4.png", 1270, 299);
        AddPngButton("images/TempCounter19.png", 990, 236);
        break;
    case 5:
        AddPngButton("images/SideTempUpR0.png", 1270, 111);
        AddPngButton("images/SideTempDownR5.png", 1270, 299);
        AddPngButton("images/TempCounter20.png", 990, 236);
        break;
    case 6:
        AddPngButton("images/SideTempUpR1.png", 1270, 111);
        AddPngButton("images/SideTempDownRFull.png", 1270, 299);
        AddPngButton("images/TempCounter21.png", 990, 236);
        break;
    case 7:
        AddPngButton("images/SideTempUpR2.png", 1270, 111);
        AddPngButton("images/SideTempDownRFull.png", 1270, 299);
        AddPngButton("images/TempCounter22.png", 990, 236);
        break;
    case 8:
        AddPngButton("images/SideTempUpR3.png", 1270, 111);
        AddPngButton("images/SideTempDownRFull.png", 1270, 299);
        AddPngButton("images/TempCounter23.png", 990, 236);
        break;
    case 9:
        AddPngButton("images/SideTempUpR4.png", 1270, 111);
        AddPngButton("images/SideTempDownRFull.png", 1270, 299);
        AddPngButton("images/TempCounter24.png", 990, 236);
        break;
    case 10:
        AddPngButton("images/SideTempUpR5.png", 1270, 111);
        AddPngButton("images/SideTempDownRFull.png", 1270, 299);
        AddPngButton("images/TempCounter25.png", 990, 236);
        break;
    }
    if(g_CESAir == 0) {
        AddPngButton("images/AirDownOn.png", 500, 511);
        AddPngButton("images/AirStraight.png", 500, 636);
        AddPngButton("images/AirUp.png", 500, 761);
    } else if(g_CESAir == 1) {
        AddPngButton("images/AirDown.png", 500, 511);
        AddPngButton("images/AirStraightOn.png", 500, 636);
        AddPngButton("images/AirUp.png", 500, 761);
    } else if(g_CESAir == 2) {
        AddPngButton("images/AirDown.png", 500, 511);
        AddPngButton("images/AirStraight.png", 500, 636);
        AddPngButton("images/AirUpOn.png", 500, 761);
    }
    if(g_CESAc)
        AddPngButton("images/ACOn.png", 740, 636);
    else
        AddPngButton("images/AC.png", 740, 636);
    if(g_CESAuto)
        AddPngButton("images/AutoOn.png", 980, 636);
    else
        AddPngButton("images/Auto.png", 980, 636);
    switch(g_CESFan) {
    case 0:
        AddPngButton("images/FanLeft0.png", 740, 511);
        AddPngButton("images/FanRight0.png", 960, 511);
        break;
    case 1:
        AddPngButton("images/FanLeft1.png", 740, 511);
        AddPngButton("images/FanRight0.png", 960, 511);
        break;
    case 2:
        AddPngButton("images/FanLeft2.png", 740, 511);
        AddPngButton("images/FanRight0.png", 960, 511);
        break;
    case 3:
        AddPngButton("images/FanLeft3.png", 740, 511);
        AddPngButton("images/FanRight0.png", 960, 511);
        break;
    case 4:
        AddPngButton("images/FanLeft4.png", 740, 511);
        AddPngButton("images/FanRight0.png", 960, 511);
        break;
    case 5:
        AddPngButton("images/FanLeftFull.png", 740, 511);
        AddPngButton("images/FanRight1.png", 960, 511);
        break;
    case 6:
        AddPngButton("images/FanLeftFull.png", 740, 511);
        AddPngButton("images/FanRight2.png", 960, 511);
        break;
    case 7:
        AddPngButton("images/FanLeftFull.png", 740, 511);
        AddPngButton("images/FanRight3.png", 960, 511);
        break;
    case 8:
        AddPngButton("images/FanLeftFull.png", 740, 511);
        AddPngButton("images/FanRight4.png", 960, 511);
        break;
    case 9:
        AddPngButton("images/FanLeftFull.png", 740, 511);
        AddPngButton("images/FanRight5.png", 960, 511);
        break;
    case 10:
        AddPngButton("images/FanLeftFull.png", 740, 511);
        AddPngButton("images/FanRight6.png", 960, 511);
        break;
    }
    if(g_CESHeatMax)
        AddPngButton("images/WinHeatMaxOn.png", 1220, 511);
    else
        AddPngButton("images/WinHeatMax.png", 1220, 511);
    if(g_CESHeatRear)
        AddPngButton("images/WinHeatRearOn.png", 1220, 636);
    else
        AddPngButton("images/WinHeatRear.png", 1220, 636);
    if(g_CESHeatFront)
        AddPngButton("images/WinHeatFrontOn.png", 1220, 761);
    else
        AddPngButton("images/WinHeatFront.png", 1220, 761);


    // update m_surfData (image data for OpenGL texture)
    cairo_surface_flush(m_cairoSurface);
    m_surfData = cairo_image_surface_get_data(m_cairoSurface);

    // Use tightly packed data
    glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

    // Bind the texture object
    glBindTexture ( GL_TEXTURE_2D, m_texture.m_textureId );

    // Use cairo texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_surfData);

    // Set the filtering mode
    //glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    //glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}


bool HUD::Create(int width, int height, HUD_TYPE h) {
    m_width = width;
    m_height = height;
    m_hudType = h;

    // Create the program objects & texture
    CreateProgram(HUD_SHADER, "shaders/hud.vp", "shaders/hud.fp", TEXTURE_HUD);

    // Generate Hotspot VBOs
    glGenBuffers(2, temp_vboIds);

    // HUD vertices
    // Screen coords go from -1.0 to 1.0
    switch(h) {
    case(HOME_HUD):
        m_vertex[0] = VertexFormat(-1.0,  1.0, 0.0f, 0.0f, 0.0f);   // x,y,z,u,v
        m_vertex[1] = VertexFormat( 1.0,  1.0, 0.0f, 1.0f, 0.0f);
        m_vertex[2] = VertexFormat( 1.0, -0.8, 0.0f, 1.0f, 1.0f);
        m_vertex[3] = VertexFormat(-1.0, -0.8, 0.0f, 0.0f, 1.0f);
        m_vertex[0] = VertexFormat(-0.48,  0.84, 0.0f, 0.0f, 0.0f);   // x,y,z,u,v
        m_vertex[1] = VertexFormat( 0.48,  0.84, 0.0f, 1.0f, 0.0f);
        m_vertex[2] = VertexFormat( 0.48, -0.64, 0.0f, 1.0f, 1.0f);
        m_vertex[3] = VertexFormat(-0.48, -0.64, 0.0f, 0.0f, 1.0f);
        break;
    case(CES_HUD):
        m_vertex[0] = VertexFormat(-1.0,  1.0, 0.0f, 0.0f, 0.0f);   // x,y,z,u,v
        m_vertex[1] = VertexFormat( 1.0,  1.0, 0.0f, 1.0f, 0.0f);
        m_vertex[2] = VertexFormat( 1.0, -0.8, 0.0f, 1.0f, 1.0f);
        m_vertex[3] = VertexFormat(-1.0, -0.8, 0.0f, 0.0f, 1.0f);
        break;
    case(NAV_HUD0):
    case(NAV_HUD1):
    case(NAV_HUD2):
    case(NAV_HUD3):
    case(NAV_HUD4):
    case(NAV_HUD5):
        m_vertex[0] = VertexFormat(-0.9,  0.9, 0.0f, 0.0f, 0.0f);
        m_vertex[1] = VertexFormat(-0.3,  0.9, 0.0f, 1.0f, 0.0f);
        m_vertex[2] = VertexFormat(-0.3,  0.6, 0.0f, 1.0f, 1.0f);
        m_vertex[3] = VertexFormat(-0.9,  0.6, 0.0f, 0.0f, 1.0f);
        break;
    case(BAR_HUD):
        m_vertex[0] = VertexFormat(-1.0, -0.8, 0.0f, 0.0f, 0.0f);
        m_vertex[1] = VertexFormat( 1.0, -0.8, 0.0f, 1.0f, 0.0f);
        m_vertex[2] = VertexFormat( 1.0, -1.0, 0.0f, 1.0f, 1.0f);
        m_vertex[3] = VertexFormat(-1.0, -1.0, 0.0f, 0.0f, 1.0f);
        break;
    case STATS_HUD:
        m_vertex[0] = VertexFormat(-0.8,  0.8, 0.0f, 0.0f, 0.0f);   // x,y,z,u,v
        m_vertex[1] = VertexFormat( 0.8,  0.8, 0.0f, 1.0f, 0.0f);
        m_vertex[2] = VertexFormat( 0.8, -0.6, 0.0f, 1.0f, 1.0f);
        m_vertex[3] = VertexFormat(-0.8, -0.6, 0.0f, 0.0f, 1.0f);
    }

    // Indicies
    m_index[0] = 0;   m_index[1] = 2;   m_index[2] = 1;
    m_index[3] = 0;   m_index[4] = 3;   m_index[5] = 2;

    m_vtxStride = sizeof(VertexFormat);

    // Generate VBOs
    glGenBuffers(2, m_vboIds);

    // Vertex Array
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, m_vtxStride * 4, m_vertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Array Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 6, m_index, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Create cairo surface & context
    m_cairoSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, m_width, m_height);
    m_cairoContext = cairo_create(m_cairoSurface);




    // Create HUD content with Cairo
    switch(m_hudType) {
    case(HOME_HUD):     // 1920 * 972 pixels
        // Background Rect
        cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.3);
        cairo_rectangle(m_cairoContext, 0.0, 0.0, m_width, m_height);
        cairo_stroke_preserve(m_cairoContext);
        cairo_fill(m_cairoContext);
        //Draw line
        cairo_set_line_width(m_cairoContext, 3.0);
        cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 1.0);
        cairo_move_to(m_cairoContext, 100, 100);
        cairo_line_to(m_cairoContext, 100, 872);
        cairo_move_to(m_cairoContext, 800, 100);
        cairo_line_to(m_cairoContext, 800, 872);
        cairo_move_to(m_cairoContext, 1120, 100);
        cairo_line_to(m_cairoContext, 1120, 872);
        cairo_move_to(m_cairoContext, 1820, 100);
        cairo_line_to(m_cairoContext, 1820, 872);
        cairo_stroke(m_cairoContext);
        cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.4);
        cairo_rectangle(m_cairoContext, 100.0, 100.0, 700, 772);
        cairo_stroke_preserve(m_cairoContext);
        cairo_fill(m_cairoContext);
        cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.4);
        cairo_rectangle(m_cairoContext, 1120.0, 100.0, 700, 772);
        cairo_stroke_preserve(m_cairoContext);
        cairo_fill(m_cairoContext);
        //Read in buttons
        LoadButtonFromConfig("/grfxEmul/home_hud/button");
        break;
    case(CES_HUD):
        cairo_set_line_width(m_cairoContext, 3.0);
        cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.5);
        // Outside lines
        cairo_move_to(m_cairoContext, 460, 86);
        cairo_line_to(m_cairoContext, 460, 886);
        cairo_move_to(m_cairoContext, 1460, 86);
        cairo_line_to(m_cairoContext, 1460, 886);
        // Top half lines
        cairo_move_to(m_cairoContext, 960, 111);
        cairo_line_to(m_cairoContext, 960, 498);
        cairo_move_to(m_cairoContext, 500, 498);
        cairo_line_to(m_cairoContext, 1420, 498);
        // Bottom half lines
        cairo_move_to(m_cairoContext, 720, 498);
        cairo_line_to(m_cairoContext, 720, 860);
        cairo_move_to(m_cairoContext, 1200, 498);
        cairo_line_to(m_cairoContext, 1200, 860);
        cairo_stroke(m_cairoContext);
        // Fill background
        cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.3);
        cairo_rectangle(m_cairoContext, 460.0, 86.0, 1000, 800);
        cairo_stroke_preserve(m_cairoContext);
        cairo_fill(m_cairoContext);
        //Read in buttons
        LoadButtonFromConfig("/grfxEmul/ces_hud/button");
        break;
    case(STATS_HUD):
        // Background Rect
        cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.3);
        cairo_rectangle(m_cairoContext, 0.0, 0.0, m_width, m_height);
        cairo_stroke_preserve(m_cairoContext);
        cairo_fill(m_cairoContext);
        // Draw line
        cairo_set_line_width(m_cairoContext, 3.0);
        //cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 1.0);
        cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.5);
        cairo_move_to(m_cairoContext, 0, 0);
        cairo_line_to(m_cairoContext, 0, 756);
        cairo_move_to(m_cairoContext, 1536, 0);
        cairo_line_to(m_cairoContext, 1536, 756);
        cairo_stroke(m_cairoContext);
        // Print stats text
        cairo_select_font_face(m_cairoContext, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(m_cairoContext, 50.0);
        cairo_move_to(m_cairoContext, 650, 60);
        cairo_set_source_rgba(m_cairoContext, 1.0, 1.0, 0.0, 0.7);
        cairo_show_text(m_cairoContext, "Statistics");
        cairo_select_font_face(m_cairoContext, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(m_cairoContext, 30.0);
        cairo_set_source_rgba(m_cairoContext, 1.0, 1.0, 1.0, 0.7);
        cairo_move_to(m_cairoContext, 100, 150);
        cairo_show_text(m_cairoContext, "Resolution:");
        cairo_move_to(m_cairoContext, 100, 200);
        cairo_show_text(m_cairoContext, "OpenGL Vendor:");
        cairo_move_to(m_cairoContext, 100, 250);
        cairo_show_text(m_cairoContext, "OpenGL Version:");
        cairo_move_to(m_cairoContext, 100, 300);
        cairo_show_text(m_cairoContext, "Driver Version:");
        cairo_move_to(m_cairoContext, 100, 400);
        cairo_show_text(m_cairoContext, "Tesselation Factor:");
        cairo_move_to(m_cairoContext, 100, 450);
        cairo_show_text(m_cairoContext, "QuadTree Size:");
        cairo_move_to(m_cairoContext, 100, 550);
        cairo_show_text(m_cairoContext, "CPU Utilization:");
        cairo_move_to(m_cairoContext, 100, 600);
        cairo_show_text(m_cairoContext, "GPU Utilization:");
        cairo_move_to(m_cairoContext, 800, 150);
        cairo_show_text(m_cairoContext, "# Polygons:");
        cairo_move_to(m_cairoContext, 800, 200);
        cairo_show_text(m_cairoContext, "# Vertices:");
        cairo_move_to(m_cairoContext, 800, 250);
        cairo_show_text(m_cairoContext, "# Indices:");
        cairo_move_to(m_cairoContext, 800, 300);
        cairo_show_text(m_cairoContext, "# VBOs:");
        cairo_move_to(m_cairoContext, 800, 400);
        cairo_show_text(m_cairoContext, "Vertex Memory (MB):");
        cairo_move_to(m_cairoContext, 800, 450);
        cairo_show_text(m_cairoContext, "Index Memory (MB):");
        cairo_move_to(m_cairoContext, 800, 500);
        cairo_show_text(m_cairoContext, "Bytes per Vertex:");
        cairo_move_to(m_cairoContext, 800, 600);
        cairo_show_text(m_cairoContext, "Num Draw Calls:");

        break;
    case(NAV_HUD0):  // 600 * 300
        // Background Rect
        cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.4);
        cairo_rectangle(m_cairoContext, 0.0, 0.0, m_width, m_height);
        cairo_stroke_preserve(m_cairoContext);
        cairo_fill(m_cairoContext);
        // Draw line
        cairo_set_line_width(m_cairoContext, 3.0);
        //cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 1.0);
        cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.5);
        cairo_move_to(m_cairoContext, 2, 0);
        cairo_line_to(m_cairoContext, 2, 300);
        cairo_move_to(m_cairoContext, 598, 0);
        cairo_line_to(m_cairoContext, 598, 300);
        cairo_stroke(m_cairoContext);
        // Add text with shadow
        cairo_select_font_face(m_cairoContext, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(m_cairoContext, 32.0);
        cairo_set_source_rgb(m_cairoContext, 0.2, 0.2, 0.8);
        cairo_move_to(m_cairoContext, 20.0, 110.0);
        cairo_show_text(m_cairoContext, "Left turn");
        cairo_set_source_rgb(m_cairoContext, 1.0, 1.0, 1.0);
        cairo_move_to(m_cairoContext, 23.0, 113.0);
        cairo_show_text(m_cairoContext, "Left turn");
        cairo_set_source_rgb(m_cairoContext, 0.2, 0.2, 0.8);
        cairo_move_to(m_cairoContext, 20.0, 160.0);
        cairo_show_text(m_cairoContext, "on Vector St.");
        cairo_set_source_rgb(m_cairoContext, 1.0, 1.0, 1.0);
        cairo_move_to(m_cairoContext, 23.0, 163.0);
        cairo_show_text(m_cairoContext, "on Vector St.");
        // Add arrow
        cairo_surface_t *arrow0;
        arrow0 = cairo_image_surface_create_from_png("images/leftArrow.png");  // 256x256
        cairo_set_source_surface(m_cairoContext, arrow0, 250, 20);
        cairo_move_to(m_cairoContext, 250,20);
        cairo_paint(m_cairoContext);
        break;
    case(NAV_HUD1):  // 600 * 300
        // Background Rect
        cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.4);
        cairo_rectangle(m_cairoContext, 0.0, 0.0, m_width, m_height);
        cairo_stroke_preserve(m_cairoContext);
        cairo_fill(m_cairoContext);
        // Draw line
        cairo_set_line_width(m_cairoContext, 3.0);
        //cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 1.0);
        cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.5);
        cairo_move_to(m_cairoContext, 2, 0);
        cairo_line_to(m_cairoContext, 2, 300);
        cairo_move_to(m_cairoContext, 598, 0);
        cairo_line_to(m_cairoContext, 598, 300);
        cairo_stroke(m_cairoContext);
        // Add text with shadow
        cairo_select_font_face(m_cairoContext, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(m_cairoContext, 32.0);
        cairo_set_source_rgb(m_cairoContext, 0.2, 0.2, 0.8);
        cairo_move_to(m_cairoContext, 20.0, 110.0);
        cairo_show_text(m_cairoContext, "Left turn");
        cairo_set_source_rgb(m_cairoContext, 1.0, 1.0, 1.0);
        cairo_move_to(m_cairoContext, 23.0, 113.0);
        cairo_show_text(m_cairoContext, "Left turn");
        cairo_set_source_rgb(m_cairoContext, 0.2, 0.2, 0.8);
        cairo_move_to(m_cairoContext, 20.0, 160.0);
        cairo_show_text(m_cairoContext, "on Shannon St.");
        cairo_set_source_rgb(m_cairoContext, 1.0, 1.0, 1.0);
        cairo_move_to(m_cairoContext, 23.0, 163.0);
        cairo_show_text(m_cairoContext, "on Shannon St.");
        // Add arrow
        AddPngButton("images/leftArrow.png", 250, 20);
        break;
    case(NAV_HUD2):  // 600 * 300
        // Background Rect
        cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.4);
        cairo_rectangle(m_cairoContext, 0.0, 0.0, m_width, m_height);
        cairo_stroke_preserve(m_cairoContext);
        cairo_fill(m_cairoContext);
        // Draw line
        cairo_set_line_width(m_cairoContext, 3.0);
        //cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 1.0);
        cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.5);
        cairo_move_to(m_cairoContext, 2, 0);
        cairo_line_to(m_cairoContext, 2, 300);
        cairo_move_to(m_cairoContext, 598, 0);
        cairo_line_to(m_cairoContext, 598, 300);
        cairo_stroke(m_cairoContext);
        // Add text with shadow
        cairo_select_font_face(m_cairoContext, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(m_cairoContext, 32.0);
        cairo_set_source_rgb(m_cairoContext, 0.2, 0.2, 0.8);
        cairo_move_to(m_cairoContext, 20.0, 110.0);
        cairo_show_text(m_cairoContext, "Left turn");
        cairo_set_source_rgb(m_cairoContext, 1.0, 1.0, 1.0);
        cairo_move_to(m_cairoContext, 23.0, 113.0);
        cairo_show_text(m_cairoContext, "Left turn");
        cairo_set_source_rgb(m_cairoContext, 0.2, 0.2, 0.8);
        cairo_move_to(m_cairoContext, 20.0, 160.0);
        cairo_show_text(m_cairoContext, "on Yokohama St.");
        cairo_set_source_rgb(m_cairoContext, 1.0, 1.0, 1.0);
        cairo_move_to(m_cairoContext, 23.0, 163.0);
        cairo_show_text(m_cairoContext, "on Yokohama St.");
        // Add arrow
        AddPngButton("images/leftArrow.png", 250, 20);
        break;
    case(NAV_HUD3):  // 600 * 300
        // Background Rect
        cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.4);
        cairo_rectangle(m_cairoContext, 0.0, 0.0, m_width, m_height);
        cairo_stroke_preserve(m_cairoContext);
        cairo_fill(m_cairoContext);
        // Draw line
        cairo_set_line_width(m_cairoContext, 3.0);
        //cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 1.0);
        cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.5);
        cairo_move_to(m_cairoContext, 2, 0);
        cairo_line_to(m_cairoContext, 2, 300);
        cairo_move_to(m_cairoContext, 598, 0);
        cairo_line_to(m_cairoContext, 598, 300);
        cairo_stroke(m_cairoContext);
        // Add text with shadow
        cairo_select_font_face(m_cairoContext, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(m_cairoContext, 32.0);
        cairo_set_source_rgb(m_cairoContext, 0.2, 0.2, 0.8);
        cairo_move_to(m_cairoContext, 20.0, 110.0);
        cairo_show_text(m_cairoContext, "Left turn");
        cairo_set_source_rgb(m_cairoContext, 1.0, 1.0, 1.0);
        cairo_move_to(m_cairoContext, 23.0, 113.0);
        cairo_show_text(m_cairoContext, "Left turn");
        cairo_set_source_rgb(m_cairoContext, 0.2, 0.2, 0.8);
        cairo_move_to(m_cairoContext, 20.0, 160.0);
        cairo_show_text(m_cairoContext, "on Mission Blvd.");
        cairo_set_source_rgb(m_cairoContext, 1.0, 1.0, 1.0);
        cairo_move_to(m_cairoContext, 23.0, 163.0);
        cairo_show_text(m_cairoContext, "on Mission Blvd.");
        // Add arrow
        AddPngButton("images/leftArrow.png", 250, 20);
        break;
    case(NAV_HUD4):  // 600 * 300
        // Background Rect
        cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.4);
        cairo_rectangle(m_cairoContext, 0.0, 0.0, m_width, m_height);
        cairo_stroke_preserve(m_cairoContext);
        cairo_fill(m_cairoContext);
        // Draw line
        cairo_set_line_width(m_cairoContext, 3.0);
        //cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 1.0);
        cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.5);
        cairo_move_to(m_cairoContext, 2, 0);
        cairo_line_to(m_cairoContext, 2, 300);
        cairo_move_to(m_cairoContext, 598, 0);
        cairo_line_to(m_cairoContext, 598, 300);
        cairo_stroke(m_cairoContext);
        // Add text with shadow
        cairo_select_font_face(m_cairoContext, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(m_cairoContext, 32.0);
        cairo_set_source_rgb(m_cairoContext, 0.2, 0.2, 0.8);
        cairo_move_to(m_cairoContext, 20.0, 110.0);
        cairo_show_text(m_cairoContext, "Straight ahead");
        cairo_set_source_rgb(m_cairoContext, 1.0, 1.0, 1.0);
        cairo_move_to(m_cairoContext, 23.0, 113.0);
        cairo_show_text(m_cairoContext, "Straight ahead");
        cairo_set_source_rgb(m_cairoContext, 0.2, 0.2, 0.8);
        cairo_move_to(m_cairoContext, 20.0, 160.0);
        cairo_show_text(m_cairoContext, "for a while");
        cairo_set_source_rgb(m_cairoContext, 1.0, 1.0, 1.0);
        cairo_move_to(m_cairoContext, 23.0, 163.0);
        cairo_show_text(m_cairoContext, "for a while");
        // Add arrow
        AddPngButton("images/UpArrow.png", 250, 20);
        break;
    case(NAV_HUD5):  // 600 * 300
        // Background Rect
        cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.4);
        cairo_rectangle(m_cairoContext, 0.0, 0.0, m_width, m_height);
        cairo_stroke_preserve(m_cairoContext);
        cairo_fill(m_cairoContext);
        // Draw line
        cairo_set_line_width(m_cairoContext, 3.0);
        //cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 1.0);
        cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.5);
        cairo_move_to(m_cairoContext, 2, 0);
        cairo_line_to(m_cairoContext, 2, 300);
        cairo_move_to(m_cairoContext, 598, 0);
        cairo_line_to(m_cairoContext, 598, 300);
        cairo_stroke(m_cairoContext);
        // Add text with shadow
        cairo_select_font_face(m_cairoContext, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(m_cairoContext, 32.0);
        cairo_set_source_rgb(m_cairoContext, 0.2, 0.2, 0.8);
        cairo_move_to(m_cairoContext, 20.0, 110.0);
        cairo_show_text(m_cairoContext, "Approaching");
        cairo_set_source_rgb(m_cairoContext, 1.0, 1.0, 1.0);
        cairo_move_to(m_cairoContext, 23.0, 113.0);
        cairo_show_text(m_cairoContext, "Approaching");
        cairo_set_source_rgb(m_cairoContext, 0.2, 0.2, 0.8);
        cairo_move_to(m_cairoContext, 20.0, 160.0);
        cairo_show_text(m_cairoContext, "Left turn");
        cairo_set_source_rgb(m_cairoContext, 1.0, 1.0, 1.0);
        cairo_move_to(m_cairoContext, 23.0, 163.0);
        cairo_show_text(m_cairoContext, "Left turn");
        // Add arrow
        AddPngButton("images/approachLeft.png", 250, 20);
        break;
    case(BAR_HUD):  // 1920 * 108
        // Background Rect
        //cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.4);
        cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.3);
        cairo_rectangle(m_cairoContext, 0.0, 0.0, m_width, m_height);
        cairo_stroke_preserve(m_cairoContext);
        cairo_fill(m_cairoContext);
        // Draw line
        cairo_set_line_width(m_cairoContext, 3.0);
        //cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 1.0);
        cairo_set_source_rgba(m_cairoContext, 0.0, 0.5, 1.0, 0.5);
        cairo_move_to(m_cairoContext, 0, 2);
        cairo_line_to(m_cairoContext, 1920, 2);
        cairo_stroke(m_cairoContext);
        LoadButtonFromConfig("/grfxEmul/bar_hud/button");
        break;
    }

#ifdef DEBUG_HUD
// Force the color for HUD debug uv(0,0)
     cairo_set_line_width(m_cairoContext, 2.0);
     cairo_set_source_rgb(m_cairoContext, 1.0, 0.0, 0.0);
     cairo_rectangle(m_cairoContext, 0, 0,  2, 2);
     cairo_stroke(m_cairoContext);
#endif


    // update m_surfData (image data for OpenGL texture)
    cairo_surface_flush(m_cairoSurface);
    m_surfData = cairo_image_surface_get_data(m_cairoSurface);

    // Use tightly packed data
    glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

    // Generate a texture object
    glGenTextures ( 1, &m_texture.m_textureId );

    // Bind the texture object
    glBindTexture ( GL_TEXTURE_2D, m_texture.m_textureId );

    // Use cairo texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_surfData);

    // Set the filtering mode
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // Debug dump of HUDs to png
    if(0) {
        switch(m_hudType) {
        case(HOME_HUD):
            cairo_surface_write_to_png(m_cairoSurface, "./homeHud.png");
            break;
        case(NAV_HUD0):
        case(NAV_HUD1):
        case(NAV_HUD2):
        case(NAV_HUD3):
        case(NAV_HUD4):
        case(NAV_HUD5):
            cairo_surface_write_to_png(m_cairoSurface, "./navHud.png");
            break;
        case(BAR_HUD):
            cairo_surface_write_to_png(m_cairoSurface, "./barHud.png");
            break;
        }
    }

    return true;
}

void HUD::Draw(CCamera* camera) {
    if(m_visible) {
        //*********************************
        //          OpenGL Bit
        //*********************************
        // Reset offset for each object
        GLuint offset = 0;

        // Use the program object
        glUseProgram ( m_programObject );

        // Use VBOs Vertex Data
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);

        // Load the vertex position attribute
        glVertexAttribPointer ( m_positionLoc, 3, GL_FLOAT,
                                GL_FALSE, 5 * sizeof(GLfloat), (const void*)offset );
        offset += sizeof(GLfloat) * 3;

        // Load the texture coordinate attribute
        glVertexAttribPointer ( m_texCoordLoc, 2, GL_FLOAT,
                                GL_FALSE, 5 * sizeof(GLfloat), (const void *)offset );

        glEnableVertexAttribArray ( m_positionLoc );
        glEnableVertexAttribArray ( m_texCoordLoc );

        // User VBOs Index Data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);

        // Bind the texture object
        glActiveTexture ( GL_TEXTURE0 );
        // glEnable(GL_TEXTURE_2D); DEPRECATED
        glBindTexture ( GL_TEXTURE_2D, m_texture.m_textureId );

        // Disable depth check
        glDisable(GL_DEPTH_TEST);

        // Blend particles
        glEnable ( GL_BLEND );
        glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        // Set the sampler texture unit to 0
        glUniform1i ( m_samplerLoc, 0 );

        // Draw the HUD
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

        glDisable(GL_BLEND );
        // Enable depth check
        glEnable(GL_DEPTH_TEST);
    }
}

void HUD::DrawHotzones() {
    static VertexFormat points[4];
    static GLushort ind[6];
    ind[0] = 0;
    ind[1] = 2;
    ind[2] = 1;
    ind[3] = 0;
    ind[4] = 3;
    ind[5] = 2;
    GLuint offset;

    // Generate VBOs
   // glGenBuffers(2, temp_vboIds);

    // Use the program object
    glUseProgram ( m_programObject );

//glDisable(GL_CULL_FACE);
    for(unsigned int i=0; i<m_callbacks.size(); i++) {
        offset = 0;
        points[0] = VertexFormat(m_callbacks[i].topLeft.x, m_callbacks[i].topLeft.y, 0.0,       0,0);
        points[1] = VertexFormat(m_callbacks[i].botRight.x, m_callbacks[i].topLeft.y, 0.0,     0,0);
        points[2] = VertexFormat(m_callbacks[i].botRight.x, m_callbacks[i].botRight.y, 0.0,     0,0);
        points[3] = VertexFormat(m_callbacks[i].topLeft.x, m_callbacks[i].botRight.y, 0.0,      0,0);

        // Vertex Array
        glBindBuffer(GL_ARRAY_BUFFER, temp_vboIds[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat) * 4, points, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Array Buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temp_vboIds[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 6, ind, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // Use VBOs Vertex Data
        glBindBuffer(GL_ARRAY_BUFFER, temp_vboIds[0]);

        // Load the vertex position attribute
        glVertexAttribPointer ( m_positionLoc, 3, GL_FLOAT,
                                GL_FALSE, 5 * sizeof(GLfloat), (const void*)offset );
        offset += sizeof(GLfloat) * 3;

        // Load the texture coordinate attribute
        glVertexAttribPointer ( m_texCoordLoc, 2, GL_FLOAT,
                                GL_FALSE, 5 * sizeof(GLfloat), (const void *)offset );

        glEnableVertexAttribArray ( m_positionLoc );
        glEnableVertexAttribArray ( m_texCoordLoc );

        // User VBOs Index Data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temp_vboIds[1]);

        // Bind the texture object
        glActiveTexture ( GL_TEXTURE0 );
        // glEnable(GL_TEXTURE_2D); DEPRECATED
        glBindTexture ( GL_TEXTURE_2D, m_texture.m_textureId );

        // Blend particles
        glEnable ( GL_BLEND );
        glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        glDisable(GL_DEPTH_TEST);

        // Set the sampler texture unit to 0
        glUniform1i ( m_samplerLoc, 0 );

        // Draw the HUD
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

        glDisable(GL_BLEND );
        glEnable(GL_DEPTH_TEST);
    }
}
