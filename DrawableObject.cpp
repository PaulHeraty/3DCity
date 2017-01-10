#include "DrawableObject.h"

//std::vector<Shader> programObjects;

DrawableObject::DrawableObject() {
    m_programObject = -1;
    m_shaderType = BASIC_SHADER;
    m_positionLoc = -1;
    m_texCoordLoc = -1;
    m_colorLoc = -1;
    m_mvpMatrixLoc = -1;
    m_lookAtMatrixLoc = -1;
    m_samplerLoc = -1;
    m_highlightLoc = -1;
}

DrawableObject::~DrawableObject() {
}

bool DrawableObject::ShaderExists(ShaderType shaderType, std::string vertexShader, std::string fragmentShader, GLuint &progObj) {
    for(unsigned int i = 0; i < shaderObjects.size(); i++) {
        if(shaderObjects[i].m_vertexShader == vertexShader &&
                shaderObjects[i].m_fragmentShader == fragmentShader &&
                shaderObjects[i].m_shaderType == shaderType) {
            progObj = shaderObjects[i].m_programObject;
            return true;
        }
    }
    return false;
}

void DrawableObject::CreateProgram(ShaderType shaderType, std::string vertexShader, std::string fragmentShader,
                                   textureType t, char* filename, int texSize) {
    GLint result;
    GLuint progObj;

    m_shaderType = shaderType;
    m_fragmentShader = fragmentShader;
    m_vertexShader = vertexShader;

    // Check if a similiar shader already exists
    if(ShaderExists(shaderType, vertexShader, fragmentShader, progObj)) {
        m_programObject = progObj;
    } else {        // Create new shader
        Shader newShader;
        /* create program object and attach shaders */
        m_programObject = glCreateProgram();
        printf("Compiling shader %s...\n", &vertexShader[0]);
        esShaderAttachFromFile(m_programObject, GL_VERTEX_SHADER, &vertexShader[0]);
        printf("Compiling shader %s...\n", &fragmentShader[0]);
        esShaderAttachFromFile(m_programObject, GL_FRAGMENT_SHADER, &fragmentShader[0]);

        /* link the program and make sure that there were no errors */
        glLinkProgram(m_programObject);
        glGetProgramiv(m_programObject, GL_LINK_STATUS, &result);
        if(result == GL_FALSE) {
            GLint length;
            char *log;

            /* get the program info log */
            glGetProgramiv(m_programObject, GL_INFO_LOG_LENGTH, &length);
            log = (char*) malloc(length);
            glGetProgramInfoLog(m_programObject, length, &result, log);

            /* print an error message and the info log */
            fprintf(stderr, "Init(): Program linking failed: %s\n", log);
            free(log);

            /* delete the program */
            glDeleteProgram(m_programObject);
            m_programObject = 0;
        }

        // Add this shader to global list
        newShader.m_fragmentShader = fragmentShader;
        newShader.m_vertexShader = vertexShader;
        newShader.m_shaderType = shaderType;
        newShader.m_programObject = m_programObject;
        shaderObjects.push_back(newShader);
    }

    // Get the attribute locations
    m_positionLoc = glGetAttribLocation ( m_programObject, "a_position" );

    if(m_shaderType == BASIC_SHADER || m_shaderType == HUD_SHADER ||
            m_shaderType == LIGHT_SHADER || m_shaderType == AABB_SHADER ||
            m_shaderType == ROUTE_SHADER || m_shaderType == CAR_SHADER ||
            m_shaderType == STREETLIGHT_SHADER || m_shaderType == BARY_SHADER)
        m_texCoordLoc = glGetAttribLocation ( m_programObject, "a_texCoord" );

    if(m_shaderType == LIGHT_SHADER || m_shaderType == CAR_SHADER) {
        m_lookAtMatrixLoc = glGetUniformLocation(m_programObject, "u_lookMatrix");
    }

    if(m_shaderType == LIGHT_SHADER || m_shaderType == AABB_SHADER ||
            m_shaderType == VALBAR_SHADER) {
        m_colorLoc = glGetUniformLocation ( m_programObject, "u_color" );
    }

    if(m_shaderType == ROUTE_SHADER) {
        m_highlightLoc = glGetUniformLocation ( m_programObject, "u_highlight" );
    }

    if(m_shaderType == BARY_SHADER || m_shaderType == LIGHT_SHADER ||
            m_shaderType == ROUTE_SHADER || m_shaderType == CAR_SHADER ||
            m_shaderType == STREETLIGHT_SHADER) {
        m_baryLoc = glGetAttribLocation ( m_programObject, "a_barycentric" );
    }

    // Get the uniform locations
    if(m_shaderType == BASIC_SHADER || m_shaderType == LIGHT_SHADER ||
            m_shaderType == ROUTE_SHADER || m_shaderType == CAR_SHADER ||
            m_shaderType == STREETLIGHT_SHADER || m_shaderType == BARY_SHADER) {
        m_wireframeLoc = glGetUniformLocation(m_programObject, "u_wireframe");
    }

    if(m_shaderType == BASIC_SHADER || m_shaderType == LIGHT_SHADER ||
            m_shaderType == AABB_SHADER || m_shaderType == ROUTE_SHADER ||
            m_shaderType == CAR_SHADER || m_shaderType == STREETLIGHT_SHADER ||
            m_shaderType == BARY_SHADER) {
        m_mvpMatrixLoc = glGetUniformLocation(m_programObject, "u_mvpMatrix");
    }

    // Get the sampler location
    if(m_shaderType == BASIC_SHADER || m_shaderType == LIGHT_SHADER ||
            m_shaderType == HUD_SHADER || m_shaderType == ROUTE_SHADER ||
            m_shaderType == CAR_SHADER || m_shaderType == STREETLIGHT_SHADER ||
            m_shaderType == BARY_SHADER) {
        m_samplerLoc = glGetUniformLocation(m_programObject, "s_texture" );
    }

    // Create the texture
    if(t == TEXTURE_FILE)
        m_texture.LoadTexture2D(filename, texSize);
    else if(t != TEXTURE_NONE)
        m_texture.CreateTexture2D(t);
}


