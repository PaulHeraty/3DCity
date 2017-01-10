#ifndef AABB_H
#define AABB_H

#include "Common.h"

#include "glTypes.h"
#include "VertexFormat.h"
#include "DrawableObject.h"
#include "Camera.h"

class AABB : public DrawableObject {
public:
    AABB();
    ~AABB();

    void CreateAABB(GLvector3 loc, GLvector3 size, bool quadTree = false);
    void Draw(CCamera *camera);
//private:
    // Axis Aligned Bounding Box in world space coords
    GLbbox m_AABBinWS;
    // Vertex Buffer (Interleaved)
    PointFormat m_vertex[8];
    // Index Buffer
    GLushort m_index[36];
    // Vertex Stride (Coords + UV + Normals + etc...)
    GLint m_vtxStride;
    // QuadTree AABB?
    bool m_isQuadTree;
};

#endif // AABB_H
