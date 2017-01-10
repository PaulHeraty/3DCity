#ifndef CAR_H
#define CAR_H

#include "Common.h"

#include "glTypes.h"
#include "VertexFormat.h"
#include "Material.h"

extern unsigned int g_wireframe;
extern unsigned int g_numOtherDraws;

//class Car : public DrawableObject {
class Car {
public:
  Car();
  ~Car();

  void            Draw(CCamera* camera);
  void            Create(Material mat);

//private:
  // Material
  Material m_material;
  BaryVertexFormat2 m_vertex[4];
  // Index Buffer
  GLushort m_index[6];
  // Vertex Buffer Objects (for OpenGL)
  GLuint          m_vboId[2];
  // Vertex Stride (Coords + UV + Normals + etc...)
  GLint           m_vtxStride;
  // Position
  GLvector        m_position;
};

#endif // CAR_H
