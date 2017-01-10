#include "Car.h"

Car::Car() {
    //m_vertexShader = "shaders/car.vp";
    //m_fragmentShader = "shaders/car.fp";
}

Car::~Car() {
}

void Car::Create(Material mat) {
    // Set the material type
    m_material = mat;

    m_position = glVector(0.0f, 0.0f, 0.0f);
    // Billboard coords will always be the same
    // Draw function will create a matrix to position it in world coords & to face camera
    m_vertex[0] = BaryVertexFormat2(-2.0f, -2.0f, 0.0f,  0.0f, 0.0f,     1, 0, 0);
    m_vertex[1] = BaryVertexFormat2(2.0f,  -2.0f, 0.0f,  1.0f, 0.0f,     0, 0, 1);
    m_vertex[2] = BaryVertexFormat2(2.0f,   2.0f, 0.0f,  1.0f, 1.0f,     0, 1, 0);
    m_vertex[3] = BaryVertexFormat2(-2.0f,  2.0f, 0.0f,  0.0f, 1.0f,     0, 0, 1);

    m_index[0] = 0;
    m_index[1] = 2;
    m_index[2] = 1;
    m_index[3] = 0;
    m_index[4] = 3;
    m_index[5] = 2;

    m_vtxStride = sizeof(BaryVertexFormat2);

    // Generate VBOs
    glGenBuffers(2, m_vboId);

    // Vertex Array
    glBindBuffer(GL_ARRAY_BUFFER, m_vboId[0]);
    glBufferData(GL_ARRAY_BUFFER, m_vtxStride * 4, m_vertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Array Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboId[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 6, m_index, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Car::Draw(CCamera* camera) {
    // Use the program object
    glUseProgram ( m_material.m_shader.m_programObject );

    // Calculate matrix to rotate billboard to face camera
    GLvector eyePos = camera->GetPosition();
    //GLvector lookVector = glVectorNormalize(eyePos - m_position);
    GLvector lookVector = glVectorNormalize(eyePos - camera->GetPlayer()->GetPosition());
    GLvector rightVector = glVectorNormalize(glVectorCrossProduct(glVector(0.0,1.0,0.0), lookVector));
    GLvector upVector = glVectorCrossProduct(lookVector, rightVector);

    GLmatrix lookAtMat;
    lookAtMat.elements[0][0] = rightVector.x;
    lookAtMat.elements[0][1] = rightVector.y;
    lookAtMat.elements[0][2] = rightVector.z;
    lookAtMat.elements[0][3] = 0.0f;
    lookAtMat.elements[1][0] = upVector.x;
    lookAtMat.elements[1][1] = upVector.y;
    lookAtMat.elements[1][2] = upVector.z;
    lookAtMat.elements[1][3] = 0.0f;
    lookAtMat.elements[2][0] = lookVector.x;
    lookAtMat.elements[2][1] = lookVector.y;
    lookAtMat.elements[2][2] = lookVector.z;
    lookAtMat.elements[2][3] = 0.0f;
    lookAtMat.elements[3][0] = camera->GetPlayer()->GetPosition().x;
    lookAtMat.elements[3][1] = camera->GetPlayer()->GetPosition().y;
    lookAtMat.elements[3][2] = camera->GetPlayer()->GetPosition().z;
    lookAtMat.elements[3][3] = 1.0f;

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
    glUniformMatrix4fv(m_material.m_shader.m_lookAtMatrixLoc, 1, GL_FALSE,  &lookAtMat.elements[0][0]);

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
    glUniform1i( m_material.m_shader.m_samplerLoc, 0 );

    // Set wireframe mode in shader
    glUniform1i(m_material.m_shader.m_wireframeLoc, g_wireframe);

    if(g_wireframe > 0) {
        // Blend particles
        //glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
    }

    // Draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    g_numOtherDraws++;

    // Disable Blend
    glDisable(GL_BLEND );

    if(g_wireframe > 0) {
        // Disable Blend
        //glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
    }

}

