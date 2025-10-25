#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "math/vector/vector2.h"
#include "math/vector/vector3.h"

typedef struct 
{
    double mass; // in kg
    double radius; //in m
    vector2 pos;
    vector2 velocity;
} two_d_body;

typedef struct 
{
    double mass; // in kg
    double radius; //in m
    vector3 pos;
    vector3 velocity;
} body_3d;

#endif