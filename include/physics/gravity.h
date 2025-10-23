#ifndef GRAVITY_H
#define GRAVITY_H

#include <stdbool.h>
#include "utils/utils.h"
#include "utils/structures.h"
#include "utils/constants.h"

void equation_of_motion( two_d_body *b1,  two_d_body *b2, float delta_t);
void relative_equation_of_motion( two_d_body *b1,  two_d_body *b2, float delta_t);
void rk4_equation_of_motion( two_d_body *b1, two_d_body *b2, float delta_t);
void rk4_relative_equation_of_motion( two_d_body *b1, two_d_body *b2, float delta_t);

// these should also live in a graphics file at some point
void drawCircle(two_d_vector c, float r, int num_segments);
void drawOrbits(points_list *orbit);
int render( two_d_body* body1,  two_d_body* body2, int REF_FRAME_CODE, bool DEBUG);

double scharzchild_radius(double mass);

#endif