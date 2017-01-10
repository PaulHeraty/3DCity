#ifndef STREETLIGHT_H
#define STREETLIGHT_H

#include "Common.h"

#include "glTypes.h"
#include "VertexFormat.h"
#include "DrawableObject.h"
#include "Material.h"

extern unsigned int g_wireframe;
extern unsigned int g_numStreetlightDraws;

//class Streetlight : public DrawableObject {
class Streetlight {
public:
    Streetlight();
    ~Streetlight();

    void Create(Material mat, GLvector3 ll, GLvector3 ur);
    //void Draw(CCamera* camera);

//private:
    // Material type
    Material m_material;
    // Vertex Buffer (Interleaved)
    BaryVertexFormat2 vertex[4];
    // Index Buffer
    GLushort index[6];
    // Vertex Buffer Objects (for OpenGL)
    GLuint vboIds[2];

    // Vertex Stride (Coords + UV + Normals + etc...)
    GLint vtxStride;
    // Number of Vertices
    GLint numVertices;
    // Number of Indices
    GLint numIndices;
};

#endif // STREETLIGHT_H
