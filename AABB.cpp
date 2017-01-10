#include "AABB.h"

AABB::AABB() {
}

AABB::~AABB() {
}

void AABB::CreateAABB(GLvector3 loc, GLvector3 size, bool quadTree) {
    m_vertexShader = "shaders/aabb.vp";
    m_fragmentShader = "shaders/aabb.fp";
    m_shaderType = AABB_SHADER;

    if(quadTree)
        m_isQuadTree = true;
    else
        m_isQuadTree = false;

    CreateProgram(AABB_SHADER, "shaders/aabb.vp", "shaders/aabb.fp", BLACK_TEXTURE);

    m_AABBinWS.min = glVector(loc.x - size.x, loc.y, loc.z - size.z);
    m_AABBinWS.max = glVector(loc.x + size.x, loc.y + 2*size.y, loc.z + size.z);

    // Create vertices
    m_vertex[0] = PointFormat(loc.x - size.x, loc.y, loc.z - size.z);
    m_vertex[1] = PointFormat(loc.x - size.x, loc.y, loc.z + size.z);
    m_vertex[2] = PointFormat(loc.x + size.x, loc.y, loc.z + size.z);
    m_vertex[3] = PointFormat(loc.x + size.x, loc.y, loc.z - size.z);
    m_vertex[4] = PointFormat(loc.x - size.x, loc.y + 2*size.y, loc.z - size.z);
    m_vertex[5] = PointFormat(loc.x - size.x, loc.y + 2*size.y, loc.z + size.z);
    m_vertex[6] = PointFormat(loc.x + size.x, loc.y + 2*size.y, loc.z + size.z);
    m_vertex[7] = PointFormat(loc.x + size.x, loc.y + 2*size.y, loc.z - size.z);

    // Create Indices
    m_index[0] = 0;   m_index[1] = 4;   m_index[2] = 7;
    m_index[3] = 0;   m_index[4] = 7;   m_index[5] = 3;
    m_index[6] = 3;   m_index[7] = 7;   m_index[8] = 6;
    m_index[9] = 3;   m_index[10] = 6;   m_index[11] = 2;
    m_index[12] = 2;   m_index[13] = 6;   m_index[14] = 5;
    m_index[15] = 2;   m_index[16] = 5;   m_index[17] = 1;
    m_index[18] = 1;   m_index[19] = 5;   m_index[20] = 4;
    m_index[21] = 1;   m_index[22] = 4;   m_index[23] = 0;
    m_index[24] = 4;   m_index[25] = 5;   m_index[26] = 6;
    m_index[27] = 4;   m_index[28] = 6;   m_index[29] = 7;
    m_index[30] = 0;   m_index[31] = 3;   m_index[32] = 2;
    m_index[33] = 0;   m_index[34] = 2;   m_index[35] = 1;
}

void AABB::Draw(CCamera *camera) {
    // Use the program object
    glUseProgram (m_programObject );

    GLfloat *vtxBuf = (GLfloat*) &m_vertex[0];
    m_vtxStride = sizeof(PointFormat);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(m_positionLoc);   // pos
    glEnableVertexAttribArray(m_texCoordLoc);   // uv

    glVertexAttribPointer(m_positionLoc, 3, GL_FLOAT, GL_FALSE, m_vtxStride, vtxBuf);

    glBindAttribLocation(m_programObject, 0, "a_position");
    glBindAttribLocation(m_programObject, 1, "a_texCoord");

    // Set the mvpMatrix uniform
    GLmatrix vMatrix = camera->GetViewMatrix();
    GLmatrix pMatrix = camera->GetProjMatrix();
    GLmatrix mvpMatrix = glMatrixMultiply(vMatrix, pMatrix);
    glUniformMatrix4fv(m_mvpMatrixLoc, 1, GL_FALSE,  &mvpMatrix.elements[0][0]);

    // Set color uniform
    float color[4] = {0.7f, 0.0f, 0.0f, 1.0f};
    // yellow AABB box for quadTree nodes
    if(m_isQuadTree)
        color[1] = 0.7f;
    glUniform4fv(m_colorLoc, 1, &color[0]);

    // Bind the texture
    glActiveTexture ( GL_TEXTURE0 );
    //glEnable(GL_TEXTURE_2D);   DEPRECATED
    glBindTexture ( GL_TEXTURE_2D, m_texture.m_textureId );

    // Set the sampler texture unit to 0
    glUniform1i ( m_samplerLoc, 0 );

    GLushort *ind = m_index;
    for(int i=0; i < 36; i+=3) {
        glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_SHORT, ind);
        ind += 3;
    }

}
