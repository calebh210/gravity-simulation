#ifndef RENDER3D_H
#define RENDER3D_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <glad.h>
//#include <unistd.h>
#include <GLFW/glfw3.h>
#include "utils/utils.h"
#include "utils/structures.h"
#include "utils/constants.h"

typedef struct camera{
    vector3 pos;
} camera;

typedef struct grid{
    GLuint vbo;
    GLuint vao;
} grid;

//inits glad and glfw
GLFWwindow* init_render();

// init shader object
GLuint init_shaders();

void init_3d_bodies(body_3d* bodies_array[], int num_bodies);

// render the 3d scene
void render3d(body_3d* bodies_array[], int ref_frame_code, int timeskip, const int num_bodies, const bool debug);

#endif