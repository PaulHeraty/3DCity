#include "Cars.h"

Cars::Cars() {
    m_draw = true;
    m_numIndices = 0;
    m_numVertices = 0;
    m_vtxStride = sizeof(BaryVertexFormat2);
    m_codStride = sizeof(CarColorOffsetDir);
}

Cars::~Cars() {
}

void Cars::Init(Material mat) {
    // Set the material type
    m_material = mat;
}

void Cars::AddCar(float offset[3], CarDirection dir) {
    CarColorOffsetDir cod;

    // Add Color/Offset/Direction
    cod.m_dir = dir;
    cod.m_offset[0] = offset[0];
    cod.m_offset[1] = offset[1];
    cod.m_offset[2] = offset[2];
    cod.m_offset[3] = 0.0f;

    // Add vertices
    if(dir == POSX) {
        // Back
        BaryVertexFormat2 bv;
        bv = BaryVertexFormat2(0.0f, 0.0f,   0.0f,      0.0f, 1.0f,     1, 0, 0);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(0.0f, 0.0f,   2.0f,      1.0f, 1.0f,     0, 1, 0);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(0.0f, 2.0f,   2.0f,      1.0f, 0.0f,     0, 0, 1);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(0.0f, 2.0f,   0.0f,      0.0f, 0.0f,     0, 1, 0);
        m_vertex.push_back(bv);
        // RED
        cod.m_color[0] = 245.0f/256.0f;
        cod.m_color[1] = 26.0f/256.0f;
        cod.m_color[2] = 22.0f/256.0f;
        cod.m_color[3] = 0.5f;
        // Add Color/Offset/Direction to list
        // Every 4 vertices will have the same color/offset, so push back 4 times
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
        // Front
        bv = BaryVertexFormat2(2.0f, 0.0f,   0.0f,      0.0f, 1.0f,     1, 0, 0);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(2.0f, 0.0f,   2.0f,      1.0f, 1.0f,     0, 1, 0);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(2.0f, 2.0f,   2.0f,      1.0f, 0.0f,     0, 0, 1);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(2.0f, 2.0f,   0.0f,      0.0f, 0.0f,     0, 1, 0);
        m_vertex.push_back(bv);
        // WHITE
        cod.m_color[0] = 236.0f/256.0f;
        cod.m_color[1] = 245.0f/256.0f;
        cod.m_color[2] = 136.0f/256.0f;
        cod.m_color[3] = 0.5f;
        // Add Color/Offset/Direction to list
        // Every 4 vertices will have the same color/offset, so push back 4 times
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
    } else if(dir == NEGX) {
        // FRONT
        BaryVertexFormat2 bv;
        bv = BaryVertexFormat2(0.0f, 0.0f,   0.0f,      0.0f, 1.0f,     1, 0, 0);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(0.0f, 0.0f,   2.0f,      1.0f, 1.0f,     0, 1, 0);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(0.0f, 2.0f,   2.0f,      1.0f, 0.0f,     0, 0, 1);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(0.0f, 2.0f,   0.0f,      0.0f, 0.0f,     0, 1, 0);
        m_vertex.push_back(bv);
        // WHITE
        cod.m_color[0] = 236.0f/256.0f;
        cod.m_color[1] = 245.0f/256.0f;
        cod.m_color[2] = 136.0f/256.0f;
        cod.m_color[3] = 0.5f;
        // Add Color/Offset/Direction to list
        // Every 4 vertices will have the same color/offset, so push back 4 times
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
        // Back
        bv = BaryVertexFormat2(2.0f, 0.0f,   0.0f,      0.0f, 1.0f,     1, 0, 0);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(2.0f, 0.0f,   2.0f,      1.0f, 1.0f,     0, 1, 0);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(2.0f, 2.0f,   2.0f,      1.0f, 0.0f,     0, 0, 1);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(2.0f, 2.0f,   0.0f,      0.0f, 0.0f,     0, 1, 0);
        m_vertex.push_back(bv);
        // RED
        cod.m_color[0] = 245.0f/256.0f;
        cod.m_color[1] = 26.0f/256.0f;
        cod.m_color[2] = 22.0f/256.0f;
        cod.m_color[3] = 0.5f;
        // Add Color/Offset/Direction to list
        // Every 4 vertices will have the same color/offset, so push back 4 times
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
    } else if(dir == POSZ) {
        // Front
        BaryVertexFormat2 bv;
        bv = BaryVertexFormat2(0.0f, 0.0f,   2.0f,      0.0f, 1.0f,     1, 0, 0);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(2.0f, 0.0f,   2.0f,      1.0f, 1.0f,     0, 1, 0);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(2.0f, 2.0f,   2.0f,      1.0f, 0.0f,     0, 0, 1);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(0.0f, 2.0f,   2.0f,      0.0f, 0.0f,     0, 1, 0);
        m_vertex.push_back(bv);
        // White
        cod.m_color[0] = 236.0f/256.0f;
        cod.m_color[1] = 245.0f/256.0f;
        cod.m_color[2] = 136.0f/256.0f;
        cod.m_color[3] = 0.5f;
        // Add Color/Offset/Direction to list
        // Every 4 vertices will have the same color/offset, so push back 4 times
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
        // Back
        bv = BaryVertexFormat2(0.0f, 0.0f,   0.0f,      0.0f, 1.0f,     1, 0, 0);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(2.0f, 0.0f,   0.0f,      1.0f, 1.0f,     0, 1, 0);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(2.0f, 2.0f,   0.0f,      1.0f, 0.0f,     0, 0, 1);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(0.0f, 2.0f,   0.0f,      0.0f, 0.0f,     0, 1, 0);
        m_vertex.push_back(bv);
        // RED
        cod.m_color[0] = 245.0f/256.0f;
        cod.m_color[1] = 26.0f/256.0f;
        cod.m_color[2] = 22.0f/256.0f;
        cod.m_color[3] = 0.5f;
        // Add Color/Offset/Direction to list
        // Every 4 vertices will have the same color/offset, so push back 4 times
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
    } else if(dir == NEGZ) {
        // Back
        BaryVertexFormat2 bv;
        bv = BaryVertexFormat2(0.0f, 0.0f,   2.0f,      0.0f, 1.0f,     1, 0, 0);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(2.0f, 0.0f,   2.0f,      1.0f, 1.0f,     0, 1, 0);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(2.0f, 2.0f,   2.0f,      1.0f, 0.0f,     0, 0, 1);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(0.0f, 2.0f,   2.0f,      0.0f, 0.0f,     0, 1, 0);
        m_vertex.push_back(bv);
        // WHITE
        cod.m_color[0] = 245.0f/256.0f;
        cod.m_color[1] = 26.0f/256.0f;
        cod.m_color[2] = 22.0f/256.0f;
        cod.m_color[3] = 0.5f;
        // Add Color/Offset/Direction to list
        // Every 4 vertices will have the same color/offset, so push back 4 times
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
        // Front
        bv = BaryVertexFormat2(0.0f, 0.0f,   0.0f,      0.0f, 1.0f,     1, 0, 0);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(2.0f, 0.0f,   0.0f,      1.0f, 1.0f,     0, 1, 0);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(2.0f, 2.0f,   0.0f,      1.0f, 0.0f,     0, 0, 1);
        m_vertex.push_back(bv);
        bv = BaryVertexFormat2(0.0f, 2.0f,   0.0f,      0.0f, 0.0f,     0, 1, 0);
        m_vertex.push_back(bv);
        // WHITE
        cod.m_color[0] = 236.0f/256.0f;
        cod.m_color[1] = 245.0f/256.0f;
        cod.m_color[2] = 136.0f/256.0f;
        cod.m_color[3] = 0.5f;
        // Add Color/Offset/Direction to list
        // Every 4 vertices will have the same color/offset, so push back 4 times
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
        m_colorOffsetDir.push_back(cod);
    }

    // Add indices
    m_index.push_back(m_numVertices);
    m_index.push_back(m_numVertices + 2);
    m_index.push_back(m_numVertices + 1);
    m_index.push_back(m_numVertices);
    m_index.push_back(m_numVertices + 3);
    m_index.push_back(m_numVertices + 2);
    m_numVertices +=4;
    m_index.push_back(m_numVertices);
    m_index.push_back(m_numVertices + 1);
    m_index.push_back(m_numVertices + 2);
    m_index.push_back(m_numVertices);
    m_index.push_back(m_numVertices + 2);
    m_index.push_back(m_numVertices + 3);
    m_numVertices +=4;

    m_numIndices += 12;
}

