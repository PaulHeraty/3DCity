#ifndef CARS_H
#define CARS_H

#include "Common.h"

#include "glTypes.h"
#include "VertexFormat.h"
#include "AppMode.h"
#include "Material.h"
#include "Random.h"

extern unsigned int g_wireframe;
extern enum APP_MODE g_appMode;

enum CarLight {BACK, FRONT};

enum CarDirection {POSX, NEGX, POSZ, NEGZ};

struct CarColorOffsetDir {
    // Offset of the car
    float           m_offset[4];
    // Car Light Color
    float           m_color[4];
    // CarDirection
    CarDirection m_dir;
};

class Cars {
public:
  Cars();
  ~Cars();

  void            Draw(CCamera* camera);
  //void            Create(Material mat, float offset[3], CarDirection dir);
  void            AddCar(float offset[3], CarDirection dir);
  void            Init(Material mat);
  void            FinishInit();
  void            Update(CCamera* camera, float deltaTime);

//private:
  // Vertex Buffer (Interleaved)
  std::vector<BaryVertexFormat2>      m_vertex;
  unsigned int m_numVertices;
  // Index Buffer
  std::vector<GLushort> m_index;
  unsigned int m_numIndices;
  // Vertex Stride (Coords + UV + Normals + etc...)
  GLint           m_vtxStride;
  // Vertex Buffer Objects (for OpenGL)
  GLuint          m_vboId[2];


  // Material
  Material m_material;

  std::vector<CarColorOffsetDir> m_colorOffsetDir;
  // Vertex Buffer Objects (for OpenGL)
  GLint           m_codStride;
  GLuint          m_codId[1];

  /*
  // Position of the car
  float           m_offset[4];
  // Car Light Color
  float           m_color[4];
  // CarDirection
  CarDirection m_dir;
  */
  // Draw or not?
  bool m_draw;
};

#endif // CARS_H
