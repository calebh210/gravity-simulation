#ifndef SCENE_H
#define SCENE_H

#include "utils/config_parser.h"
#include "graphics/body.h"
#include "graphics/camera.h"
#include "graphics/grid.h"

// this is hardcoded for 3d rn
typedef struct Scene {

    int num_bodies;
    body_3d** bodies_array;
    Grid* grid;
    Settings* config;
    Camera* cam;
    float framerate; // current FPS of the scene

} Scene;

#endif