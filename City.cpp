#include "City.h"

extern char g_FPSString[];

City::City() {
    m_cityIndices = 0;
    m_cityVertices = 0;
    m_texMemoryUsed = 0;
    m_drawHud = true;
//    m_drawCesHud = false;
}

City::~City() {
}

void City::Create(unsigned int xSize, unsigned int ySize, unsigned short tessFactor, unsigned short quadTreeSize) {
    // Create quadTree (city, lights, roads, etc)
    m_baseTree.Create(tessFactor, quadTreeSize, xSize, ySize);

    // Update vertex counts
    m_cityVertices += m_baseTree.m_totalVertices;
    for(unsigned int i=0; i<=m_baseTree.m_numVBOs; i++) {
        for(int j=0; j<MAX_MATERIAL_TYPES; j++) {
            m_cityIndices += m_baseTree.m_materialBin[j].m_numIndices[i];
        }
    }

    // Add moving car verts/indices
    m_cityVertices += m_baseTree.m_numCars * 8;
    m_cityIndices += m_baseTree.m_numCars * 12;


    // Create HUD 0 - HOME_HUD
    HUD *hudMenu = new HUD();
    hudMenu->Create(1920, 972, HOME_HUD);
    m_huds.push_back(hudMenu);

    // HUD 1 - BAR_HUD
    HUD *hudTaskBar = new HUD();
    hudTaskBar->Create(1920, 108, BAR_HUD);
    m_huds.push_back(hudTaskBar);


    HUD_Updater *text1 = new HUD_Updater(m_huds[1]);
    text1->Create(1100,10,320,40); // No check on size for now - carefull please
    m_huds_updater.push_back(text1);
    text1 = new HUD_Updater(m_huds[1]);
    text1->Create(1100,52,320,30);
    m_huds_updater.push_back(text1);


    // HUDs 2-7 - NAV_HUDs
    HUD *hudDirections0 = new HUD();
    hudDirections0->Create(600, 300, NAV_HUD0);      // Left on Vector St.
    m_huds.push_back(hudDirections0);
    HUD *hudDirections1 = new HUD();
    hudDirections1->Create(600, 300, NAV_HUD1);      // Left on Shannon St.
    m_huds.push_back(hudDirections1);
    HUD *hudDirections2 = new HUD();
    hudDirections2->Create(600, 300, NAV_HUD2);      // Left on Yokohama Rd.
    m_huds.push_back(hudDirections2);
    HUD *hudDirections3 = new HUD();
    hudDirections3->Create(600, 300, NAV_HUD3);      // Left on Mission Blvd.
    m_huds.push_back(hudDirections3);
    HUD *hudDirections4 = new HUD();
    hudDirections4->Create(600, 300, NAV_HUD4);      // Straight On
    m_huds.push_back(hudDirections4);
    HUD *hudDirections5 = new HUD();
    hudDirections5->Create(600, 300, NAV_HUD5);      // Prepare to turn left
    m_huds.push_back(hudDirections5);

    // HUD 8 - Stats Hud
    HUD *hudStats = new HUD();
    hudStats->Create(1536, 756, STATS_HUD);
    m_huds.push_back(hudStats);

    // Stats Dynamic Hud Text
    // CPU Util
    //HUD_Updater *text1 = new HUD_Updater(m_huds[8]);
    text1 = new HUD_Updater(m_huds[8]);
    text1->Create(450,520,350,30); // No check on size for now - carefull please
    m_huds_updater.push_back(text1);
    // GPU Util
    text1 = new HUD_Updater(m_huds[8]);
    text1->Create(450,570,350,30);
    m_huds_updater.push_back(text1);
    // Draw Calls
    text1 = new HUD_Updater(m_huds[8]);
    text1->Create(1200,570,300,30);
    m_huds_updater.push_back(text1);

    // HUD 9 - CES HUD
    HUD *hudCes = new HUD();
    hudCes->Create(1920, 972, CES_HUD);
    m_huds.push_back(hudCes);

    // Validation Bar
    m_valBar.Create();

    printf("\n\nCity complete...\n\n");

    printf("  Num Vertices  : %lu\n", m_cityVertices);
    printf("  Num Indices   : %lu\n", m_cityIndices);
    printf("  Num polygons  : %lu\n", m_cityIndices/3);
    printf("  Num VBOs      : %u\n", m_baseTree.m_numVBOs +1);
    printf("  Memory used   :\n" );
  //printf("    Tex     : %lu bytes\n", m_texMemoryUsed);
    printf("    Vertex  : %lu bytes     (%d bytes per vertex)\n", m_cityVertices * sizeof(BaryVertexFormat2), sizeof(BaryVertexFormat2));
    printf("    Index   : %lu bytes     (%d bytes per index)\n\n", m_cityIndices * sizeof(GLuint), sizeof(GLuint));

}

