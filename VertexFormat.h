#ifndef VERTEXFORMAT_H
#define VERTEXFORMAT_H

#include "Common.h"

class VertexFormat {
public:
    VertexFormat();
    VertexFormat(GLfloat xin, GLfloat yin, GLfloat zin, GLfloat uin, GLfloat vin);

    GLfloat x, y, z;
    GLfloat u, v;
};

class VertexFormatPNTB {
public:
    VertexFormatPNTB();
    VertexFormatPNTB(GLfloat xin, GLfloat yin, GLfloat zin,
                     GLfloat nxin, GLfloat nyin, GLfloat nzin,
                     GLfloat uin, GLfloat vin,
                     unsigned char bxin, unsigned char byin, unsigned char bzin);

    // Overload assignment operator
    VertexFormatPNTB& operator =(const VertexFormatPNTB& rhs);

    GLfloat x, y, z;
    GLfloat nx, ny, nz;
    GLfloat u, v;
    unsigned char bx, by, bz, bpack;
};

class BaryVertexFormat2 {
public:
    BaryVertexFormat2();
    BaryVertexFormat2(GLfloat xin, GLfloat yin, GLfloat zin, GLfloat uin, GLfloat vin,
                      unsigned char bxin, unsigned char byin, unsigned char bzin);

    // Overload assignment operator
    BaryVertexFormat2& operator =(const BaryVertexFormat2& rhs);

    GLfloat x, y, z;
    GLfloat u, v;
    unsigned char bx, by, bz, bpack;
};

class PointFormat {
public:
    PointFormat();
    PointFormat(GLfloat xin, GLfloat yin, GLfloat zin);

    GLfloat x, y, z;
};

#endif // VERTEXFORMAT_H
