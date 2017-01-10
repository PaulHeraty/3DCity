#include "SkyBox.h"


// Constructor
SkySide::SkySide(std::string vertShader, std::string fragShader) {
    m_numVertices = 0;
    m_numIndices = 0;
    vtxStride = 0;
    m_vertexShader = vertShader;
    m_fragmentShader = fragShader;
}

// Destructor
SkySide::~SkySide() {
}

// Create
void SkySide::Create(GLvector3 lowerLeft, GLvector3 upperRight, char* texture, int texSize) {
    // location is (midX, botY, midZ) - i.e. middle of X/Z coord is middle of building, Y always 0.0 (on ground)

    // Create the program objects & texture
    CreateProgram(BARY_SHADER, m_vertexShader, m_fragmentShader, TEXTURE_FILE, texture, texSize);
    //CreateProgram(vertexShader, fragmentShader, SIMPLE_TEXTURE);

    // *** Remember: tex u=0, v=0 => upper left of texture ***

    // Setup vertex and index VBOs
    // Need seperate vertices per face (for individual texture coordinates)
    // Front Face
    if (lowerLeft.z == upperRight.z) {  // front/back sides
        vertex[1] = BaryVertexFormat2(lowerLeft.x,  lowerLeft.y, lowerLeft.z,   0.0f,  0.5f,     0,1,0);
        vertex[0] = BaryVertexFormat2(upperRight.x, lowerLeft.y, lowerLeft.z,   1.0f,  0.5f,     1,0,0);
        vertex[3] = BaryVertexFormat2(upperRight.x, upperRight.y, lowerLeft.z,   1.0f,  0.0f,    0,1,0);
        vertex[2] = BaryVertexFormat2(lowerLeft.x,  upperRight.y, lowerLeft.z,   0.0f,  0.0f,    0,0,1);
    } else if (lowerLeft.x == upperRight.x) {    // left/right sides
        vertex[0] = BaryVertexFormat2(lowerLeft.x,  lowerLeft.y,  lowerLeft.z,   0.0f,  0.5f,    1,0,0);
        vertex[1] = BaryVertexFormat2(lowerLeft.x,  upperRight.y, lowerLeft.z,  0.0f,  0.0f,     0,1,0);
        vertex[2] = BaryVertexFormat2(lowerLeft.x,  upperRight.y, upperRight.z,   1.0f,  0.0f,   0,0,1);
        vertex[3] = BaryVertexFormat2(lowerLeft.x,  lowerLeft.y,  upperRight.z,   1.0f,  0.5f,   0,1,0);
    } else if (lowerLeft.y == upperRight.y) {   // top
        vertex[1] = BaryVertexFormat2(lowerLeft.x,  lowerLeft.y, lowerLeft.z,   0.0f,  1.0f,     0,1,0);
        vertex[0] = BaryVertexFormat2(upperRight.x, lowerLeft.y, lowerLeft.z,   1.0f,  1.0f,     1,0,0);
        vertex[3] = BaryVertexFormat2(upperRight.x, lowerLeft.y, upperRight.z,   1.0f,  0.0f,    0,1,0);
        vertex[2] = BaryVertexFormat2(lowerLeft.x,  lowerLeft.y, upperRight.z,   0.0f,  0.0f,    0,0,1);
    }

    //Front
    index[0] = 0;   index[1] = 1;   index[2] = 2;
    index[3] = 0;   index[4] = 2;   index[5] = 3;

    vtxStride = sizeof(BaryVertexFormat2);
    m_numVertices = sizeof(vertex) / sizeof(BaryVertexFormat2);
    m_numIndices = sizeof(index) / sizeof(GLushort);

    // Generate VBOs
    glGenBuffers(2, vboIds);

    // Vertex Array
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, vtxStride * m_numVertices, vertex, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Array Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * m_numIndices, index, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void SkySide::Draw(CCamera *camera) {
    // Reset offset for each object
    GLuint offset = 0;

    // Use the program object
    glUseProgram ( m_programObject );

    // Use VBOs Vertex Data
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);

    // Load the vertex position attribute
    glVertexAttribPointer ( m_positionLoc, 3, GL_FLOAT,
                            //GL_FALSE, 8 * sizeof(GLfloat), (const void*)offset );
                            GL_FALSE, vtxStride, (const void*)offset );
    offset += sizeof(GLfloat) * 3;

    // Load the texture coordinate attribute
    glVertexAttribPointer ( m_texCoordLoc, 2, GL_FLOAT,
                            //GL_FALSE, 8 * sizeof(GLfloat), (const void *)offset );
                            GL_FALSE, vtxStride, (const void*)offset );
    offset += sizeof(GLfloat) * 2;

    // Load the bary coordinate attribute
    glVertexAttribPointer ( m_baryLoc, 3, GL_BYTE,
                            GL_FALSE, vtxStride, (const void*)offset );

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
    //glEnable(GL_TEXTURE_2D); DEPRECATED
    glBindTexture ( GL_TEXTURE_2D, m_texture.m_textureId );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    // Set the sampler texture unit to 0
    glUniform1i ( m_samplerLoc, 0 );

    // Set wireframe mode in shader
    glUniform1i(m_wireframeLoc, g_wireframe);

    if(g_wireframe ==2) {
        // Blend particles
        //glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
        glEnable ( GL_BLEND );
        glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glDisable(GL_CULL_FACE);
        //glDisable(GL_DEPTH_TEST);
    }

    // Submit draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    g_numOtherDraws++;

    //glEnable(GL_DEPTH_TEST);

    if(g_wireframe ==2) {
        // Disable Blend
        //glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
        glDisable(GL_BLEND );
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
    }
}

