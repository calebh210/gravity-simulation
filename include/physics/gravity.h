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
void drawCircle(vector2 c, float r, int num_segments);
void drawOrbits(points_list* orbits_list[], int N);
void drawBodies(two_d_body* bodies_array[], points_list* orbits_list[], int N);
void updateOrbits(points_list* orbits_list, vector2 coords);
int render(two_d_body* bodies_array[], int REF_FRAME_CODE, float TIME_DELTA, int NUM_BODIES, bool DEBUG);

double scharzchild_radius(double mass);

#endif