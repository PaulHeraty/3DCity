#include "Texture.h"

texel redTexel = texel(254, 0, 0);
texel greenTexel = texel(0, 254, 0);
texel blueTexel = texel(0, 0, 254);
texel blackTexel = texel(0, 0, 0);
texel nearBlackTexel = texel(10, 10, 10);
texel midnightBlueTexel = texel(10, 10, 50);
texel whiteTexel = texel(254, 254, 254);
texel yellowTexel = texel(254, 254, 0);

std::vector<Textures> textures;

texel::texel() {
    red = 0;
    green = 0;
    blue = 0;
}

texel::texel(GLubyte r, GLubyte g, GLubyte b) {
    red = r;
    green = g;
    blue = b;
}

Texture::Texture() {
    m_textureId = 0;
    m_frameBufferObject = 0;
    m_texSize = 0;
    m_clamp = false;
    m_percentLit = 0;
    m_windowRuns = false;
    m_furniture = false;
    m_litColor = texel(RandomVal(154)+100, RandomVal(154)+100, RandomVal(154)+100);
    m_winType = (windowType)RandomVal(6);
    m_texType = TEXTURE_NONE;
}

Texture::~Texture() {
    //glDeleteFramebuffers(1, &m_frameBufferObject);
    //glDeleteTextures(1, &m_textureId);
}

bool Texture::TextureExists(textureType t, GLuint &texId) {
    for(unsigned int i = 0; i < textures.size(); i++) {
        if(textures[i].t == t) {
            texId = textures[i].textureId;
            return true;
        }
    }
    return false;
}

GLuint Texture::CreateTexture2D(textureType t, unsigned int size) {
    m_texType = t;

    switch(m_texType) {
    case(SIMPLE_TEXTURE):
        CreateSimpleTexture2D();
        break;
    case(BLACK_TEXTURE):
        CreateBlackTexture2D();
        break;
    case(TEXTURE_LATTICE):
        CreateLattice2D();
        break;
    case(TEXTURE_BUILDING0):
        m_percentLit = 30;
        CreateBuildingTexture2D();
        break;
    case(TEXTURE_BUILDING1):
        m_percentLit = 30;
        m_furniture = true;
        CreateBuildingTexture2D();
        break;
    case(TEXTURE_BUILDING2):
        m_percentLit = 10;
        m_furniture = true;
        m_windowRuns = true;
        CreateBuildingTexture2D();
        break;
    case(TEXTURE_BUILDING3):
        m_percentLit = 15;
        m_furniture = true;
        m_windowRuns = true;
        CreateBuildingTexture2D();
        break;
    case(TEXTURE_BUILDING4):
        m_percentLit = 10;
        m_furniture = false;
        m_windowRuns = true;
        CreateBuildingTexture2D();
        break;
    case(TEXTURE_BUILDING5):
        m_percentLit = 15;
        m_furniture = false;
        m_windowRuns = true;
        CreateBuildingTexture2D();
        break;
    case(TEXTURE_BUILDING6):
        m_percentLit = 20;
        m_furniture = true;
        m_windowRuns = true;
        CreateBuildingTexture2D();
        break;
    case(TEXTURE_BUILDING7):
        m_percentLit = 20;
        m_furniture = true;
        m_windowRuns = false;
        CreateBuildingTexture2D();
        break;
    case(TEXTURE_BUILDING8):
        m_percentLit = 15;
        m_furniture = false;
        m_windowRuns = false;
        CreateBuildingTexture2D();
        break;
    case(TEXTURE_BUILDING9):
        m_percentLit = 10;
        m_furniture = true;
        m_windowRuns = true;
        CreateBuildingTexture2D();
        break;
    case(TEXTURE_LIGHT):
        CreateLightTexture2D(size);
        break;
    case(TEXTURE_STREETLIGHT):
        CreateStreetlightTexture2D();
        break;
    case(TEXTURE_CAR):
        CreateCarTexture2D();
        break;
    case(TEXTURE_CARS):
        CreateCarsTexture2D();
        break;
    case(TEXTURE_SIGN0):
    case(TEXTURE_SIGN1):
    case(TEXTURE_SIGN2):
    case(TEXTURE_SIGN3):
    case(TEXTURE_SIGN4):
    case(TEXTURE_SIGN5):
    case(TEXTURE_SIGN6):
    case(TEXTURE_SIGN7):
    case(TEXTURE_SIGN8):
    case(TEXTURE_SIGN9):
        CreateSignTexture2D(m_texType);
        break;
    case(TEXTURE_ROAD):
        CreateRoadTexture2D();
        break;
    case(TEXTURE_HUD):
        CreateHUDTexture2D();
        break;
    }

    return 1;
}


