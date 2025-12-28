#ifndef MATH_FUNC_H
#define MATH_FUNC_H

#include "utils/structures.h"
#include "utils/constants.h"

vector2 find_cog(double m1,  vector2 pos1, double m2,  vector2 pos2);

double scharzchild_radius(double mass);

double standard_gravitational_parameter(double m1, double m2);

// normalize values to something that opengl can render
float normalize(double value, double min, double max);
float denormalize(double value, double min, double max);

//RK4 Implementations
void cog_ref_runge_kutta(double t, double h, body_2d *body1, body_2d *body2);
void runge_kutta(double t, double h, double m, body_2d *b);
void coint_runge_kutta(double t, double h, body_2d *body1, body_2d *body2);
void rk4_nbody(double t, double h, body_2d* bodies[], int NUM_BODIES);

#endif