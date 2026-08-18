#ifndef GRAVITY_H
#define GRAVITY_H

#include <stdbool.h>
#include "graphics/body.h"
#include "utils/utils.h"
#include "utils/constants.h"

void equation_of_motion(body_2d* b1, body_2d* b2, float delta_t);
void relative_equation_of_motion(body_2d* b1, body_2d* b2, float delta_t);
void rk4_equation_of_motion(body_2d* b1, body_2d* b2, float delta_t);
void rk4_relative_equation_of_motion(body_2d* b1, body_2d* b2, float delta_t);
double scharzchild_radius(double mass);

#endif