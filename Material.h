#ifndef MATERIAL_H
#define MATERIAL_H

#include "Common.h"

#include "Texture.h"
#include "Shader.h"
#include "Camera.h"
#include "VertexFormat.h"

extern std::vector<Shader> shaderObjects;


// NB Add transparent object to end of list, as they will be drawn last
enum materialType {
    BUILDING0_MAT,
    BUILDING1_MAT,
    BUILDING2_MAT,
    BUILDING3_MAT,
    BUILDING4_MAT,
    BUILDING5_MAT,
    BUILDING6_MAT,
    BUILDING7_MAT,
    BUILDING8_MAT,
    BUILDING9_MAT,
    SIMPLE_MAT,
    BLACK_MAT,
    ROAD_MAT,
    SIGN0_MAT,
    SIGN1_MAT,
    SIGN2_MAT,
    SIGN3_MAT,
    SIGN4_MAT,
    SIGN5_MAT,
    SIGN6_MAT,
    SIGN7_MAT,
    SIGN8_MAT,
    SIGN9_MAT,
    //SKYBOX_LEFT_MAT,
    //SKYBOX_RIGHT_MAT,
    //SKYBOX_TOP_MAT,
    //SKYBOX_FRONT_MAT,
    //SKYBOX_BACK_MAT,
    STREETLIGHT_MAT,
    LIGHT_MAT,
    CAR_MAT,
    CARS_MAT,
    DEPTH_COMPLEXITY_MAT,
    MAX_MATERIAL_TYPES
};

class State {
public:
    State();
    ~State();
//private:
    bool m_alpha;
    bool m_visible;
};

class Material {
public:
    Material();
    ~Material();

    void CreateProgram(ShaderType shaderType, std::string vertexShader, std::string fragmentShader, textureType t, char *filename = "", int texSize = 0);
    bool ShaderExists(ShaderType shaderType, std::string vertexShader, std::string fragmentShader, GLuint &progObj);

//private:
    materialType m_matType;
    // Shader
    Shader m_shader;
    // Texture
    Texture m_texture;
    // State
    State m_state;
};

#endif // MATERIAL_H
