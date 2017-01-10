#include "GroundPlane.h"

// Constructor
GroundPlane::GroundPlane(std::string vertShader, std::string fragShader) {
    numVertices = 0;
    numIndices = 0;
    vtxStride = 0;
    m_vertexShader = vertShader;
    m_fragmentShader = fragShader;
}

// Destructor
GroundPlane::~GroundPlane() {
}

// Create
void GroundPlane::Create(unsigned int xSize, unsigned int zSize, textureType texType) {

    // Create the program objects & texture
    CreateProgram(BARY_SHADER, m_vertexShader, m_fragmentShader, texType);

    // Setup vertex and index VBOs
    // Don't need seperate vertices per face (since no individual texture coordinates)
    vertex[0] = BaryVertexFormat2(0.0f, 0.0f, 0.0f,                0.0f, 0.0f,   1,0,0);
    vertex[1] = BaryVertexFormat2((float)xSize, 0.0f, 0.0f,        0.0f, 0.0f,   0,1,0);
    vertex[2] = BaryVertexFormat2((float)xSize, 0.0f, (float)zSize,0.0f, 0.0f,   0,0,1);
    vertex[3] = BaryVertexFormat2(0.0f, 0.0f, (float)zSize,        0.0f, 0.0f,   0,1,0);
    //Front
    index[0] = 0;  index[1] = 1;  index[2] = 2;
    index[3] = 0;  index[4] = 2;  index[5] = 3;

    vtxStride = sizeof(BaryVertexFormat2);
    numVertices = sizeof(vertex) / sizeof(BaryVertexFormat2);
    numIndices = sizeof(index) / sizeof(GLushort);

    // Generate VBOs
    glGenBuffers(2, vboIds);

    // Vertex Array
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, vtxStride * numVertices, vertex, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Array Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * numIndices, index, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GroundPlane::Draw(CCamera* camera) {
    // Reset offset for each object
    GLuint offset = 0;

    // Use the program object
    glUseProgram ( m_programObject );

    // Use VBOs Vertex Data
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);

    // Load the vertex position attribute
    glVertexAttribPointer ( m_positionLoc, 3, GL_FLOAT,
                            GL_FALSE, vtxStride, (const void*)offset );
    offset += sizeof(GLfloat) * 3;

    // Load the texture coordinate attribute
    glVertexAttribPointer ( m_texCoordLoc, 2, GL_FLOAT,
                            GL_FALSE, vtxStride, (const void *)offset );
    offset += sizeof(GLfloat) * 2;

    // Load the bary coordinate attribute
    glVertexAttribPointer ( m_baryLoc, 3, GL_BYTE,
            GL_FALSE, vtxStride, (const void *)offset );

    // Set the mvpMatrix uniform
    GLmatrix vMatrix = camera->GetViewMatrix();
    GLmatrix pMatrix = camera->GetProjMatrix();
    GLmatrix mvpMatrix = glMatrixMultiply(vMatrix, pMatrix);
    glUniformMatrix4fv(m_mvpMatrixLoc, 1, GL_FALSE,  &mvpMatrix.elements[0][0]);

    glEnableVertexAttribArray(m_positionLoc);
    glEnableVertexAttribArray(m_texCoordLoc);
    glEnableVertexAttribArray(m_baryLoc);

    // User VBOs Index Data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);

    // Bind the texture
    glActiveTexture ( GL_TEXTURE0 );
    //glEnable(GL_TEXTURE_2D);  DEPRECATED
    glBindTexture(GL_TEXTURE_2D, m_texture.m_textureId);

    // Set the sampler texture unit to 0
    glUniform1i(m_samplerLoc, 0);

    // Set wireframe mode in shader
    glUniform1i(m_wireframeLoc, g_wireframe);

    if(g_wireframe ==2) {
        // Blend particles
        //glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
    }

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    g_numOtherDraws++;

    if(g_wireframe ==2) {
        // Disable Blend
        //glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
    }

}
