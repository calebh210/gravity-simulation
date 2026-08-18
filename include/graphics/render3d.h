#ifndef RENDER3D_H
#define RENDER3D_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <glad.h>
// #include <unistd.h>
#include <GLFW/glfw3.h>
#include "utils/utils.h"
#include "utils/constants.h"
#include "graphics/body.h"
#include "graphics/scene.h"
#include "graphics/camera.h"

// inits glad and glfw
GLFWwindow* init_render();

// init shader object
GLuint init_shaders();

void init_3d_bodies(body_3d* bodies_array[], int num_bodies);

// render the 3d scene
void render3d(Scene* scene);

#endif
