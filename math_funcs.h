#ifndef MATH_FUNC_H
#define MATH_FUNC_H

#include "structures.h"
#include "constants.h"

two_d_vector* find_cog(double m1,  two_d_vector pos1, double m2,  two_d_vector pos2);

// normalize values to something that opengl can render
float normalize(double value, double min, double max);

// normalize a two_d_vectpr to something that opengl can render
two_d_vector normalize_vec2(two_d_vector vec, double min, double max);

// Scale up a vector vec by s
two_d_vector scale_vec2(two_d_vector vec, double s);

// Add two vectors and return their sum
two_d_vector add_vecs(two_d_vector vec1, two_d_vector vec2);

//Add 4 vecs (used for RK4)
two_d_vector add_vecs4(two_d_vector vec1, two_d_vector vec2, two_d_vector vec3, two_d_vector vec4);

//subtract two vec2s from eachother
two_d_vector subtract_vec2s(two_d_vector vec1, two_d_vector vec2);

//RK4 Implementations
void cog_ref_runge_kutta(double t, double h, two_d_body *body1, two_d_body *body2);
void runge_kutta(double t, double h, double m, two_d_body *b);
void coint_runge_kutta(double t, double h, two_d_body *body1, two_d_body *body2);

#endif