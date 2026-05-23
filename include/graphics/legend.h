#ifndef LEGEND_H
#define LEGEND_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include "math/vector/vector2.h"
#include "math/vector/vector3.h"
#include <glad.h>
#include "graphics/render3d.h"

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

void render_text(FT_Setup* ft, char text[], int text_len, vector2 position, float scale, vector3 color);

void draw_legend(FT_Setup* ft, Camera* cam, body_3d* bodies_array[], double nbFrames);

#endif