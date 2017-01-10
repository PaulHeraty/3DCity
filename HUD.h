#ifndef HUD_H
#define HUD_H

#include "Common.h"

#include "VertexFormat.h"
#include "DrawableObject.h"
#include "glTypes.h"

extern bool g_CESSeatHeatL;
extern bool g_CESSeatHeatR;
extern bool g_CESHeatMax;
extern bool g_CESHeatRear;
extern bool g_CESHeatFront;
extern bool g_CESAuto;
extern bool g_CESAc;
extern unsigned int g_CESAir;
extern int g_CESFan;
extern int g_CESTempL;
extern int g_CESTempR;

enum HUD_TYPE {
    HOME_HUD,       // 0
    BAR_HUD,        // 1
    NAV_HUD0,       // 2
    NAV_HUD1,       // 3
    NAV_HUD2,       // 4
    NAV_HUD3,       // 5
    NAV_HUD4,       // 6
    NAV_HUD5,       // 7
    STATS_HUD,      // 8
    CES_HUD,        // 9
    CAR_HUD,        // 11
    NO_HUD
};

struct button_callback {
    // Hotzone in normalized screen-space (-1.0 to 1.0)
    GLvector2 topLeft;
    GLvector2 botRight;
    std::string message;
};

class HUD;

class HUD_Updater {
public:
    HUD_Updater(HUD *hud);
    ~HUD_Updater();
    int Create(int posx, int posy, int sizex, int sizey);
    int UpdateText(int dx, int dy, float sz, char *text);
    int ShowHotZone(bool on) { m_HotZone = on; return 1;};
protected:
    cairo_surface_t  *m_SurfaceBackup;
    cairo_surface_t  *m_SurfaceWork;
    cairo_t          *m_ContextBackup;
    cairo_t          *m_ContextWork;
    int   m_PosX;
    int   m_PosY;
    int   m_SizeX;
    int   m_SizeY;
    bool  m_HotZone;
    HUD   *m_Hud;
};

class HUD : public DrawableObject {
    friend class HUD_Updater;
public:
    HUD();
    ~HUD();

    bool Create(int width, int height, HUD_TYPE h);
    void Draw(CCamera* camera);
    void CheckHotzones(int mouseX, int mouseY, int winWidth, int winHeight);
    void DrawHotzones();
    bool UpdateCES_HUD();
protected:
    void AddPngButton(char *filename, int px, int py, char *Message = NULL, bool visible = true);
    void LoadButtonFromConfig(char *path);
public:
//private:
    // Cairo
    cairo_surface_t  *m_cairoSurface;
    cairo_t          *m_cairoContext;
    unsigned char    *m_surfData;
    int              m_width;
    int              m_height;
    int              m_channels; // RGBA = 4
    // Vertex Buffer (Interleaved)
    VertexFormat     m_vertex[4];
    // Index Buffer
    GLushort         m_index[6];
    // Vertex Buffer Objects (for OpenGL)
    GLuint           m_vboIds[2];
    // Vertex Stride (Coords + UV + Normals + etc...)
    GLint            m_vtxStride;
    // Type
    HUD_TYPE        m_hudType;
    // Hotzones & callbacks
    std::vector<button_callback> m_callbacks;

    // Hotspots drawing
    GLuint temp_vboIds[2];

    // Visible
    bool m_visible;
};

#endif // HUD_H
