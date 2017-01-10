#ifndef CITY_H
#define CITY_H

#include "Common.h"

#include "glTypes.h"
#include "Camera.h"
#include "Buildings.h"
#include "RoadRoute.h"
#include "HUD.h"
#include "ValidationBar.h"
#include "Car.h"
#include "AppMode.h"
#include "Streetlight.h"
#include "QuadTree.h"

extern enum APP_MODE g_appMode;
extern enum HUD_TYPE g_navHud;
extern bool g_showHotzones;
extern bool g_ShowHudMenu;
extern bool g_StatsHud;
extern bool g_VirtualPos;
extern bool g_depthComplexity;
extern bool g_validationMode;
extern bool g_drawValBars;
extern unsigned int g_numBuildingDraws;
extern unsigned int g_numSignDraws;
extern unsigned int g_numLightDraws;
extern unsigned int g_numStreetlightDraws;
extern unsigned int g_numRoadDraws;
extern unsigned int g_numOtherDraws;

struct plot {
    GLvector2 lowerLeft;
    GLvector2 upperRight;
};

enum mapEntry {
    ROAD,
    FOOTPATH,
    SITE
};

class City {
public:
    City();
    ~City();

    void Create(unsigned int xSize, unsigned int ySize, unsigned short tessFactor, unsigned short quadTreeSize);
    void Draw(CCamera* camera);
    void Update(CCamera* camera, float deltaTime);

    // quadTree (spatial partitioning)
    BaseTree m_baseTree;

    // HUDs
    std::vector<HUD*> m_huds;
    std::vector<HUD_Updater*> m_huds_updater;
//    CES_HUD m_CesHud;
    bool m_drawHud;
//    bool m_drawCesHud;

    // Validation sidebar
    ValidationBar m_valBar;

    // Count of objects
    unsigned long m_cityVertices;
    unsigned long m_cityIndices;

    // memory footprint
    unsigned long m_texMemoryUsed;
};

#endif // CITY_H
