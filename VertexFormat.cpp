#include "VertexFormat.h"

VertexFormat::VertexFormat() {
    x = 0.0;
    y = 0.0;
    z = 0.0;
    u = 0.0;
    v = 0.0;
}

VertexFormat::VertexFormat(GLfloat xin, GLfloat yin, GLfloat zin, GLfloat uin, GLfloat vin) {
    x = xin;
    y = yin;
    z = zin;
    u = uin;
    v = vin;
}
VertexFormatPNTB::VertexFormatPNTB() {
    x = 0.0;
    y = 0.0;
    z = 0.0;
    nx = 0.0;
    ny = 0.0;
    nz = 0.0;
    u = 0.0;
    v = 0.0;
    bx = 0;
    by = 0;
    bz = 0;
    bpack = 0;
}

VertexFormatPNTB::VertexFormatPNTB(GLfloat xin, GLfloat yin, GLfloat zin,
                                   GLfloat nxin, GLfloat nyin, GLfloat nzin,
                                   GLfloat uin, GLfloat vin,
                                   unsigned char bxin, unsigned char byin, unsigned char bzin) {
    x = xin;
    y = yin;
    z = zin;
    nx = nxin;
    ny = nyin;
    nz = nzin;
    u = uin;
    v = vin;
    bx = bxin;
    by = byin;
    bz = bzin;
    bpack = 0;
}

VertexFormatPNTB& VertexFormatPNTB::operator =(const VertexFormatPNTB& rhs) {
    // Prevent self assignment
    if(this == &rhs)
        return *this;

    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    nx = rhs.nx;
    ny = rhs.ny;
    nz = rhs.nz;
    u = rhs.u;
    v = rhs.v;
    bx = rhs.bx;
    by = rhs.by;
    bz = rhs.bz;
    bpack = rhs.bpack;

    // Return a reference to *this object
    return *this;
}

BaryVertexFormat2::BaryVertexFormat2() {
    x = 0.0;
    y = 0.0;
    z = 0.0;
    u = 0.0;
    v = 0.0;
    bx = 0;
    by = 0;
    bz = 0;
    bpack = 0;
}

BaryVertexFormat2::BaryVertexFormat2(GLfloat xin, GLfloat yin, GLfloat zin, GLfloat uin, GLfloat vin,
                                     unsigned char bxin, unsigned char byin, unsigned char bzin) {
    x = xin;
    y = yin;
    z = zin;
    u = uin;
    v = vin;
    bx = bxin;
    by = byin;
    bz = bzin;
    bpack = 0;
}

BaryVertexFormat2& BaryVertexFormat2::operator =(const BaryVertexFormat2& rhs) {
    // Prevent self assignment
    if(this == &rhs)
        return *this;

    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    u = rhs.u;
    v = rhs.v;
    bx = rhs.bx;
    by = rhs.by;
    bz = rhs.bz;
    bpack = rhs.bpack;

    // Return a reference to *this object
    return *this;
}

PointFormat::PointFormat() {
    x = 0.0;
    y = 0.0;
    z = 0.0;
}

PointFormat::PointFormat(GLfloat xin, GLfloat yin, GLfloat zin) {
    x = xin;
    y = yin;
    z = zin;
}
