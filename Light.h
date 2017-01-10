#ifndef LIGHT_H
#define LIGHT_H

#include "Common.h"

#include "glTypes.h"
#include "VertexFormat.h"
#include "AppMode.h"
#include "Material.h"
#include "Random.h"

extern unsigned int g_wireframe;
extern enum APP_MODE g_appMode;

enum LightColor {RED, GREEN, CYAN, YELLOW};

class Light {
public:
  Light();
  ~Light();

  void            Draw(CCamera* camera);
  void            Create(Material mat, GLvector pos);
  void            Update(CCamera* camera, float deltaTime);

//private:
  // Vertex Buffer (Interleaved)
  static std::vector<BaryVertexFormat2>      m_vertex;
  // Index Buffer
  static std::vector<GLushort> m_index;
  // Vertex Buffer Objects (for OpenGL)
  static GLuint          m_vboId[2];
  // Vertex Stride (Coords + UV + Normals + etc...)
  static GLint           m_vtxStride;

  // Material
  Material m_material;
  // Position of the light
  GLvector        m_position;
  // LookAt matrix for billboard
  GLmatrix m_lookAtMat;
  // Color
  float           m_color[4];

  // Blinking vars
  bool            m_blink;
  float           m_time;
  bool            m_draw;
};

#endif // LIGHT_H
