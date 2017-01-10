#ifndef TEXTURE_H
#define TEXTURE_H

#include "Common.h"
#include "png.h"

// Define OpenGL functions
//#if defined(_WIN32) || defined(WIN32)
//#include "Windows/OGLDefs.h"
//#endif

#include "glTypes.h"
#include "Random.h"

#define M_PI 3.14159265358979323846

enum textureType {
    TEXTURE_BUILDING0,
    TEXTURE_BUILDING1,
    TEXTURE_BUILDING2,
    TEXTURE_BUILDING3,
    TEXTURE_BUILDING4,
    TEXTURE_BUILDING5,
    TEXTURE_BUILDING6,
    TEXTURE_BUILDING7,
    TEXTURE_BUILDING8,
    TEXTURE_BUILDING9,
    SIMPLE_TEXTURE,
    BLACK_TEXTURE,
    TEXTURE_LATTICE,
    TEXTURE_FILE,
    TEXTURE_LIGHT,
    TEXTURE_HUD,
    TEXTURE_NONE,
    TEXTURE_CAR,
    TEXTURE_CARS,
    TEXTURE_ROAD,
    TEXTURE_SIGN0,
    TEXTURE_SIGN1,
    TEXTURE_SIGN2,
    TEXTURE_SIGN3,
    TEXTURE_SIGN4,
    TEXTURE_SIGN5,
    TEXTURE_SIGN6,
    TEXTURE_SIGN7,
    TEXTURE_SIGN8,
    TEXTURE_SIGN9,
    TEXTURE_STREETLIGHT,
    MAX_TEXTURE_TYPES
};

enum windowType {
    SQUARE_WINDOW,
    TWO_TALL_WINDOW,
    FOUR_PANE_WINDOW,
    T_PANE_WINDOW,
    NARROW_WINDOW,
    OBLONG_WINDOW
};

class texel {
public:
    texel();
    texel(GLubyte r, GLubyte g, GLubyte b);
    GLubyte red;
    GLubyte green;
    GLubyte blue;
};

struct Textures {
    textureType t;
    GLuint textureId;
};

static char* signPrefix[] =
{
  "i",
  "Green ",
  "Mega",
  "Super ",
  "Omni",
  "e",
  "Hyper",
  "Global ",
  "Vital",
  "Next ",
  "Pacific ",
  "Metro",
  "Unity ",
  "G-",
  "Trans",
  "Infinity ",
  "Superior ",
  "Monolith ",
  "Best ",
  "Atlantic ",
  "First ",
  "Union ",
  "National ",
};

static char* signName[] =
{
  "Biotic",
  "Info",
  "Data",
  "Solar",
  "Aerospace",
  "Motors",
  "Nano",
  "Online",
  "Circuits",
  "Energy",
  "Med",
  "Robotic",
  "Exports",
  "Security",
  "Systems",
  "Financial",
  "Industrial",
  "Media",
  "Materials",
  "Foods",
  "Networks",
  "Shipping",
  "Tools",
  "Medical",
  "Publishing",
  "Enterprises",
  "Audio",
  "Health",
  "Bank",
  "Imports",
  "Apparel",
  "Petroleum",
  "Studios",
};

static char* signSuffix[] =
{
  "Corp",
  " Inc.",
  "Co",
  "World",
  ".Com",
  " USA",
  " Ltd.",
  "Net",
  " Tech",
  " Labs",
  " Mfg.",
  " UK",
  " Unlimited",
  " One",
  " LLC"
};

#define PREFIX_COUNT        (sizeof (signPrefix) / sizeof (char*))
#define SUFFIX_COUNT        (sizeof (signSuffix) / sizeof (char*))
#define NAME_COUNT          (sizeof (signName) / sizeof (char*))

class Texture {
public:
    Texture();
    ~Texture();

    GLuint CreateTexture2D(textureType t, unsigned int size = 4);
    GLuint LoadTexture2D(char *filename, int size);
//private:
    bool TextureExists(textureType t, GLuint &texId);
    char* LoadTGA ( char *fileName, int *width, int *height );
    GLuint CreateSimpleTexture2D();
    GLuint CreateHUDTexture2D();
    GLuint CreateBlackTexture2D();
    GLuint CreateStreetlightTexture2D();
    GLuint CreateLightTexture2D(unsigned int size);
    GLuint CreateCarTexture2D();
    GLuint CreateCarsTexture2D();
    GLuint CreateRoadTexture2D();
    GLuint CreateSignTexture2D(textureType t);
    GLuint CreateLattice2D();
    GLuint CreateBuildingTexture2D();
    GLuint DrawTo2DTex(unsigned int size);
    void ClearTexToColor(texel* texArray, texel color);
    void AddLattice(texel* texArray, unsigned int winSize);
    void AddWindows(texel* start, unsigned int winSize);
    void AddHorizLine(texel* start, unsigned int offset, unsigned int length, texel color, bool randomNoise = false);
    void AddVertLine(texel* texArray, unsigned int offset, unsigned int length, texel color);
    void AddBox(texel* texArray, unsigned int offset, unsigned int width, unsigned int height, texel color, bool fade);
    void AddFurniture(texel* texArray, unsigned int offset, unsigned int length, texel color);
    texel FadeColor(texel color, unsigned int fade);
    bool IsColorLit(texel color);
    void AddLitWindow(texel* texArray, unsigned int offset);
    void AddUnlitWindow(texel* texArray, unsigned int offset);
    void AddDecor(texel* texArray);

    GLuint m_textureId;
    GLuint m_frameBufferObject;
    unsigned int m_texSize;
    bool m_clamp;
    textureType m_texType;
    unsigned int m_percentLit;
    bool m_furniture;
    bool m_windowRuns;
    texel m_litColor;
    windowType m_winType;

    // Cairo
    cairo_surface_t  *m_cairoSurface;
    cairo_t          *m_cairoContext;
    unsigned char    *m_surfData;

};

#endif // TEXTURE_H