GLuint Texture::CreateHUDTexture2D() {
    m_texSize = 0; // 2x2 texture

    // Texture generated by HUD::Create

   return 1;
}

GLuint Texture::CreateSignTexture2D(textureType t) {
    //GLuint texObj;
    unsigned int nameNum, prefixNum, suffixNum;

    // Create random sign text
    char signText[50];

    // Set the texture type
    m_texType = t;

    // Use tightly packed data
    glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

    // Generate a texture object
    glGenTextures ( 1, &m_textureId );

    // Bind the texture object
    glBindTexture ( GL_TEXTURE_2D, m_textureId );

    // Set the filtering mode (Trilinear)
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // Create cairo surface & context
    m_cairoSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 512, 512);
    m_cairoContext = cairo_create(m_cairoSurface);

    // Clear background of texture to nearBlack to match building textures
    cairo_set_source_rgb(m_cairoContext, 10.0/256.0, 10.0/256.0, 10.0/256.0 );
    cairo_move_to(m_cairoContext, 0.0, 0.0);
    cairo_paint_with_alpha(m_cairoContext, 1.0);

    // 8 signs (512x64) will fit in a 512x512 texture
    for(int i=0; i<8; i++) {
        nameNum = RandomVal (NAME_COUNT);
        prefixNum = RandomVal (PREFIX_COUNT);
        suffixNum = RandomVal (SUFFIX_COUNT);
        // Prefix or suffix - not both
        if(COIN_FLIP) {
            sprintf(signText, "%s%s", signPrefix[prefixNum], signName[nameNum]);
        } else {
            sprintf(signText, "%s%s", signName[nameNum], signSuffix[suffixNum]);
        }

        // Add text
        cairo_select_font_face(m_cairoContext,
                               "serif",                 // Font type
                               (_cairo_font_slant) RandomVal(3),        // Font_slant
                               (_cairo_font_weight) RandomVal(2));      // Font_weight
        cairo_set_font_size(m_cairoContext, 32.0);
        //cairo_set_source_rgb(m_cairoContext, 1.0/(RandomVal(5)+1.0), 1.0/(RandomVal(5)+1.0), 1.0/(RandomVal(5)+1.0));
        cairo_set_source_rgb(m_cairoContext, (RandomVal(192)+64.0)/255.0, (RandomVal(192)+64.0)/255.0, (RandomVal(192)+64.0)/255.0);
        cairo_move_to(m_cairoContext, 10.0, (i*64.0) + 50.0);
        cairo_show_text(m_cairoContext, signText);
    }

    // update m_surfData (image data for OpenGL texture)
    cairo_surface_flush(m_cairoSurface);
    m_surfData = cairo_image_surface_get_data(m_cairoSurface);


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_surfData);

    // Gen Mip Map
    glGenerateMipmap(GL_TEXTURE_2D);

    // Dump output
    //cairo_surface_write_to_png(m_cairoSurface, "./sign.png");

	return 1;
}

GLuint Texture::CreateStreetlightTexture2D() {
    m_texSize = 4; // 2x2 texture
    GLuint texObj;

    // Check if texture exists
    if(TextureExists(TEXTURE_STREETLIGHT, texObj)) {
        m_textureId = texObj;
    } else {        // Create new texture
        // 4x1 Image, 4 bytes per pixel (R, G, B, A)
        GLubyte pixels[4 * 4] =   {
            0,   0,   0, 0, // Black
            200, 200,   200, 200, // White-ish
            0,   0, 0, 0, // Black
            0, 0,   0,  0 // Black
        };

        // Use tightly packed data
        glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

        // Generate a texture object
        glGenTextures ( 1, &m_textureId );

        // Bind the texture object
        glBindTexture ( GL_TEXTURE_2D, m_textureId );

        // Load the texture
        glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGBA, 1, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

        // Set the filtering mode
        glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

        // Gen Mip Map
        glGenerateMipmap(GL_TEXTURE_2D);

        // Add this texture to global list
        Textures newTex;
        newTex.t = m_texType;
        newTex.textureId = m_textureId;
        textures.push_back(newTex);
    }

   return 1;
}

GLuint Texture::CreateRoadTexture2D() {
    GLuint texObj;

    m_texType = TEXTURE_ROAD;

    if(TextureExists(TEXTURE_ROAD, texObj)) {
        m_textureId = texObj;
    } else {
        m_textureId = LoadTexture2D("images/road.tga", 144);
        // Add this texture to global list
        Textures newTex;
        newTex.t = TEXTURE_ROAD;
        newTex.textureId = m_textureId;
        textures.push_back(newTex);
    }

	return 1;
}

