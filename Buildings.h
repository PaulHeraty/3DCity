#ifndef BUILDINGS_H
#define BUILDINGS_H

#include "Common.h"

#include "glTypes.h"
#include "VertexFormat.h"
#include "Material.h"
#include "Light.h"
#include "AABB.h"

struct BuildingSize {
    int x;
    int y;
    int z;
};

extern unsigned int g_numBuildingDraws;
extern unsigned int g_numSignDraws;

enum BuildingType {
    BUILDING_SIMPLE,
    BUILDING_TOWER,
    BUILDING_BLOCKY,
    BUILDING_MODERN,
};

class Building  {
public:
    Building();
    ~Building();

    void CreateSimple (Material mat, GLvector3 loc, GLvector3 size, unsigned int tessFactor);
    void CreateBlocky (Material mat, Material lightMat, GLvector3 loc, GLvector3 size, unsigned int tessFactor);
    void CreateModern (Material mat, GLvector3 loc, GLvector3 size, unsigned int tessFactor);
    void CreateTower  (Material mat, GLvector3 loc, GLvector3 size, unsigned int tessFactor);
    void AddTexturedBlock(GLvector3 location, GLvector3 size, unsigned int tessFactor = 1);
    void AddUnTexturedBlock(GLvector3 location, GLvector3 size);
    void AddSpike(GLvector3 location, GLvector3 size);
    void AddBoxes(GLvector3 location, GLvector3 size);
    void AddAntenna(Material mat, GLvector3 location, GLvector3 size);
    //void CreateSign();
    void AddSign(GLvector3 location, GLvector3 size);
    void AddNeonStrips(GLvector3 location, GLvector3 size);

//private:
    // Building type
    BuildingType m_buildingType;

    // Material type
    Material m_material;

    // Lights
    bool m_hasLight;
    Light m_light;

    // Vertex Stride (Coords + UV + Normals + etc...)
    GLint m_vtxStride;
    // Number of Vertices
    GLint m_numVertices;
    // Vertex Buffer (Interleaved)
    std::vector<BaryVertexFormat2> m_vertex;

    // Number of Indices
    GLint m_numIndices;
    // Index Buffer
    std::vector<GLushort> m_index;

    // Signs
    bool m_hasSign;
    // Sign Texture
    GLuint m_signTextureId;
    // Vertex Buffer (Interleaved)
    BaryVertexFormat2     m_signVertex[8];
    // Index Buffer
    GLushort         m_signIndex[12];
    // Vertex Buffer Objects (for OpenGL)
    GLuint           m_signVboIds[2];

    // Bounding box
    AABB m_AABB;
};

#endif // BUILDINGS_H
