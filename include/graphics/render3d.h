#ifndef RENDER3D_H
#define RENDER3D_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <glad.h>
//#include <unistd.h>
#include <GLFW/glfw3.h>
#include "utils/config_parser.h"
#include "utils/utils.h"
#include "utils/constants.h"
#include "graphics/body.h"

typedef struct Camera{
    vector3 pos;
    float yaw;
    float pitch;
    float speed;
    float rotSpeed;
    float speedMultiplier;
    vector3 up;
    vector3 front;
    vector3 right;
    bool tracking;
    int tracked_body;
    int num_bodies;
    svector3 tracking_vector;
} Camera;

//inits glad and glfw
GLFWwindow* init_render();

// init shader object
GLuint init_shaders();

void init_3d_bodies(body_3d* bodies_array[], int num_bodies);

// render the 3d scene
void render3d(body_3d* bodies_array[], Settings* config_settings);

#endif
