#include "ValidationBar.h"

ValidationBar::ValidationBar() {
}

ValidationBar::~ValidationBar() {
}

bool ValidationBar::Create() {
    // Setup colors
    m_currentColor = 0;
    // WHITE
    m_barColor[0].color[0] = 1.0f;  //r
    m_barColor[0].color[1] = 1.0f;  //g
    m_barColor[0].color[2] = 1.0f;  //b
    m_barColor[0].color[3] = 1.0f;  //a
    // LIME
    m_barColor[1].color[0] = 0.0f;  //r
    m_barColor[1].color[1] = 1.0f;  //g
    m_barColor[1].color[2] = 0.0f;  //b
    m_barColor[1].color[3] = 1.0f;  //a
    // BLUE
    m_barColor[2].color[0] = 0.0f;  //r
    m_barColor[2].color[1] = 0.0f;  //g
    m_barColor[2].color[2] = 1.0f;  //b
    m_barColor[2].color[3] = 1.0f;  //a
    // RED
    m_barColor[3].color[0] = 1.0f;  //r
    m_barColor[3].color[1] = 0.0f;  //g
    m_barColor[3].color[2] = 0.0f;  //b
    m_barColor[3].color[3] = 1.0f;  //a
    // TEAL
    m_barColor[4].color[0] = 0.0f;  //r
    m_barColor[4].color[1] = 0.597f;  //g  0x99
    m_barColor[4].color[2] = 0.597f;  //b
    m_barColor[4].color[3] = 1.0f;  //a
    // NAVY
    m_barColor[5].color[0] = 0.0f;  //r
    m_barColor[5].color[1] = 0.0f;  //g
    m_barColor[5].color[2] = 0.5f;  //b
    m_barColor[5].color[3] = 1.0f;  //a
    // GREEN
    m_barColor[6].color[0] = 0.0f;  //r
    m_barColor[6].color[1] = 0.597f;  //g
    m_barColor[6].color[2] = 0.0f;  //b
    m_barColor[6].color[3] = 1.0f;  //a
    // AQUA
    m_barColor[7].color[0] = 0.0f;  //r
    m_barColor[7].color[1] = 1.0f;  //g
    m_barColor[7].color[2] = 1.0f;  //b
    m_barColor[7].color[3] = 1.0f;  //a
    // OLIVE
    m_barColor[8].color[0] = 0.488f;  //r  0x7d
    m_barColor[8].color[1] = 0.488f;  //g
    m_barColor[8].color[2] = 0.0f;  //b
    m_barColor[8].color[3] = 1.0f;  //a
    // SILVER
    m_barColor[9].color[0] = 0.625f;  //r  0xa0
    m_barColor[9].color[1] = 0.625f;  //g
    m_barColor[9].color[2] = 0.625f;  //b
    m_barColor[9].color[3] = 1.0f;  //a
    // PURPLE
    m_barColor[10].color[0] = 0.5f;  //r
    m_barColor[10].color[1] = 0.0f;  //g
    m_barColor[10].color[2] = 0.5f;  //b
    m_barColor[10].color[3] = 1.0f;  //a
    // OLIVE2
    m_barColor[11].color[0] = 0.5f;  //r
    m_barColor[11].color[1] = 0.5f;  //g
    m_barColor[11].color[2] = 0.0f;  //b
    m_barColor[11].color[3] = 1.0f;  //a
    // GREY
    m_barColor[12].color[0] = 0.859f;  //r  0xdc
    m_barColor[12].color[1] = 0.859f;  //g
    m_barColor[12].color[2] = 0.859f;  //b
    m_barColor[12].color[3] = 1.0f;  //a
    // FUCHSIA
    m_barColor[13].color[0] = 1.0f;  //r
    m_barColor[13].color[1] = 0.0f;  //g
    m_barColor[13].color[2] = 1.0f;  //b
    m_barColor[13].color[3] = 1.0f;  //a
    // YELLOW
    m_barColor[14].color[0] = 1.0f;  //r
    m_barColor[14].color[1] = 1.0f;  //g
    m_barColor[14].color[2] = 0.0f;  //b
    m_barColor[14].color[3] = 1.0f;  //a
    // ORANGE
    m_barColor[15].color[0] = 1.0f;  //r
    m_barColor[15].color[1] = 0.8125f;  //g
    m_barColor[15].color[2] = 0.0f;  //b
    m_barColor[15].color[3] = 1.0f;  //a

    // Create the program objects & texture
    CreateProgram(VALBAR_SHADER, "shaders/valbar.vp", "shaders/valbar.fp", TEXTURE_NONE);

    // Vertices
    m_vertex[0] = VertexFormat(-1.0,  1.0, 0.0f, 0.0f, 0.0f);   // x,y,z,u,v
    m_vertex[1] = VertexFormat(-0.9,  1.0, 0.0f, 1.0f, 0.0f);
    m_vertex[2] = VertexFormat(-0.9, -1.0, 0.0f, 1.0f, 1.0f);
    m_vertex[3] = VertexFormat(-1.0, -1.0, 0.0f, 0.0f, 1.0f);

    // Indicies
    m_index[0] = 0;   m_index[1] = 2;   m_index[2] = 1;
    m_index[3] = 0;   m_index[4] = 3;   m_index[5] = 2;

    m_vtxStride = sizeof(VertexFormat);

    // Generate VBOs
    glGenBuffers(2, m_vboIds);

    // Vertex Array
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, m_vtxStride * 4, m_vertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Array Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 6, m_index, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ValidationBar::Draw(CCamera* camera) {

    //*********************************
    //          OpenGL Bit
    //*********************************
    // Reset offset for each object
    GLuint offset = 0;

    // Use the program object
    glUseProgram( m_programObject );

    // Use VBOs Vertex Data
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);

    // Load the vertex position attribute
    glVertexAttribPointer( m_positionLoc, 3, GL_FLOAT,
                            GL_FALSE, 5 * sizeof(GLfloat), (const void*)offset );
    offset += sizeof(GLfloat) * 3;

    // Load the texture coordinate attribute
    glVertexAttribPointer( m_texCoordLoc, 2, GL_FLOAT,
                            GL_FALSE, 5 * sizeof(GLfloat), (const void *)offset );

    glEnableVertexAttribArray( m_positionLoc );

    // Set color uniform
    glUniform4fv(m_colorLoc, 1, &m_barColor[m_currentColor].color[0] );
    m_currentColor++;
    if(m_currentColor > 15)
        m_currentColor = 0;

    // User VBOs Index Data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);

    // Disable depth test
    glDisable(GL_DEPTH_TEST);

    // Draw the HUD
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

}