GLuint Texture::CreateSimpleTexture2D() {
    m_texSize = 4; // 2x2 texture
    GLuint texObj;

    // Check if texture exists
    if(TextureExists(SIMPLE_TEXTURE, texObj)) {
        m_textureId = texObj;
    } else {        // Create new texture
        // 2x2 Image, 3 bytes per pixel (R, G, B)
        GLubyte pixels[4 * 3] =   {
            255,   0,   0, // Red
            0, 255,   0, // Green
            0,   0, 255, // Blue
            255, 255,   0  // Yellow
        };

        // Use tightly packed data
        glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

        // Generate a texture object
        glGenTextures ( 1, &m_textureId );

        // Bind the texture object
        glBindTexture ( GL_TEXTURE_2D, m_textureId );

        // Load the texture
        glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels );

        // Set the filtering mode
        glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        // Gen Mip Map
        //glGenerateMipmap(GL_TEXTURE_2D);

        // Add this texture to global list
        Textures newTex;
        newTex.t = m_texType;
        newTex.textureId = m_textureId;
        textures.push_back(newTex);
    }

   return 1;
}


char* Texture::LoadTGA ( char *fileName, int *width, int *height ) {
    char *buffer = NULL;
    FILE *f;
    unsigned char tgaheader[12];
    unsigned char attributes[6];
    unsigned int imagesize;

    f = fopen(fileName, "rb");
    printf("LoadTGA %s\n", fileName);
    if(f == NULL) return NULL;

    if(fread(&tgaheader, sizeof(tgaheader), 1, f) == 0)
    {
        fclose(f);
        return NULL;
    }

    if(fread(attributes, sizeof(attributes), 1, f) == 0)
    {
        fclose(f);
        return 0;
    }

    *width = attributes[1] * 256 + attributes[0];
    *height = attributes[3] * 256 + attributes[2];
    imagesize = attributes[4] / 8 * *width * *height;
    buffer = (char*) malloc(imagesize);
    if (buffer == NULL)
    {
        fclose(f);
        return 0;
    }
    if(fread(buffer, 1, imagesize, f) != imagesize)
    {
        free(buffer);
        return NULL;
    }
    fclose(f);

    return buffer;
}



GLuint Texture::LoadTexture2D(char* filename, int size) {
    int width, height;

    m_texType = TEXTURE_FILE;

    // Load the TGA texture
   char *buffer = LoadTGA( filename, &width, &height );

    if ( buffer == NULL ) {
       printf( "Error loading (%s) image.\n", filename );
       return 0;
    }

    // Generate a texture object
    glGenTextures ( 1, &m_textureId );

    // Bind the texture object
    glBindTexture ( GL_TEXTURE_2D, m_textureId );

    // Load the texture
    glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer );

    // Set the filtering mode (Trilinear)
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    //glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    //glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    // Anisotropic
    GLfloat largest_supported_anisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest_supported_anisotropy);

    // Gen Mip Map
    glGenerateMipmap(GL_TEXTURE_2D);

    free(buffer);

    return m_textureId;
}


GLuint Texture::CreateBlackTexture2D() {
    m_texSize = 1; // 2x2 texture
    GLuint texObj;

    // Check if texture exists
    if(TextureExists(BLACK_TEXTURE, texObj)) {
        m_textureId = texObj;
    } else {        // Create new texture
        // 2x2 Image, 3 bytes per pixel (R, G, B)
        GLubyte pixels[1 * 3] =   {
            0,   0,   0, // Black
        };

        // Use tightly packed data
        glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

        // Generate a texture object
        glGenTextures ( 1, &m_textureId );

        // Bind the texture object
        glBindTexture ( GL_TEXTURE_2D, m_textureId );

        // Load the texture
        glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels );

        // Set the filtering mode
        glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        // Gen Mip Map
        //glGenerateMipmap(GL_TEXTURE_2D);

        // Add this texture to global list
        Textures newTex;
        newTex.t = m_texType;
        newTex.textureId = m_textureId;
        textures.push_back(newTex);
    }

   return 1;
}

void Texture::ClearTexToColor(texel* texArray, texel color) {
    for(unsigned int row = 0; row < m_texSize; row++) {
        for(unsigned int col=0; col < m_texSize; col++) {
            texArray[col + (row*m_texSize)] = color;
        }
    }
}

bool Texture::IsColorLit(texel color) {
    bool retVal = false;

    if(color.red > 100 || color.green > 100 || color.blue > 100)
        retVal = true;

    return retVal;
}

