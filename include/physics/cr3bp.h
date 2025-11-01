#ifndef CR3BP_H
#define CR3BP_H

#include "utils/utils.h"
#include "utils/structures.h"
#include "utils/constants.h"

double angular_velocity(two_d_body *b1, two_d_body *b2);

double solve_cr3bp(two_d_body *b1, two_d_body *b2, two_d_body *t, double h);

#endif