/*************************************************************
 ********************** SKYBOX *******************************
 ************************************************************/

// Constructor
SkyBox::SkyBox(std::string vertShader, std::string fragShader) {
    m_vertexShader = vertShader;
    m_fragmentShader = fragShader;
    m_numVertices = 0;
    m_numIndices = 0;
}

// Destructor
SkyBox::~SkyBox() {
}

// Create
void SkyBox::Create(unsigned int xSize, unsigned int ySize, char* texture,  int texSize) {
    char thisTex[40];

    m_xSize = xSize;
    m_ySize = ySize;
    m_zSize = ySize;

    strcpy(thisTex, texture);
    strcat(thisTex, "_front.tga");
    SkySide *frontSide = new SkySide(m_vertexShader, m_fragmentShader);
    frontSide->Create(glVector(0.0f,0.0f,0.0f), glVector(m_xSize, m_ySize/2.0, 0), thisTex, texSize);
    m_side.push_back(frontSide);
    m_numVertices += frontSide->m_numVertices;
    m_numIndices += frontSide->m_numIndices;

    strcpy(thisTex, texture);
    strcat(thisTex, "_right.tga");
    SkySide *rightSide  = new SkySide(m_vertexShader, m_fragmentShader);
    rightSide->Create(glVector(m_xSize,0,0), glVector(m_xSize, m_ySize/2.0, m_zSize), thisTex, texSize);
    m_side.push_back(rightSide);
    m_numVertices += rightSide->m_numVertices;
    m_numIndices += rightSide->m_numIndices;

    strcpy(thisTex, texture);
    strcat(thisTex, "_back.tga");
    SkySide *backSide = new SkySide(m_vertexShader, m_fragmentShader);
    backSide->Create(glVector(m_xSize,0,m_zSize), glVector(0, m_ySize/2.0, m_zSize), thisTex, texSize);
    m_side.push_back(backSide);
    m_numVertices += backSide->m_numVertices;
    m_numIndices += backSide->m_numIndices;

    strcpy(thisTex, texture);
    strcat(thisTex, "_left.tga");
    SkySide *leftSide = new SkySide(m_vertexShader, m_fragmentShader);
    leftSide->Create(glVector(0,0,m_zSize), glVector(0, m_ySize/2.0, 0), thisTex, texSize);
    m_side.push_back(leftSide);
    m_numVertices += leftSide->m_numVertices;
    m_numIndices += leftSide->m_numIndices;

    strcpy(thisTex, texture);
    strcat(thisTex, "_top.tga");
    SkySide *topSide = new SkySide(m_vertexShader, m_fragmentShader);
    topSide->Create(glVector(0,m_ySize/2.0,0), glVector(m_xSize, m_ySize/2.0, m_zSize), thisTex, texSize);
    m_side.push_back(topSide);
    m_numVertices += topSide->m_numVertices;
    m_numIndices += topSide->m_numIndices;

}

void SkyBox::Draw(CCamera* camera) {
    for(unsigned int i = 0; i < m_side.size(); i++) {
        m_side[i]->Draw(camera);
    }
}
