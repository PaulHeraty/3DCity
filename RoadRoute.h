#ifndef ROADROUTE_H
#define ROADROUTE_H

#include "Common.h"

#include "glTypes.h"
#include "VertexFormat.h"
#include "Material.h"

extern unsigned int g_wireframe;
extern unsigned int g_numRoadDraws;

class RoadRoute {
public:
    RoadRoute();
    ~RoadRoute();

    void Create(Material mat, GLvector3 min, GLvector3 max);
    void Draw(CCamera* camera);

//private:
    // Material
    Material m_material;
    // Vertex Buffer (Interleaved)
    BaryVertexFormat2 m_vertex[4];
    // Index Buffer
    GLushort m_index[6];
    // Vertex Buffer Objects (for OpenGL)
    GLuint m_vboIds[2];

    // Vertex Stride (Coords + UV + Normals + etc...)
    GLint m_vtxStride;
    // Number of Vertices
    GLint m_numVertices;
    // Number of Indices
    GLint m_numIndices;
};


#endif // ROADROUTE_H