void Texture::AddHorizLine(texel* texArray, unsigned int offset, unsigned int length, texel color, bool randomNoise) {
    texel texColor = color;
    for(unsigned int i=0; i < length; i++) {
        if(randomNoise && IsColorLit(color)) {
            if(RandomVal(10) > 7) {
                texColor = texel(RandomVal(154)+100, RandomVal(154)+100, RandomVal(154)+100);
            }
        }
        texArray[offset+i] = texColor;
    }
}

void Texture::AddVertLine(texel* texArray, unsigned int offset, unsigned int length, texel color) {
    for(unsigned int i=0; i < length; i++) {
        texArray[offset + (i*m_texSize)] = color;
    }
}

texel Texture::FadeColor(texel color, unsigned int fade) {
    int newRed = color.red - fade;
    int newBlue = color.blue - fade;
    int newGreen = color.green - fade;

    if(newRed < 0.0f)
        newRed = 0.0f;
    if(newGreen < 0.0f)
        newGreen = 0.0f;
    if(newBlue < 0.0f)
        newBlue = 0.0f;

    texel newColor = texel((GLubyte)newRed, (GLubyte)newGreen, (GLubyte)newBlue);

    return newColor;
}

void Texture::AddBox(texel* texArray, unsigned int offset, unsigned int width, unsigned int height, texel color, bool fade) {
    texel winColor = color;
    // offset is top left of box

    for(unsigned int i=0; i < height; i++) {
        AddHorizLine(texArray, offset+(i*m_texSize), width, winColor, true);
        // Add a fade from top to bottom
        if(fade)
            winColor = FadeColor(winColor, 10);
    }
}

void Texture::AddFurniture(texel* texArray, unsigned int offset, unsigned int length, texel color) {
    unsigned int furnH;
    // offset is top left of window
    for(unsigned int i=0; i < length; i++) {
        if(RandomVal(2)) {
            furnH = RandomVal(3)+1;
            // Only put furniture in bottom half of window, so add three rows (m_texSize*3)
            AddVertLine(texArray, offset+i+(m_texSize*3)+((3-furnH)*m_texSize), furnH, color);
        }
    }
}

void Texture::AddLitWindow(texel* texArray, unsigned int offset) {
    texel winColor;

    winColor = m_litColor;

    switch(m_winType) {
    case(SQUARE_WINDOW):    // 6*6 window
        AddBox(texArray, offset+m_texSize+1, 6, 6, winColor, true);
        if(m_furniture)
            AddFurniture(texArray, offset+m_texSize+1, 6, blackTexel);
        break;
    case(TWO_TALL_WINDOW):
        AddBox(texArray, offset+m_texSize+1, 2, 6, winColor, true);
        AddBox(texArray, offset+m_texSize+5, 2, 6, winColor, true);
        if(m_furniture)
            AddFurniture(texArray, offset+m_texSize+1, 6, blackTexel);
        break;
    case(FOUR_PANE_WINDOW):
        AddBox(texArray, offset+m_texSize+1, 2, 2, winColor, true);
        AddBox(texArray, offset+m_texSize+5, 2, 2, winColor, true);
        AddBox(texArray, offset+m_texSize+1+(m_texSize*4), 2, 2, winColor, true);
        AddBox(texArray, offset+m_texSize+5+(m_texSize*4), 2, 2, winColor, true);
        if(m_furniture)
            AddFurniture(texArray, offset+m_texSize+1, 6, blackTexel);
        break;
    case(T_PANE_WINDOW):
        AddBox(texArray, offset+m_texSize+1, 6, 2, winColor, true);
        AddBox(texArray, offset+m_texSize+1+(m_texSize*3), 2, 3, winColor, true);
        AddBox(texArray, offset+m_texSize+5+(m_texSize*3), 2, 3, winColor, true);
        if(m_furniture)
            AddFurniture(texArray, offset+m_texSize+1, 6, blackTexel);
        break;
    case(NARROW_WINDOW):
        AddBox(texArray, offset+m_texSize+2, 4, 6, winColor, true);
        if(m_furniture)
            AddFurniture(texArray, offset+m_texSize+1, 6, blackTexel);
        break;
    case(OBLONG_WINDOW):
        AddBox(texArray, offset+m_texSize+1, 6, 2, winColor, true);
        AddBox(texArray, offset+m_texSize+1+(m_texSize*3), 6, 3, winColor, true);
        if(m_furniture)
            AddFurniture(texArray, offset+m_texSize+1, 6, blackTexel);
        break;
    }
}

