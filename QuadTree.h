#ifndef QUADTREE_H
#define QUADTREE_H

#include "Common.h"

#include "glTypes.h"
#include "VertexFormat.h"
#include "DrawableObject.h"
#include "Material.h"
#include "Camera.h"
#include "Buildings.h"
#include "Streetlight.h"
#include "AppMode.h"
#include "GroundPlane.h"
#include "RoadRoute.h"
#include "SkyBox.h"
#include "Car.h"
#include "Cars.h"
#include "Cockpit.h"

#define MAX_VBO_VERTICES 1000000
#define MAX_OVERDRAW_DRAWN 16

extern bool g_drawAABB;
extern unsigned int g_numBuildingDraws;
extern unsigned int g_numSignDraws;
extern unsigned int g_numLightDraws;
extern unsigned int g_numStreetlightDraws;
extern unsigned int g_numRoadDraws;
extern unsigned int g_numOtherDraws;
extern enum APP_MODE g_appMode;
extern bool g_depthComplexity;
extern bool g_reportDC;

class RenderBatch {
public:
    RenderBatch();
    ~RenderBatch();
//private:
    GLuint m_indexCount;
    GLuint m_primCount;
};

class MaterialBin {
public:
    MaterialBin();
    ~MaterialBin();
    void DrawMaterial(CCamera *camera, int vboNum);
    void UpdateRenderBatch(GLuint indexCount, GLuint primCount, int vboNum);
    void MergeRenderBatches(int vboNum);
    void Create(materialType t);
//private:
    // Index buffers
    std::vector< std::vector<GLuint> > m_index;
    std::vector<GLuint> m_numIndices;
    std::vector<GLuint> m_indexVboId;

    // Render batches
    std::vector<std::vector<RenderBatch>  > m_renderBatches;
    std::vector<GLuint> m_numRenderBatches;

    // Material (texture, shader, etc)
    Material m_material;
};

class QuadTree : public DrawableObject {
public:
    QuadTree();
    ~QuadTree();

    void Create(unsigned int lowerx, unsigned int lowerz, unsigned short tessFactor,
                GLint m_vtxStride, MaterialBin *materials, int quadTreeSize);
    void AddBuilding(Material mat, Material lightMat, GLvector3 loc, GLvector3 size, unsigned short tessFactor,
                     BuildingType builType);
    void Draw(CCamera *camera);
//private:
    // Bounding box
    AABB m_AABB;

    // Buildings
    std::vector<Building> m_buildings;
    unsigned int m_numBuildings;

    // Index counters for hierarchial batching
    std::vector< std::vector<GLuint> > m_indexCount;
    std::vector< std::vector<GLuint> > m_primCount;
    static std::vector< std::vector<GLuint> > m_currentIndexCount;

    // Street Lights
    std::vector<Streetlight> m_streetlights;
    unsigned int m_numstreetlights;
    std::vector<BaryVertexFormat2> m_slVertex;
    GLint m_numSlVertices;
    std::vector<GLushort> m_slIndex;
    GLint m_numSlIndices;

    // Roads
    std::vector<RoadRoute> m_roads;
    unsigned int m_numroads;
    std::vector<BaryVertexFormat2> m_rVertex;
    GLint m_numRVertices;
    std::vector<GLushort> m_rIndex;
    GLint m_numRIndices;

    // Signs
    Material m_signMaterial;
    std::vector<BaryVertexFormat2> m_signVertex;
    GLint m_numSignVertices;
    std::vector<GLushort> m_signIndex;
    GLint m_numSignIndices;
};

class BaseTree : public DrawableObject {
public:
    BaseTree();
    ~BaseTree();
    void Draw(CCamera *camera);
    void DrawBillBoard(CCamera* camera, Light light);
    void Create(unsigned short tessFactor, unsigned short quadTreeSize, unsigned int xSize, unsigned int ySize);
    void CreateVBOs();
    void Update(CCamera* camera, float deltaTime);
    void Add2RenderBatch(QuadTree *qt);
    void AddNewQuadTreeVBOIndex();
    void DrawDepthComplexity(CCamera* camera, int depth);
    void AddVertIndBuffersToMain(materialType mType, std::vector<BaryVertexFormat2> verts, unsigned int numVertices,
            std::vector<GLushort> inds, unsigned int numIndices, unsigned int &offset);

    // quadTree nodes
    unsigned int m_quadTreeSize;
    QuadTree m_quadTree[32][32];
    // leafTree nodes
    MaterialBin m_materialBin[MAX_MATERIAL_TYPES];

    // Ground plane
    std::vector<GroundPlane*> m_ground;
    // Skybox
    std::vector<SkyBox*> m_skybox;
    // Route road traces
    std::vector<RoadRoute*> m_routes;
    // Car
    Car m_car;
    // Cockpit
    Cockpit m_cockpit;

    //Cars
    //std::vector<Cars> m_cars;
    Cars m_cars;
    unsigned int m_numCars;

    // Vertex Buffer Objects - can be multiple due to max VBO size
    std::vector<GLuint> m_vertexVboId;
    // Vertex Buffer (Interleaved)
    std::vector< std::vector<BaryVertexFormat2> > m_vertex;
    GLuint m_numVBOs;
    std::vector<GLuint> m_numVertices;
    GLuint m_totalVertices;
    // Vertex Stride (Coords + UV + Normals + etc...)
    GLint m_vtxStride;

    // Signs
    GLuint m_signTextureId[32][32];
};

#endif // QUADTREE_H