void Cars::FinishInit() {
    // Generate VBOs
    glGenBuffers(2, m_vboId);

    // Vertex Array
    glBindBuffer(GL_ARRAY_BUFFER, m_vboId[0]);
    glBufferData(GL_ARRAY_BUFFER, m_vtxStride * m_numVertices, &m_vertex[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Array Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboId[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * m_numIndices, &m_index[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Generate COD VBOs
    glGenBuffers(1, m_codId);

    // COD Array
    glBindBuffer(GL_ARRAY_BUFFER, m_codId[0]);
    glBufferData(GL_ARRAY_BUFFER, m_codStride * m_numVertices, &m_colorOffsetDir[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Cars::Draw(CCamera* camera) {
    // Should we draw?
    if(!m_draw)
        return;

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

    // Bind Color/Offset Vertex Data
    glBindBuffer(GL_ARRAY_BUFFER, m_codId[0]);

    offset = 0;
    // Load the offset attribute
    glVertexAttribPointer ( m_material.m_shader.m_offsetLoc, 4, GL_FLOAT,
                            GL_FALSE, m_codStride, (const void*)offset );
    offset += sizeof(GLfloat) * 4;
    // Load the color attribute
    glVertexAttribPointer ( m_material.m_shader.m_lightColorLoc, 4, GL_FLOAT,
                            GL_FALSE, m_codStride, (const void *)offset );

    // Set the mvpMatrix uniform
    GLmatrix vMatrix = camera->GetViewMatrix();
    GLmatrix pMatrix = camera->GetProjMatrix();
    GLmatrix mvpMatrix = glMatrixMultiply(vMatrix, pMatrix);
    glUniformMatrix4fv(m_material.m_shader.m_mvpMatrixLoc, 1, GL_FALSE,  &mvpMatrix.elements[0][0]);

    glEnableVertexAttribArray ( m_material.m_shader.m_positionLoc );
    glEnableVertexAttribArray ( m_material.m_shader.m_texCoordLoc );
    glEnableVertexAttribArray ( m_material.m_shader.m_baryLoc );
    glEnableVertexAttribArray ( m_material.m_shader.m_offsetLoc );
    glEnableVertexAttribArray ( m_material.m_shader.m_lightColorLoc );

//    // Set color uniform
//    glUniform4fv(m_material.m_shader.m_colorLoc, 1, &m_color[0] );

//    // Set offset uniform
//    glUniform4fv(m_material.m_shader.m_offsetLoc, 1, &m_offset[0] );

    // User VBOs Index Data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboId[1]);

    // Blend particles
    glEnable ( GL_BLEND );
    glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    // Disable back face culling
    //glDisable(GL_CULL_FACE);

    // Bind the texture
    glActiveTexture( GL_TEXTURE0 );
    // glEnable(GL_TEXTURE_2D);  DEPRECATED
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
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_SHORT, 0);

    // Disable Blend
    glDisable(GL_BLEND );

    // Enable Backface culling
    //glEnable(GL_CULL_FACE);

    if(g_wireframe ==2) {
        // Disable Blend
        //glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
    }
}

void Cars::Update(CCamera* camera, float deltaTime) {

//    // Always draw lights in FUNCTEST_MODE
//    if(g_appMode == FUNCTEST_MODE) {
//        m_draw = false;
//    } else {
//        m_draw = true;
//    }

    if(m_draw) {
        float speed = deltaTime * 10.0;
        // Update cars
        for(int p=0; p < m_numVertices; p++) {
            if(m_colorOffsetDir[p].m_dir == POSX) {
                m_colorOffsetDir[p].m_offset[0]+= speed;
                if(m_colorOffsetDir[p].m_offset[0] > 1024.0)
                    m_colorOffsetDir[p].m_offset[0] = 0.0f;
            } else if(m_colorOffsetDir[p].m_dir == NEGX) {
                m_colorOffsetDir[p].m_offset[0]-= speed;
                if(m_colorOffsetDir[p].m_offset[0] < 0.0)
                    m_colorOffsetDir[p].m_offset[0] = 1024.0f;
            } else if(m_colorOffsetDir[p].m_dir == POSZ) {
                m_colorOffsetDir[p].m_offset[2]+= speed;
                if(m_colorOffsetDir[p].m_offset[2] > 1024.0)
                    m_colorOffsetDir[p].m_offset[2] = 0.0f;
            } else if(m_colorOffsetDir[p].m_dir == NEGZ) {
                m_colorOffsetDir[p].m_offset[2]-= speed;
                if(m_colorOffsetDir[p].m_offset[2] < 0.0)
                    m_colorOffsetDir[p].m_offset[2] = 1024.0f;
            }
        }

        // Regen COD VBO
        glBindBuffer(GL_ARRAY_BUFFER, m_codId[0]);
        glBufferData(GL_ARRAY_BUFFER, m_codStride * m_numVertices, &m_colorOffsetDir[0], GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

