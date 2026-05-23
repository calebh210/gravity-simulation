#ifndef ORBITS_H
#define ORBITS_H

#include <glad.h>
#include "utils/utils.h"
#include "math/matrix/matrix4.h"

typedef struct Orbit{
    GLuint vao;
    GLuint vbo;
    GLuint* shaders;
    points_list* points;
} Orbit;

Orbit* initOrbit();
void drawOrbit(Orbit* orbits_list, GLuint shader);
void updateOrbits(Orbit* orbits_list, vector3 coords);
GLuint init_orbit_shaders();
void update_orbit_shaders(GLuint* shaders, matrix4 view);

#endif
