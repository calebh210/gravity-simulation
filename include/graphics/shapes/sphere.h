#ifndef SPHERE_H
#define SPHERE_H

#include "math/vector/vector3.h"
#include "utils/dynamic_array.h"

void drawSphere(vector3 s, float r, int NUM_SEGMENTS, vector3_da *out_vertices, vector3_da *out_normals);

#endif