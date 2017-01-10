#ifndef GROUNDPLANE_H
#define GROUNDPLANE_H

#include "Common.h"

#include "glTypes.h"
#include "VertexFormat.h"
#include "DrawableObject.h"

extern unsigned int g_wireframe;
extern unsigned int g_numOtherDraws;

class GroundPlane : public DrawableObject {
public:
    GroundPlane(std::string vertexShader, std::string fragmentShader);
    ~GroundPlane();

    void Create(unsigned int xSize, unsigned int zSize, textureType t);
    void Draw(CCamera* camera);

//private:
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
#endif // GROUNDPLANE_H
