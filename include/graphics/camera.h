#ifndef CAMERA_H
#define CAMERA_H

#include "math/vector/vector3.h"

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
    svector3 tracking_vector;
} Camera;

#endif