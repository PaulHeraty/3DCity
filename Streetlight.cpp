#include "Streetlight.h"

// Constructor
Streetlight::Streetlight() {
    numVertices = 0;
    numIndices = 0;
    vtxStride = 0;
}

// Destructor
Streetlight::~Streetlight() {
}

// Create
void Streetlight::Create(Material mat, GLvector3 ll, GLvector3 ur) {
    // Set the material type
    m_material = mat;

    // Path in X or Z?
    if((ur.x - ll.x) > (ur.z - ll.z)) {     // X path
        vertex[0] = BaryVertexFormat2(ll.x, ll.y, ll.z,        1.0f,  0.0f,  1, 0, 0);
        vertex[1] = BaryVertexFormat2(ur.x, ll.y, ll.z,        1.0f,  6.0f,  0, 1, 0);
        vertex[2] = BaryVertexFormat2(ur.x, ll.y, ur.z,        0.0f,  6.0f,  0, 0, 1);
        vertex[3] = BaryVertexFormat2(ll.x, ll.y, ur.z,        0.0f,  0.0f,  0, 1, 0);
    } else {                                // Z path
        vertex[0] = BaryVertexFormat2(ll.x, ll.y, ll.z,        0.0f,  0.0f,  1, 0, 0);
        vertex[1] = BaryVertexFormat2(ur.x, ll.y, ll.z,        1.0f,  0.0f,  0, 1, 0);
        vertex[2] = BaryVertexFormat2(ur.x, ll.y, ur.z,        1.0f,  6.0f,  0, 0, 1);
        vertex[3] = BaryVertexFormat2(ll.x, ll.y, ur.z,        0.0f,  6.0f,  0, 1, 0);
    }

    //Front
    index[0] = 0;  index[1] = 1;  index[2] = 2;
    index[3] = 0;  index[4] = 2;  index[5] = 3;

    vtxStride = sizeof(BaryVertexFormat2);
    numVertices = sizeof(vertex) / sizeof(BaryVertexFormat2);
    numIndices = sizeof(index) / sizeof(GLushort);
}