void Texture::AddUnlitWindow(texel* texArray, unsigned int offset) {
    texel winColor;

    // unlit window (but still no totally black)
    winColor = texel(RandomVal(20), RandomVal(20), RandomVal(20));

    switch(m_winType) {
    case(SQUARE_WINDOW):    // 6*6 window
        AddBox(texArray, offset+m_texSize+1, 6, 6, winColor, false);
        if(m_furniture)
            AddFurniture(texArray, offset+m_texSize+1, 6, blackTexel);
    case(TWO_TALL_WINDOW):
        AddBox(texArray, offset+m_texSize+1, 2, 6, winColor, true);
        AddBox(texArray, offset+m_texSize+5, 2, 6, winColor, true);
        if(m_furniture)
            AddFurniture(texArray, offset+m_texSize+1, 6, blackTexel);
        break;
    case(FOUR_PANE_WINDOW):
        AddBox(texArray, offset+m_texSize+1, 2, 2, winColor, true);
        AddBox(texArray, offset+m_texSize+5, 2, 2, winColor, true);
        AddBox(texArray, offset+m_texSize+1+(m_texSize*4), 2, 2, winColor, true);
        AddBox(texArray, offset+m_texSize+5+(m_texSize*4), 2, 2, winColor, true);
        if(m_furniture)
            AddFurniture(texArray, offset+m_texSize+1, 6, blackTexel);
        break;
    case(T_PANE_WINDOW):
        AddBox(texArray, offset+m_texSize+1, 6, 2, winColor, true);
        AddBox(texArray, offset+m_texSize+1+(m_texSize*3), 2, 3, winColor, true);
        AddBox(texArray, offset+m_texSize+5+(m_texSize*3), 2, 3, winColor, true);
        if(m_furniture)
            AddFurniture(texArray, offset+m_texSize+1, 6, blackTexel);
        break;
    case(NARROW_WINDOW):
        AddBox(texArray, offset+m_texSize+2, 4, 6, winColor, true);
        if(m_furniture)
            AddFurniture(texArray, offset+m_texSize+1, 6, blackTexel);
        break;
    case(OBLONG_WINDOW):
        AddBox(texArray, offset+m_texSize+1, 6, 2, winColor, true);
        AddBox(texArray, offset+m_texSize+1+(m_texSize*3), 6, 3, winColor, true);
        if(m_furniture)
            AddFurniture(texArray, offset+m_texSize+1, 6, blackTexel);
        break;
    }
}

void Texture::AddWindows(texel* texArray, unsigned int winSize) {
    unsigned int offset;
    unsigned int runLength = RandomVal(9) + 2;
    bool run = false;

    // Fill texture in black first
    //ClearTexToColor(texArray, greenTexel);
    ClearTexToColor(texArray, nearBlackTexel);

    if(m_windowRuns) {
        runLength = RandomVal(9) + 2;
        for(unsigned int winRow = 8; winRow < m_texSize; winRow+=winSize) {
            run = (RandomVal(100) > (100-m_percentLit));  // % chance of starting a new run
            // Change runLength every few floors
            if(!(winRow%8)) {
                runLength = RandomVal(9) + 2;
            }
            // Draw this floor
            for(unsigned int winCol = 0; winCol < m_texSize; winCol+=winSize) {
                offset = winCol + (winRow*m_texSize);
                if(run) {
                    AddLitWindow(texArray, offset);
                    runLength--;
                    if(runLength <= 0) {
                        run = false;
                        runLength = RandomVal(9) + 2;
                    }
                } else {
                    AddUnlitWindow(texArray, offset);
                    run = (RandomVal(100) > (100-m_percentLit));
                }
            }
        }
    } else {
        // No window runs -> random windows lit
        for(unsigned int winRow = 8; winRow < m_texSize; winRow+=winSize) {
            for(unsigned int winCol = 0; winCol < m_texSize; winCol+=winSize) {
                offset = winCol + (winRow*m_texSize);
                if(RandomVal(100) > (99 - m_percentLit)) {
                    AddLitWindow(texArray, offset);
                } else {
                    AddUnlitWindow(texArray, offset);
                }
            }
        }
    }
}

void Texture::AddLattice(texel* texArray, unsigned int winSize) {
    unsigned int offset;
    // Fill in white first
    ClearTexToColor(texArray, whiteTexel);

    for(unsigned int winRow = 0; winRow < m_texSize; winRow+=winSize) {
        for(unsigned int winCol = 0; winCol < m_texSize; winCol+=winSize) {
            // Now we are dealing with an 8x8 pixel window
            offset = winCol + (winRow*m_texSize);
            //AddWindow(texArray, offset);
            AddHorizLine(texArray, offset, 8, blackTexel);
            AddHorizLine(texArray, offset+(m_texSize*7), 8, blackTexel);
            // Draw vertical window frame lines
            AddVertLine(texArray, offset, 8, blackTexel);
            AddVertLine(texArray, offset+7, 8, blackTexel);

        }
    }
}

