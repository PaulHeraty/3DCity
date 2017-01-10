#include "Material.h"

//std::vector<ShaderObject> shaderObjects;

State::State() {
    m_alpha = false;
    m_visible = true;
}

State::~State() {
}

Material::Material() {
    m_shader.m_programObject = 0;
    m_shader.m_shaderType = BASIC_SHADER;
    m_shader.m_positionLoc = -1;
    m_shader.m_texCoordLoc = -1;
    m_shader.m_colorLoc = -1;
    m_shader.m_mvpMatrixLoc = -1;
    m_shader.m_lookAtMatrixLoc = -1;
    m_shader.m_wireframeLoc = -1;
    m_shader.m_samplerLoc = -1;
    m_shader.m_highlightLoc = -1;
    m_shader.m_baryLoc = -1;
    m_shader.m_offsetLoc = -1;
    m_matType = BLACK_MAT;
    m_shader.u_texCoordOffsetX = -1;
    m_shader.u_texCoordOffsetY = -1;
}

Material::~Material() {
}

bool Material::ShaderExists(ShaderType shaderType, std::string vertexShader, std::string fragmentShader, GLuint &progObj) {
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

void Material::CreateProgram(ShaderType shaderType, std::string vertexShader, std::string fragmentShader,
                                   textureType t, char* filename, int texSize) {
    GLint result;
    GLuint progObj;

    // **** CREATE THE SHADER ****

    m_shader.m_shaderType = shaderType;
    m_shader.m_fragmentShader = fragmentShader;
    m_shader.m_vertexShader = vertexShader;

    // Check if a similiar shader already exists
    if(ShaderExists(shaderType, vertexShader, fragmentShader, progObj)) {
        m_shader.m_programObject = progObj;
    } else {        // Create new shader
        Shader newShader;
        /* create program object and attach shaders */
        m_shader.m_programObject = glCreateProgram();
        printf("Compiling shader %s...\n", &vertexShader[0]);
        esShaderAttachFromFile(m_shader.m_programObject, GL_VERTEX_SHADER, &vertexShader[0]);
        printf("Compiling shader %s...\n", &fragmentShader[0]);
        esShaderAttachFromFile(m_shader.m_programObject, GL_FRAGMENT_SHADER, &fragmentShader[0]);

        /* link the program and make sure that there were no errors */
        glLinkProgram(m_shader.m_programObject);
        glGetProgramiv(m_shader.m_programObject, GL_LINK_STATUS, &result);
        if(result == GL_FALSE) {
            GLint length;
            char *log;

            /* get the program info log */
            glGetProgramiv(m_shader.m_programObject, GL_INFO_LOG_LENGTH, &length);
            log = (char*) malloc(length);
            glGetProgramInfoLog(m_shader.m_programObject, length, &result, log);

            /* print an error message and the info log */
            fprintf(stderr, "Init(): Program linking failed: %s\n", log);
            free(log);

            /* delete the program */
            glDeleteProgram(m_shader.m_programObject);
            m_shader.m_programObject = 0;
        }

        // Add this shader to global list
        newShader.m_fragmentShader = fragmentShader;
        newShader.m_vertexShader = vertexShader;
        newShader.m_shaderType = shaderType;
        newShader.m_programObject = m_shader.m_programObject;
        shaderObjects.push_back(newShader);
    }

    // Get the attribute locations
    m_shader.m_positionLoc = glGetAttribLocation ( m_shader.m_programObject, "a_position" );

    if(m_shader.m_shaderType == BASIC_SHADER || m_shader.m_shaderType == HUD_SHADER ||
            m_shader.m_shaderType == LIGHT_SHADER || m_shader.m_shaderType == AABB_SHADER ||
            m_shader.m_shaderType == ROUTE_SHADER || m_shader.m_shaderType == CAR_SHADER ||
            m_shader.m_shaderType == STREETLIGHT_SHADER || m_shader.m_shaderType == BARY_SHADER ||
            m_shader.m_shaderType == CARS_SHADER || m_shader.m_shaderType == FXAA_SHADER)
        m_shader.m_texCoordLoc = glGetAttribLocation ( m_shader.m_programObject, "a_texCoord" );

    if(m_shader.m_shaderType == LIGHT_SHADER || m_shader.m_shaderType == CAR_SHADER) {
        m_shader.m_lookAtMatrixLoc = glGetUniformLocation(m_shader.m_programObject, "u_lookMatrix");
    }

    if(m_shader.m_shaderType == LIGHT_SHADER || m_shader.m_shaderType == AABB_SHADER ||
            m_shader.m_shaderType == DEPTH_COMPLEXITY_SHADER) {
        m_shader.m_colorLoc = glGetUniformLocation ( m_shader.m_programObject, "u_color" );
    }

    if(m_shader.m_shaderType == ROUTE_SHADER) {
        m_shader.m_highlightLoc = glGetUniformLocation ( m_shader.m_programObject, "u_highlight" );
    }

    if(m_shader.m_shaderType == CARS_SHADER) {
        m_shader.m_offsetLoc = glGetAttribLocation ( m_shader.m_programObject, "a_offset" );
    }

    if(m_shader.m_shaderType == CARS_SHADER) {
        m_shader.m_lightColorLoc = glGetAttribLocation ( m_shader.m_programObject, "a_lightColor" );
    }

    if(m_shader.m_shaderType == BARY_SHADER || m_shader.m_shaderType == LIGHT_SHADER ||
            m_shader.m_shaderType == ROUTE_SHADER || m_shader.m_shaderType == CAR_SHADER ||
            m_shader.m_shaderType == STREETLIGHT_SHADER || m_shader.m_shaderType == CARS_SHADER) {
        m_shader.m_baryLoc = glGetAttribLocation ( m_shader.m_programObject, "a_barycentric" );
    }

    // Get the uniform locations
    if(m_shader.m_shaderType == BASIC_SHADER || m_shader.m_shaderType == LIGHT_SHADER ||
            m_shader.m_shaderType == ROUTE_SHADER || m_shader.m_shaderType == CAR_SHADER ||
            m_shader.m_shaderType == STREETLIGHT_SHADER || m_shader.m_shaderType == BARY_SHADER ||
            m_shader.m_shaderType == CARS_SHADER) {
        m_shader.m_wireframeLoc = glGetUniformLocation(m_shader.m_programObject, "u_wireframe");
    }

    if(m_shader.m_shaderType == BASIC_SHADER || m_shader.m_shaderType == LIGHT_SHADER ||
            m_shader.m_shaderType == AABB_SHADER || m_shader.m_shaderType == ROUTE_SHADER ||
            m_shader.m_shaderType == CAR_SHADER || m_shader.m_shaderType == STREETLIGHT_SHADER ||
            m_shader.m_shaderType == BARY_SHADER || m_shader.m_shaderType == CARS_SHADER) {
        m_shader.m_mvpMatrixLoc = glGetUniformLocation(m_shader.m_programObject, "u_mvpMatrix");
    }

    // Get the sampler location
    if(m_shader.m_shaderType == BASIC_SHADER || m_shader.m_shaderType == LIGHT_SHADER ||
            m_shader.m_shaderType == HUD_SHADER || m_shader.m_shaderType == ROUTE_SHADER ||
            m_shader.m_shaderType == CAR_SHADER || m_shader.m_shaderType == STREETLIGHT_SHADER ||
            m_shader.m_shaderType == BARY_SHADER || m_shader.m_shaderType == CARS_SHADER ||
            m_shader.m_shaderType == FXAA_SHADER) {
        m_shader.m_samplerLoc = glGetUniformLocation(m_shader.m_programObject, "s_texture" );
    }

    // Get the FXAA texCoordOffset locations
    if(m_shader.m_shaderType == FXAA_SHADER) {
        m_shader.u_texCoordOffsetX = glGetUniformLocation(m_shader.m_programObject, "u_texCoordOffsetX" );
        m_shader.u_texCoordOffsetY = glGetUniformLocation(m_shader.m_programObject, "u_texCoordOffsetY" );
    }

    // **** CREATE THE TEXTURE ****
    if(t == TEXTURE_FILE)
        m_texture.LoadTexture2D(filename, texSize);
    else if(t != TEXTURE_NONE)
        m_texture.CreateTexture2D(t);
}



