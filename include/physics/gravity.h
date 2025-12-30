#ifndef GRAVITY_H
#define GRAVITY_H

#include <stdbool.h>
#include "utils/utils.h"
#include "utils/structures.h"
#include "utils/constants.h"

void equation_of_motion( body_2d *b1,  body_2d *b2, float delta_t);
void relative_equation_of_motion( body_2d *b1,  body_2d *b2, float delta_t);
void rk4_equation_of_motion( body_2d *b1, body_2d *b2, float delta_t);
void rk4_relative_equation_of_motion( body_2d *b1, body_2d *b2, float delta_t);

// these should also live in a graphics file at some point
double* drawCircle(vector2 c, float r, int num_segments);
void initOrbit(points_list* orbit);
void drawOrbits(points_list* orbits_list, int N);
void updateOrbits(points_list* orbits_list, vector3 coords);
int render(body_2d* bodies_array[], int ref_frame_code, float timeskip, int num_bodies, bool debug);

double scharzchild_radius(double mass);

#endif