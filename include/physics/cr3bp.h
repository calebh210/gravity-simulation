#ifndef CR3BP_H
#define CR3BP_H

#include "utils/utils.h"
#include "utils/structures.h"
#include "utils/constants.h"

double angular_velocity(body_2d *b1, body_2d *b2);

double solve_cr3bp(body_2d *b1, body_2d *b2, body_2d *t, double h);

#endif