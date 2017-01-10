#include "Nav.h"

Nav::Nav() {
}

Nav::~Nav() {
}

void Nav::Create(int width, int height) {
    m_width = width;
    m_height = height;

    // Create the program objects & texture
    CreateProgram(HUD_SHADER, "shaders/hud.vp", "shaders/hud.fp", TEXTURE_HUD);

    m_vertex[0] = VertexFormat(-1.0,  1.0, 0.0f, 0.0f, 0.0f);   // x,y,z,u,v
    m_vertex[1] = VertexFormat( 1.0,  1.0, 0.0f, 1.0f, 0.0f);
    m_vertex[2] = VertexFormat( 1.0, -1.0, 0.0f, 1.0f, 1.0f);
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

    // Create cairo surface & context
    m_cairoSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, m_width, m_height);
    m_cairoContext = cairo_create(m_cairoSurface);

    cairo_surface_t *cockpit;
    cockpit = cairo_image_surface_create_from_png("images/cockpit.png");  // 1920x1080
    cairo_set_source_surface(m_cairoContext, cockpit, 0, 0);
    cairo_move_to(m_cairoContext, 0,0);
    cairo_paint(m_cairoContext);

#ifdef DEBUG_HUD
// Force the color for HUD debug uv(0,0)
     cairo_set_line_width(m_cairoContext, 2.0);
     cairo_set_source_rgb(m_cairoContext, 1.0, 0.0, 0.0);
     cairo_rectangle(m_cairoContext, 0, 0,  2, 2);
     cairo_stroke(m_cairoContext);
#endif

     // update m_surfData (image data for OpenGL texture)
     cairo_surface_flush(m_cairoSurface);
     m_surfData = cairo_image_surface_get_data(m_cairoSurface);

     // Use tightly packed data
     glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

     // Generate a texture object
     glGenTextures ( 1, &m_texture.m_textureId );

     // Bind the texture object
     glBindTexture ( GL_TEXTURE_2D, m_texture.m_textureId );

     // Use cairo texture
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_surfData);

     // Set the filtering mode
     glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
     glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

     //cairo_surface_write_to_png(m_cairoSurface, "./cockpit.png");
}

void Nav::Draw(CCamera* camera) {

    //*********************************
    //          OpenGL Bit
    //*********************************
    // Reset offset for each object
    GLuint offset = 0;

    // Use the program object
    glUseProgram ( m_programObject );

    // Use VBOs Vertex Data
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);

    // Load the vertex position attribute
    glVertexAttribPointer ( m_positionLoc, 3, GL_FLOAT,
                            GL_FALSE, 5 * sizeof(GLfloat), (const void*)offset );
    offset += sizeof(GLfloat) * 3;

    // Load the texture coordinate attribute
    glVertexAttribPointer ( m_texCoordLoc, 2, GL_FLOAT,
                            GL_FALSE, 5 * sizeof(GLfloat), (const void *)offset );

    glEnableVertexAttribArray ( m_positionLoc );
    glEnableVertexAttribArray ( m_texCoordLoc );

    // User VBOs Index Data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);

    // Bind the texture object
    glActiveTexture ( GL_TEXTURE0 );
    // glEnable(GL_TEXTURE_2D); DEPRECATED
    glBindTexture ( GL_TEXTURE_2D, m_texture.m_textureId );

    // Blend particles
    glEnable ( GL_BLEND );
    glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    // Set the sampler texture unit to 0
    glUniform1i ( m_samplerLoc, 0 );

    // Draw the HUD
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

    glDisable(GL_BLEND );
}