void Texture::AddDecor(texel* texArray) {

    texel signColor[4] = {
        texel(252, 252, 88),     // yellow neon
        texel(252, 88, 231),     // pink neon
        texel(88, 252, 252),     // cyan neon
        texel(75, 255, 43)       // green neon
    };

    // Add neon Strip to top line of texture
    AddHorizLine(texArray, 512, 512, signColor[RandomVal(4)], false);

}

GLuint Texture::CreateBuildingTexture2D() {
    m_texSize = 512;
    //GLuint texObj;

    m_litColor = texel(RandomVal(154)+100, RandomVal(154)+100, RandomVal(154)+100);
    m_winType = (windowType)RandomVal(6);
    m_texType = TEXTURE_NONE;
/*
    // Check if texture exists
    if(TextureExists(m_texType, texObj)) {
        m_textureId = texObj;
    } else {        // Create new texture

*/
        // 512x512 Image, 3 bytes per pixel (R, G, B)
        //GLubyte pixels[512 * 512 * 3];
        //texel pixels[m_texSize * m_texSize];
		texel pixels[512 * 512];

        // Each window is 8x8 pixels
        AddWindows(pixels, 8);

        // Add building decorations to top of texture
        if(RandomVal(10) > 6) {
            AddDecor(pixels);
        }

        // Use tightly packed data
        glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

        // Generate a texture object
        glGenTextures ( 1, &m_textureId );

        // Bind the texture object
        glBindTexture ( GL_TEXTURE_2D, m_textureId );

        // Load the texture
        glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, m_texSize, m_texSize, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels );

        // Set the filtering mode (Trilinear)
        glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

        // Anisotropic
        GLfloat largest_supported_anisotropy;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest_supported_anisotropy);

        // Gen Mip Map
        glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
        glGenerateMipmap(GL_TEXTURE_2D);
/*
        // Add this texture to global list
        Textures newTex;
        newTex.t = m_texType;
        newTex.textureId = m_textureId;
        textures.push_back(newTex);
    }
*/
   return 1;
}

GLuint Texture::CreateLattice2D() {
    m_texSize = 512;

   // 512x512 Image, 3 bytes per pixel (R, G, B)
   //GLubyte pixels[512 * 512 * 3];
   //texel pixels[m_texSize * m_texSize];
   texel pixels[512 * 512];

   // Each window is 8x8 pixels
   AddLattice(pixels, 8);

   // Use tightly packed data
   glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

   // Generate a texture object
   glGenTextures ( 1, &m_textureId );

   // Bind the texture object
   glBindTexture ( GL_TEXTURE_2D, m_textureId );

   // Load the texture
   glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, m_texSize, m_texSize, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels );

   // Set the filtering mode
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

   // Gen Mip Map
   //glGenerateMipmap(GL_TEXTURE_2D);

   return 1;
}


// This function allows yo to issue glDraw calls to a texture
GLuint Texture::DrawTo2DTex(unsigned int size) {
    GLint maxRenderbufferSize;
    GLenum status;

    // Check to see that size can be supported
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderbufferSize);
    if(maxRenderbufferSize > size)
        m_texSize = maxRenderbufferSize;
    else
        m_texSize = size;

    // Generate the frame buffer object (FBO)
    glGenFramebuffers(1, &m_frameBufferObject);
    // Generate a texture object
    glGenTextures ( 1, &m_textureId );

    // Bind the texture object
    glBindTexture ( GL_TEXTURE_2D, m_textureId );

    //Set up the texture
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, m_texSize, m_texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    // Set the filtering modes
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (m_clamp) {
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // Bind the frame buffer object
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObject);

    // Specify the texture object as color attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureId, 0);

    // Check for framebuffer complete
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status == GL_FRAMEBUFFER_COMPLETE) {
        // All clear - now we can render to the texture!
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw into texture now


        // Render to window system provided by framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    } else {
        printf("Texture FBO not complete...\n");
        exit(0);
    }

	return 1;
}

