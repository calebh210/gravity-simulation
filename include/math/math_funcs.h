#ifndef MATH_FUNC_H
#define MATH_FUNC_H

#include "utils/structures.h"
#include "utils/constants.h"

vector2 find_cog(double m1,  vector2 pos1, double m2,  vector2 pos2);

// normalize values to something that opengl can render
float normalize(double value, double min, double max);

//RK4 Implementations
void cog_ref_runge_kutta(double t, double h, two_d_body *body1, two_d_body *body2);
void runge_kutta(double t, double h, double m, two_d_body *b);
void coint_runge_kutta(double t, double h, two_d_body *body1, two_d_body *body2);

#endif