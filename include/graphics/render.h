#ifndef RENDER_H
#define RENDER_H

#include "utils/utils.h"
#include "utils/structures.h"
#include "utils/constants.h"

double* drawCircle(vector2 c, float r, int num_segments);
int render(body_2d* bodies_array[], enum REFERENCE_FRAME, float timeskip, int num_bodies, bool debug);

#endif