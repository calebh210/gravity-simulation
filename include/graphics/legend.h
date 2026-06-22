#ifndef LEGEND_H
#define LEGEND_H

#include "math/vector/vector2.h"
#include "math/vector/vector3.h"
#include <glad.h>
#include <graphics/scene.h>
#include <graphics/text.h>

void render_text(FT_Setup* ft, char text[], int text_len, vector2 position, float scale, vector3 color);

void draw_legend(Scene* scene);

#endif