void City::Update(CCamera* camera, float deltaTime) {
    // Update Billboard positions
    m_baseTree.Update(camera, deltaTime);
}

void City::Draw(CCamera* camera) {
    g_numBuildingDraws = 0;
    g_numSignDraws = 0;
    g_numLightDraws = 0;
    g_numStreetlightDraws = 0;
    g_numRoadDraws = 0;
    g_numOtherDraws = 0;

    // Update camera frustum planes
    camera->UpdateFrustumPlanes();

    // Draw city
    m_baseTree.Draw(camera);

//    // CES HUD
//    if(m_drawCesHud) {
//        m_CesHud.Draw(camera);
//    }

    // HUDs
    if(m_drawHud) {
        switch(g_appMode) {
        case(HOME_MODE):
            if (g_ShowHudMenu) {
                //m_huds[HOME_HUD]->Draw(camera); // Menu
                m_huds[CES_HUD]->Draw(camera); // CES
            }
            else if (g_StatsHud) {
                m_huds[STATS_HUD]->Draw(camera); // Stats
            }
            m_huds[BAR_HUD]->Draw(camera); // Taskbar
            if(g_showHotzones) {
                //m_huds[HOME_HUD]->DrawHotzones();
                m_huds[CES_HUD]->DrawHotzones();
                m_huds[BAR_HUD]->DrawHotzones();
            }
            break;
        case(NAV_MODE):
            m_huds[BAR_HUD]->Draw(camera); // Taskbar
            // Directions
            m_huds[g_navHud]->Draw(camera); // Directions
            if (g_StatsHud) {
                m_huds[STATS_HUD]->Draw(camera); // Stats
            }
            if(g_showHotzones) {
                m_huds[BAR_HUD]->DrawHotzones();
            }
            break;
        }
        if (g_FPSString[0]) {
            m_huds_updater[0]->ShowHotZone(g_showHotzones);
            m_huds_updater[0]->UpdateText(2,37,35.0, g_FPSString);
            if (g_VirtualPos) {
                m_huds_updater[1]->ShowHotZone(g_showHotzones);
                GLvector3 pos = camera->GetPosition();
                char buffer[1024];
                sprintf(buffer, "%.2f %.2f %.2f", pos.x, pos.y, pos.z);
                m_huds_updater[1]->UpdateText(2,27,20.0, buffer);
                g_FPSString[0] = 0;
            }
        }
        // Update Stats text?
        if(g_StatsHud) {
            // Draw Calls
            char totalDraws[10];
            sprintf(totalDraws, "%d", g_numBuildingDraws + g_numSignDraws + g_numLightDraws + g_numStreetlightDraws + g_numOtherDraws);
            m_huds_updater[4]->UpdateText(2,27,30.0, totalDraws);
        }
    }

    // If validation mode, draw colored sidebars
    if(g_drawValBars) {
        m_valBar.Draw(camera);
    }

    // Stats
    if(0) {
        printf("Number of draw calls :\n");
        printf("\tBuildings     : %d\n", g_numBuildingDraws);
        printf("\tSigns         : %d\n", g_numSignDraws);
        printf("\tLights        : %d\n", g_numLightDraws);
        printf("\tStreetlights  : %d\n", g_numStreetlightDraws);
        printf("\tRoad          : %d\n", g_numRoadDraws);
        printf("\tOther         : %d\n", g_numOtherDraws);
        printf("\tTotal         : %d\n", g_numBuildingDraws + g_numSignDraws + g_numLightDraws + g_numStreetlightDraws + g_numOtherDraws);
    }
}
