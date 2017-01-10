#ifndef COCKPIT_H
#define COCKPIT_H

#include "Common.h"

#include "glTypes.h"
#include "VertexFormat.h"
#include "DrawableObject.h"

extern unsigned int g_wireframe;
extern unsigned int g_numOtherDraws;

class Cockpit : public DrawableObject {
public:
    Cockpit();
    ~Cockpit();

    void            Draw(CCamera* camera);
    void            Create(int width, int height);
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
};

#endif // COCKPIT_H
