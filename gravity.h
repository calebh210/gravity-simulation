#ifndef GRAVITY_H
#define GRAVITY_H

#include <stdbool.h>
#include "structures.h"
#include "constants.h"

two_d_vector* equation_of_motion( two_d_body *b1,  two_d_body *b2, float delta_t);
two_d_vector* relative_equation_of_motion( two_d_body *b1,  two_d_body *b2, float delta_t);
two_d_vector* rk4_equation_of_motion( two_d_body *b1, two_d_body *b2, float delta_t);
two_d_vector* rk4_relative_equation_of_motion( two_d_body *b1, two_d_body *b2, float delta_t );


// This list helpers should probably live somewhere else. IDK where yet
points_list* init_list();
// add new point P to head of list
void add_to_list(points_list *pL, point *p);
void free_list(points_list *pL);

// these should also live in a graphics file at some point
void drawCircle(two_d_vector c, float r, int num_segments);
void drawOrbits(points_list *orbit);
int render( two_d_body* body1,  two_d_body* body2, bool DEBUG);

double scharzchild_radius(double mass);

#endif