#ifndef GRAVITY3D_H
#define GRAVITY3D_H

#include "math/vector/vector3.h"
#include "utils/structures.h"

void rk4_nbody_3d(double t, double h, body_3d* bodies[], int N);

#endif