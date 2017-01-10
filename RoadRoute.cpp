#include "RoadRoute.h"

// Constructor
RoadRoute::RoadRoute() {
    m_numVertices = 0;
    m_numIndices = 0;
    m_vtxStride = 0;
}

// Destructor
RoadRoute::~RoadRoute() {
}

// Create
void RoadRoute::Create(Material mat, GLvector3 min, GLvector3 max) {
    // Set the material type
    m_material = mat;

    float maxU = 1.0f;
    float maxV = 1.0f;

    if((max.x - min.x) > (max.z - min.z)) {
        // Path along X axis
        maxV = (max.x-min.x) / 6.0f;    // divide by path width
        // Setup vertex and index VBOs
        // Don't need seperate vertices per face (since no individual texture coordinates)
        m_vertex[0] = BaryVertexFormat2(min.x, min.y, min.z,         maxU, 0.0f,     1,0,0);
        m_vertex[1] = BaryVertexFormat2(max.x, min.y, min.z,         maxU, maxV,     0,1,0);
        m_vertex[2] = BaryVertexFormat2(max.x, max.y, max.z,         0.0f, maxV,     0,0,1);
        m_vertex[3] = BaryVertexFormat2(min.x, max.y, max.z,         0.0f, 0.0f,     0,1,0);
    } else {
        // Path along Z axis
        maxV = (max.z - min.z) / 6.0f;
        // Setup vertex and index VBOs
        // Don't need seperate vertices per face (since no individual texture coordinates)
        m_vertex[0] = BaryVertexFormat2(min.x, min.y, min.z,         0.0f, 0.0f,     1,0,0);
        m_vertex[1] = BaryVertexFormat2(max.x, min.y, min.z,         maxU, 0.0f,     0,1,0);
        m_vertex[2] = BaryVertexFormat2(max.x, max.y, max.z,         maxU, maxV,     0,0,1);
        m_vertex[3] = BaryVertexFormat2(min.x, max.y, max.z,         0.0f, maxV,     0,1,0);
    }

    //Front
    m_index[0] = 0;  m_index[1] = 1;  m_index[2] = 2;
    m_index[3] = 0;  m_index[4] = 2;  m_index[5] = 3;

    m_vtxStride = sizeof(BaryVertexFormat2);
    m_numVertices = sizeof(m_vertex) / m_vtxStride;
    m_numIndices = sizeof(m_index) / sizeof(GLushort);

    // Generate VBOs
    glGenBuffers(2, m_vboIds);

    // Vertex Array
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, m_vtxStride * m_numVertices, m_vertex, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Array Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * m_numIndices, m_index, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RoadRoute::Draw(CCamera* camera) {

    // draw road routes
    // Reset offset for each object
    GLuint offset = 0;

    // Use the program object
    glUseProgram ( m_material.m_shader.m_programObject );

    // Use VBOs Vertex Data
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);

    // Load the vertex position attribute
    glVertexAttribPointer ( m_material.m_shader.m_positionLoc, 3, GL_FLOAT,
                            GL_FALSE, m_vtxStride, (const void*)offset );
    offset += sizeof(GLfloat) * 3;

    // Load the texture coordinate attribute
    glVertexAttribPointer ( m_material.m_shader.m_texCoordLoc, 2, GL_FLOAT,
                            GL_FALSE, m_vtxStride, (const void *)offset );
    offset += sizeof(GLfloat) * 2;

    glVertexAttribPointer ( m_material.m_shader.m_baryLoc, 3, GL_BYTE,
                            GL_FALSE, m_vtxStride, (const void *)offset );

    // Set the mvpMatrix uniform
    GLmatrix vMatrix = camera->GetViewMatrix();
    GLmatrix pMatrix = camera->GetProjMatrix();
    GLmatrix mvpMatrix = glMatrixMultiply(vMatrix, pMatrix);
    glUniformMatrix4fv(m_material.m_shader.m_mvpMatrixLoc, 1, GL_FALSE,  &mvpMatrix.elements[0][0]);

    glEnableVertexAttribArray(m_material.m_shader.m_positionLoc);
    glEnableVertexAttribArray(m_material.m_shader.m_texCoordLoc);
    glEnableVertexAttribArray(m_material.m_shader.m_baryLoc);

    // User VBOs Index Data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);

    // Bind the texture
    glActiveTexture ( GL_TEXTURE0 );
    //glEnable(GL_TEXTURE_2D); DEPRECATED
    //glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    //glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glBindTexture ( GL_TEXTURE_2D, m_material.m_texture.m_textureId );

    // Set the sampler texture unit to 0
    glUniform1i ( m_material.m_shader.m_samplerLoc, 0 );

    // Set wireframe mode in shader
    glUniform1i(m_material.m_shader.m_wireframeLoc, g_wireframe);

    if(g_wireframe ==2) {
        // Blend particles
        //glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
        glEnable ( GL_BLEND );
        glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glDisable(GL_CULL_FACE);
    }

    // Set highlight mode in shader
    glUniform1i(m_material.m_shader.m_highlightLoc, 1);

    // Submit draw
    glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_SHORT, 0);
    g_numRoadDraws++;

    if(g_wireframe ==2) {
        // Disable Blend
        //glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
        glDisable(GL_BLEND );
        glEnable(GL_CULL_FACE);
    }

}