GLuint Texture::CreateLightTexture2D(unsigned int size) {
    // Create cairo surface & context
    m_cairoSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 128, 128);
    m_cairoContext = cairo_create(m_cairoSurface);

    // Add circles
    cairo_set_line_width(m_cairoContext, 1);
    cairo_translate(m_cairoContext, 64, 64);
    cairo_set_source_rgba(m_cairoContext, 1.0, 1.0, 1.0, 0.20);
    cairo_arc(m_cairoContext, 0.0, 0.0, 50.0, 0.0, 2.0 * M_PI);
    cairo_fill(m_cairoContext);

    cairo_set_source_rgba(m_cairoContext, 1.0, 1.0, 1.0, 0.30);
    cairo_arc(m_cairoContext, 0.0, 0.0, 40.0, 0.0, 2.0 * M_PI);
    cairo_fill(m_cairoContext);

    cairo_set_source_rgba(m_cairoContext, 1.0, 1.0, 1.0, 0.50);
    cairo_arc(m_cairoContext, 0.0, 0.0, 30.0, 0.0, 2.0 * M_PI);
    cairo_fill(m_cairoContext);

    cairo_set_source_rgba(m_cairoContext, 1.0, 1.0, 1.0, 0.75);
    cairo_arc(m_cairoContext, 0.0, 0.0, 20.0, 0.0, 2.0 * M_PI);
    cairo_fill(m_cairoContext);

    cairo_set_source_rgb(m_cairoContext, 1.0, 1.0, 1.0);
    cairo_arc(m_cairoContext, 0.0, 0.0, 10.0, 0.0, 2.0 * M_PI);
    cairo_fill(m_cairoContext);

    cairo_stroke_preserve(m_cairoContext);


    // Set m_surfData (image data for OpenGL texture)
    cairo_surface_flush(m_cairoSurface);
    m_surfData = cairo_image_surface_get_data(m_cairoSurface);

    // Create texture
    glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

    // Generate a texture object
    glGenTextures ( 1, &m_textureId );

    // Bind the texture object
    glBindTexture ( GL_TEXTURE_2D, m_textureId );

    // Set the filtering mode
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // Use cairo texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_surfData);

    // write png
    cairo_surface_write_to_png(m_cairoSurface, "./light.png");

    return 1;
}

// This code forces libpng15 functionality, which Tizen BKC does not support (libpng12)
#if 0
GLuint png_texture_load(const char * file_name, int * width, int * height) {
    png_byte header[8];

    FILE *fp = fopen(file_name, "rb");
    if (fp == 0)    {
        perror(file_name);
        return 0;
    }

    // read the header
    fread(header, 1, 8, fp);

    if (png_sig_cmp(header, 0, 8)) {
        fprintf(stderr, "error: %s is not a PNG.\n", file_name);
        fclose(fp);
        return 0;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fprintf(stderr, "error: png_create_read_struct returned 0.\n");
        fclose(fp);
        return 0;
    }

    // create png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(fp);
        return 0;
    }

    // create png info struct
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(fp);
        return 0;
    }

    // the code in this if statement gets called if libpng encounters an error
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "error from libpng\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }

    // init png reading
    png_init_io(png_ptr, fp);

    // let libpng know you already read the first 8 bytes
    png_set_sig_bytes(png_ptr, 8);

    // read all the info up to the image data
    png_read_info(png_ptr, info_ptr);

    // variables to pass to get info
    int bit_depth, color_type;
    png_uint_32 temp_width, temp_height;

    // get info about png
    png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type,
        NULL, NULL, NULL);

    if (width){ 
		*width = temp_width; 
	}
    if (height){ 
		*height = temp_height; 
	}

    //printf("%s: %lux%lu %d\n", file_name, temp_width, temp_height, color_type);

    if (bit_depth != 8) {
        fprintf(stderr, "%s: Unsupported bit depth %d.  Must be 8.\n", file_name, bit_depth);
        return 0;
    }

    GLint format;
    switch(color_type) {
    case PNG_COLOR_TYPE_RGB:
        format = GL_RGB;
        break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
        format = GL_RGBA;
        break;
    default:
        fprintf(stderr, "%s: Unknown libpng color type %d.\n", file_name, color_type);
        return 0;
    }

    // Update the png info struct.
    png_read_update_info(png_ptr, info_ptr);

    // Row size in bytes.
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    // glTexImage2d requires rows to be 4-byte aligned
    rowbytes += 3 - ((rowbytes-1) % 4);

    // Allocate the image_data as a big block, to be given to opengl
    png_byte * image_data = (png_byte *)malloc(rowbytes * temp_height * sizeof(png_byte)+15);
    if (image_data == NULL) {
        fprintf(stderr, "error: could not allocate memory for PNG image data\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }

    // row_pointers is for pointing to image_data for reading the png with libpng
    png_byte ** row_pointers = (png_byte **)malloc(temp_height * sizeof(png_byte *));
    if (row_pointers == NULL) {
        fprintf(stderr, "error: could not allocate memory for PNG row pointers\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        free(image_data);
        fclose(fp);
        return 0;
    }

    // set the individual row_pointers to point at the correct offsets of image_data
    for (unsigned int i = 0; i < temp_height; i++) {
        row_pointers[temp_height - 1 - i] = image_data + i * rowbytes;
    }

    // read the png into image_data through row_pointers
    png_read_image(png_ptr, row_pointers);

    // Generate the OpenGL texture object
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, temp_width, temp_height, 0, format, GL_UNSIGNED_BYTE, image_data);
   // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // clean up
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    free(image_data);
    free(row_pointers);
    fclose(fp);
    return texture;
}
#endif

GLuint Texture::CreateCarTexture2D() {
#if 1
    // Create cairo surface & context
    m_cairoSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 512, 512);
    m_cairoContext = cairo_create(m_cairoSurface);

    // Read png file
    cairo_surface_t *ctex;
    ctex = cairo_image_surface_create_from_png("images/audi.png");  // 512x512
	int thisW = cairo_image_surface_get_width(ctex);
	if(cairo_surface_status(ctex) == CAIRO_STATUS_NO_MEMORY) {
		printf("Error: CAIRO_STATUS_NO_MEMORY\n");
	}
	if(cairo_surface_status(ctex) == CAIRO_STATUS_FILE_NOT_FOUND) {
		printf("Error: CAIRO_STATUS_FILE_NOT_FOUND\n");
	}
	if(cairo_surface_status(ctex) == CAIRO_STATUS_READ_ERROR) {
		printf("Error: CAIRO_STATUS_READ_ERROR\n");
	}
    cairo_set_source_surface(m_cairoContext, ctex, 0, 0);
    cairo_move_to(m_cairoContext, 100, 0);
    cairo_paint(m_cairoContext);

    // Set m_surfData (image data for OpenGL texture)
    cairo_surface_flush(m_cairoSurface);
    m_surfData = cairo_image_surface_get_data(m_cairoSurface);

    // Create texture
    glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

    // Generate a texture object
    glGenTextures ( 1, &m_textureId );

    // Bind the texture object
    glBindTexture ( GL_TEXTURE_2D, m_textureId );

    // Set the filtering mode (Trilinear)
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Anisotropic
    GLfloat largest_supported_anisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest_supported_anisotropy);

    // Use cairo texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_surfData);

