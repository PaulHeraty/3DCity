#ifndef DRAWABLEOBJECT_H
#define DRAWABLEOBJECT_H

#include "Common.h"

#include "Texture.h"
#include "Shader.h"
#include "Camera.h"
#include "VertexFormat.h"

extern std::vector<Shader> shaderObjects;

class DrawableObject {
public:
    DrawableObject();
    ~DrawableObject();

    virtual void Draw(CCamera* camera) = 0;
    void CreateProgram(ShaderType shaderType, std::string vertexShader, std::string fragmentShader, textureType t, char *filename = "", int texSize = 0);
    bool ShaderExists(ShaderType shaderType, std::string vertexShader, std::string fragmentShader, GLuint &progObj);

//private:
    // Shaders
    std::string m_vertexShader;
    std::string m_fragmentShader;
    ShaderType m_shaderType;
    // OpenGL ES2 Program Object
    GLuint m_programObject;
    // Shader attribute locations
    GLint m_positionLoc;
    GLint m_texCoordLoc;
    GLint m_colorLoc;
    // Shader Uniform locations
    GLint m_mvpMatrixLoc;
    GLint m_lookAtMatrixLoc;
    // Shader wireframe loca
    GLint m_wireframeLoc;
    // Highlight  loca
    GLint m_highlightLoc;
    // Shader Sampler location
    GLint m_samplerLoc;
    // Barycentric location
    GLint m_baryLoc;
    // Texture
    Texture m_texture;
};

#endif // DRAWABLE_H
