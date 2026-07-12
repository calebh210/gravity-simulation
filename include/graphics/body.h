#ifndef BODY_H
#define BODY_H

#include "utils/utils.h"
#include "graphics/orbits.h"
#include "math/vector/vector2.h"
#include "math/vector/vector3.h"

enum BODY_DIMENSIONS {
    BODY_2D,
    BODY_3D
};

enum REFERENCE_FRAME {
    INERTIAL,
    CENTER_OF_GRAVITY,
    RELATIVE,
    CR3BP,
    N_BODY
};

// to determine if it should emit light or not
enum BODY_TYPE {
    PLANET,
    STAR
};

typedef struct body_2d
{
    char* name; // name from the init.yaml
    double mass; // in kg
    double radius; //in m
    int resolution; // how many segments to use in circle
    // Each body needs to keep track of its own VBO/VAO to update its rendering
    GLuint vbo;
    GLuint vao;
    rgb color;
    vector2 pos;
    vector2 velocity;
    Orbit* orbit;
} body_2d;

typedef struct body_3d
{
    char* name; // name from the init.yaml
    bool has_model; // boolean flag for if a model is defined
    char* model; // path to a .obj model
    enum BODY_TYPE type;
    double mass; // in kg
    double radius; //in m
    int resolution; // how many faces to use in sphere
    rgb color; // Color of the object in RGB (1.0, 1.0, 1.0)
    GLuint vbo;
    GLuint vao;
    vector3 pos;
    vector3 velocity;
    vector3 rotational_period; // time in sec it takes body to rotate 1 full time
    vector3 rotation; // current orientation of the body
    Orbit* orbit;
    bool has_texture; // boolean flag for if a texture is defined
    char* texture_path; //path to a image file to use as a texturee
    unsigned int texture; //opengl tex object
} body_3d;

#endif