#else // The call to png_texture_load forces libpng15, and Tizen only support libpng12 currently
	int imageW;
	int imageH;
	m_textureId = png_texture_load("images/audi.png", &imageW, &imageH);
#endif

    // Gen Mip Map
    glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
    glGenerateMipmap(GL_TEXTURE_2D);

    // write png
    //cairo_surface_write_to_png(m_cairoSurface, "./car.png");

    return 1;
}

GLuint Texture::CreateCarsTexture2D() {
    // Create cairo surface & context
    m_cairoSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 512, 512);
    m_cairoContext = cairo_create(m_cairoSurface);

    // Read png file
    cairo_surface_t *ctex;
    ctex = cairo_image_surface_create_from_png("images/cars.png");  // 512x512
    int thisW = cairo_image_surface_get_width(ctex);
    if(cairo_surface_status(ctex) == CAIRO_STATUS_NO_MEMORY) {
        printf("Error: CAIRO_STATUS_NO_MEMORY\n");
    }
    if(cairo_surface_status(ctex) == CAIRO_STATUS_FILE_NOT_FOUND) {
        printf("Error: CAIRO_STATUS_FILE_NOT_FOUND\n");
    }
    if(cairo_surface_status(ctex) == CAIRO_STATUS_READ_ERROR) {
        printf("Error: CAIRO_STATUS_READ_ERROR\n");
    }
    cairo_set_source_surface(m_cairoContext, ctex, 0, 0);
    cairo_move_to(m_cairoContext, 100, 0);
    cairo_paint(m_cairoContext);

    // Set m_surfData (image data for OpenGL texture)
    cairo_surface_flush(m_cairoSurface);
    m_surfData = cairo_image_surface_get_data(m_cairoSurface);

    // Create texture
    glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

    // Generate a texture object
    glGenTextures ( 1, &m_textureId );

    // Bind the texture object
    glBindTexture ( GL_TEXTURE_2D, m_textureId );

    // Set the filtering mode (Trilinear)
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Anisotropic
    GLfloat largest_supported_anisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest_supported_anisotropy);

    // Use cairo texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_surfData);

    // Gen Mip Map
    glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
    glGenerateMipmap(GL_TEXTURE_2D);

    // write png
    //cairo_surface_write_to_png(m_cairoSurface, "./cars.png");

    return 1;
}

