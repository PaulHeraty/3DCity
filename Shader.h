#ifndef SHADER_H
#define SHADER_H

#include "Common.h"

// Define OpenGL functions
//#if defined(_WIN32) || defined(WIN32)
//#include "Windows/OGLDefs.h"
//#endif

enum ShaderType {
    BASIC_SHADER,
    LIGHT_SHADER,
    HUD_SHADER,
    AABB_SHADER,
    ROUTE_SHADER,
    CAR_SHADER,
    CARS_SHADER,
    STREETLIGHT_SHADER,
    BARY_SHADER,
    DEPTH_COMPLEXITY_SHADER,
    VALBAR_SHADER,
    FXAA_SHADER
};

struct Shader {
    ShaderType  m_shaderType;
    std::string m_vertexShader;
    std::string m_fragmentShader;
    GLuint m_programObject;
    // Shader attribute locations
    GLint m_positionLoc;
    GLint m_texCoordLoc;
    GLint m_colorLoc;
    GLint m_mvpMatrixLoc;
    GLint m_lookAtMatrixLoc;
    GLint m_wireframeLoc;
    GLint m_highlightLoc;
    GLint m_samplerLoc;
    GLint m_baryLoc;
    GLint m_offsetLoc;
    GLint m_lightColorLoc;
    GLfloat u_texCoordOffsetX;
    GLfloat u_texCoordOffsetY;
};

void  esLogMessage ( const char *formatStr, ... );
GLuint  esLoadShader ( GLenum type, const char *shaderSrc );
GLuint  esLoadProgram ( const char *vertShaderSrc, const char *fragShaderSrc );
static char * esShaderLoadSource(const char *filePath);
static GLuint esShaderCompileFromFile( GLenum type, const char* filePath);
void esShaderAttachFromFile(GLuint program, GLenum type, const char *filePath);

#endif // SHADER_H
