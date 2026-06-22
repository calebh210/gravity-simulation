#ifndef TEXT_H 
#define TEXT_H

#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct {
    unsigned int TextureID;  // ID handle of the glyph texture
    ivector2   Size;       // Size of glyph
    ivector2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
} Character;

typedef struct {
    GLuint VAO;
    GLuint VBO;
    Character* character_array;
    GLuint shaders;
} FT_Setup;

FT_Setup* ft_setup(char* font);

#endif