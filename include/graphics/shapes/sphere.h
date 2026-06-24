#ifndef SPHERE_H
#define SPHERE_H

#include "math/vector/vector3.h"
#include "utils/dynamic_array.h"

void drawSphere(float r, int sector_count, int stack_count, vector3_da *out_vertices, vector3_da *out_normals, vector2_da *out_uvs);

#endif