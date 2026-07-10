#ifndef GRID_H
#define GRID_H

#include <glad.h>
#include <GLFW/glfw3.h>
#include "math/matrix/matrix4.h"

typedef struct Grid{
    GLuint vbo;
    GLuint vao;
    GLuint shaders;
} Grid;

void init_grid(Grid *g);
void draw_grid(Grid *g, matrix4 view, matrix4 projection);

#endif