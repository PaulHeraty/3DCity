#ifndef SKYBOX_H
#define SKYBOX_H

#include "Common.h"

#include "glTypes.h"
#include "VertexFormat.h"
#include "DrawableObject.h"

extern unsigned int g_wireframe;
extern unsigned int g_numOtherDraws;

class SkySide : public DrawableObject {
public:
    SkySide(std::string vertexShader, std::string fragmentShader);
    ~SkySide();

    //void Create(GLvector3 center, GLvector3 size);
    void Create(GLvector3 location, GLvector3 size, char *texture, int texSize);
    void Draw(CCamera* camera);
    void Dump();

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
    GLint m_numVertices;
    // Number of Indices
    GLint m_numIndices;
};

class SkyBox {
public:
    SkyBox(std::string vertexShader, std::string fragmentShader);
    ~SkyBox();

    //void Create(GLvector3 center, GLvector3 size);
    void Create(unsigned int xSize, unsigned int ySize, char *texture, int texSize);
    void Draw(CCamera* camera);
    void Dump();

//private:
    std::vector<SkySide*> m_side;
    // Shaders
    std::string m_vertexShader;
    std::string m_fragmentShader;
    unsigned int m_xSize;
    unsigned int m_ySize;
    unsigned int m_zSize;
    // Number of Vertices
    GLint m_numVertices;
    // Number of Indices
    GLint m_numIndices;
};

#endif // SKYBOX_H
