#include "Light.h"


// Define the class static member variables
std::vector<BaryVertexFormat2> Light::m_vertex;
std::vector<GLushort> Light::m_index;
GLuint          Light::m_vboId[2];
GLint           Light::m_vtxStride;

Light::Light() {
    m_blink = false;
    m_time = 0.0f;
    m_draw = true;
    m_lookAtMat = glMatrixIdentity();

    m_color[0] = 0.0f;
    m_color[1] = 0.0f;
    m_color[2] = 0.0f;
    m_color[3] = 0.0f;

    // Billboard coords will always be the same
    // Draw function will create a matrix to position it in world coords & to face camera
    BaryVertexFormat2 bv;
    bv = BaryVertexFormat2(-0.5f, -0.5f, 0.0f,  0.0f, 1.0f,     1, 0, 0);
    m_vertex.push_back(bv);
    bv = BaryVertexFormat2(0.5f,  -0.5f, 0.0f,  1.0f, 1.0f,     0, 1, 0);
    m_vertex.push_back(bv);
    bv = BaryVertexFormat2(0.5f,   0.5f, 0.0f,  1.0f, 0.0f,     0, 0, 1);
    m_vertex.push_back(bv);
    bv = BaryVertexFormat2(-0.5f,  0.5f, 0.0f,  0.0f, 0.0f,     0, 1, 0);
    m_vertex.push_back(bv);

    m_index.push_back(0);
    m_index.push_back(2);
    m_index.push_back(1);
    m_index.push_back(0);
    m_index.push_back(3);
    m_index.push_back(2);

    m_vtxStride = sizeof(BaryVertexFormat2);

    // Generate VBOs
    glGenBuffers(2, m_vboId);

    // Vertex Array
    glBindBuffer(GL_ARRAY_BUFFER, m_vboId[0]);
    glBufferData(GL_ARRAY_BUFFER, m_vtxStride * 4, &m_vertex[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Array Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboId[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 6, &m_index[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Light::~Light() {
}

void Light::Create(Material mat, GLvector pos) {
    // Set the material type
    m_material = mat;

    // Set light position
    m_position = pos;

    // Set color
    LightColor color = LightColor(RandomVal(4));
    if(color == RED) {
        m_color[0] = 1.0f;
        m_color[1] = 0.0f;
        m_color[2] = 0.0f;
        m_color[3] = 1.0f;
    } else if(color == GREEN) {
        m_color[0] = 0.0f;
        m_color[1] = 1.0f;
        m_color[2] = 0.0f;
        m_color[3] = 1.0f;
    } else if(color == CYAN) {
        m_color[0] = 0.0f;
        m_color[1] = 1.0f;
        m_color[2] = 1.0f;
        m_color[3] = 1.0f;
    } else if(color == YELLOW) {
        m_color[0] = 1.0f;
        m_color[1] = 1.0f;
        m_color[2] = 0.0f;
        m_color[3] = 1.0f;
    }

    // Randomly blink the light
    if(COIN_FLIP) {
        m_blink = true;
        m_time = 1.0f / RandomVal(10);
    }
}

void Light::Draw(CCamera* camera) {
    // Blink?
    if(m_draw) {
        // Use the program object
        glUseProgram ( m_material.m_shader.m_programObject );

        // Reset offset for each object
        GLuint offset = 0;

        // Use VBOs Vertex Data
        glBindBuffer(GL_ARRAY_BUFFER, m_vboId[0]);

        // Load the vertex position attribute
        glVertexAttribPointer ( m_material.m_shader.m_positionLoc, 3, GL_FLOAT,
                                GL_FALSE, m_vtxStride, (const void*)offset );
        offset += sizeof(GLfloat) * 3;
        // Load the texture coordinate attribute
        glVertexAttribPointer ( m_material.m_shader.m_texCoordLoc, 2, GL_FLOAT,
                                GL_FALSE, m_vtxStride, (const void *)offset );
        offset += sizeof(GLfloat) * 2;

        // Load the bary coordinate attribute
        glVertexAttribPointer ( m_material.m_shader.m_baryLoc, 3, GL_BYTE,
                                GL_FALSE, m_vtxStride, (const void *)offset );

        // Set the mvpMatrix uniform
        GLmatrix vMatrix = camera->GetViewMatrix();
        GLmatrix pMatrix = camera->GetProjMatrix();
        GLmatrix mvpMatrix = glMatrixMultiply(vMatrix, pMatrix);
        glUniformMatrix4fv(m_material.m_shader.m_mvpMatrixLoc, 1, GL_FALSE,  &mvpMatrix.elements[0][0]);

        glEnableVertexAttribArray ( m_material.m_shader.m_positionLoc );
        glEnableVertexAttribArray ( m_material.m_shader.m_texCoordLoc );
        glEnableVertexAttribArray ( m_material.m_shader.m_baryLoc );


        // Set lookAt Matrix uniform
        glUniformMatrix4fv(m_material.m_shader.m_lookAtMatrixLoc, 1, GL_FALSE,  &m_lookAtMat.elements[0][0]);

        // Set color uniform
        glUniform4fv(m_material.m_shader.m_colorLoc, 1, &m_color[0] );

        // User VBOs Index Data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboId[1]);

        // Blend particles
        glEnable ( GL_BLEND );
        glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        // Bind the texture
        glActiveTexture( GL_TEXTURE0 );
        // glEnable(GL_TEXTURE_2D); DEPRECATED
        glBindTexture( GL_TEXTURE_2D, m_material.m_texture.m_textureId );

        // Set the sampler texture unit to 0
        glUniform1i(m_material.m_shader. m_samplerLoc, 0 );

        // Set wireframe mode in shader
        glUniform1i(m_material.m_shader.m_wireframeLoc, g_wireframe);

        if(g_wireframe ==2) {
            // Blend particles
            //glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
            glDisable(GL_CULL_FACE);
            glDisable(GL_DEPTH_TEST);
        }

        // Draw
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

        // Disable Blend
        glDisable(GL_BLEND );

        if(g_wireframe ==2) {
            // Disable Blend
            //glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
        }
    }

}

void Light::Update(CCamera* camera, float deltaTime) {
    // Calculate matrix to rotate billboard to face camera
    GLvector eyePos = camera->GetPosition();
    GLvector lookVector = glVectorNormalize(eyePos - m_position);
    GLvector rightVector = glVectorNormalize(glVectorCrossProduct(glVector(0.0,1.0,0.0), lookVector));
    GLvector upVector = glVectorCrossProduct(lookVector, rightVector);

    //GLmatrix lookAtMat;
    m_lookAtMat.elements[0][0] = rightVector.x;
    m_lookAtMat.elements[0][1] = rightVector.y;
    m_lookAtMat.elements[0][2] = rightVector.z;
    m_lookAtMat.elements[0][3] = 0.0f;
    m_lookAtMat.elements[1][0] = upVector.x;
    m_lookAtMat.elements[1][1] = upVector.y;
    m_lookAtMat.elements[1][2] = upVector.z;
    m_lookAtMat.elements[1][3] = 0.0f;
    m_lookAtMat.elements[2][0] = lookVector.x;
    m_lookAtMat.elements[2][1] = lookVector.y;
    m_lookAtMat.elements[2][2] = lookVector.z;
    m_lookAtMat.elements[2][3] = 0.0f;
    m_lookAtMat.elements[3][0] = m_position.x;
    m_lookAtMat.elements[3][1] = m_position.y;
    m_lookAtMat.elements[3][2] = m_position.z;
    m_lookAtMat.elements[3][3] = 1.0f;

    // Update blink
    if(m_blink) {
        m_time += deltaTime;
        if(m_time > 1.0) {
            m_time = 0.0f;
            if(m_draw)
                m_draw = false;
            else
                m_draw = true;
        }
    }

    // Always draw lights in FUNCTEST_MODE
    if(g_appMode == FUNCTEST_MODE) {
        m_draw = true;
    }
}
