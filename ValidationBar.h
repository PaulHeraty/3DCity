#ifndef VALIDATIONBAR_H
#define VALIDATIONBAR_H

#include "Common.h"

#include "VertexFormat.h"
#include "DrawableObject.h"
#include "glTypes.h"

struct barColor {
    float color[4];
};

class ValidationBar : public DrawableObject {
public:
    ValidationBar();
    ~ValidationBar();

    bool Create();
    void Draw(CCamera* camera);
public:
//private:
    // Vertex Buffer (Interleaved)
    VertexFormat     m_vertex[4];
    // Index Buffer
    GLushort         m_index[6];
    // Vertex Buffer Objects (for OpenGL)
    GLuint           m_vboIds[2];
    // Vertex Stride (Coords + UV + Normals + etc...)
    GLint            m_vtxStride;
    // Color
    barColor    m_barColor[16];
    int         m_currentColor;
};

#endif // VALIDATIONBAR_H
