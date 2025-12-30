#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "math/vector/vector2.h"
#include "math/vector/vector3.h"
#include "utils/utils.h"

typedef struct rgb
{
    float r;
    float g;
    float b;

} rgb;

// to determine if it should emit light or not
enum BODY_TYPE {
    PLANET,
    STAR
};

typedef struct body_2d
{
    double mass; // in kg
    double radius; //in m
    int resolution; // how many segments to use in circle
    // Each body needs to keep track of its own VBO/VAO to update its rendering
    GLuint vbo;
    GLuint vao;
    rgb color;
    vector2 pos;
    vector2 velocity;
    points_list* orbit;
} body_2d;

typedef struct body_3d
{
    enum BODY_TYPE type;
    double mass; // in kg
    double radius; //in m
    int resolution; // how many faces to use in sphere
    rgb color; // Color of the object in RGB (1.0, 1.0, 1.0)
    GLuint vbo;
    GLuint vao;
    vector3 pos;
    vector3 velocity;
} body_3d;


enum BODY_DIMENSIONS {
    BODY_2D,
    BODY_3D
};

// this "generic" body object might help
typedef struct body_t
{
    enum BODY_DIMENSIONS dimensions;
    union 
    {
        body_2d* as_2d; 
        body_3d* as_3d;
    }t;

} body_t;

#endif