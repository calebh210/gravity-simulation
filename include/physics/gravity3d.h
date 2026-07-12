#ifndef GRAVITY3D_H
#define GRAVITY3D_H

#include "math/vector/vector3.h"
#include "graphics/body.h"

void rk4_nbody_3d(double t, double h, body_3d* bodies[], int N);
void cog_ref_runge_kutta_3d(double t, double h, body_3d* body1, body_3d* body2);

#endif