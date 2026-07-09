#ifndef RENDER_H
#define RENDER_H

#include "utils/utils.h"
#include "utils/constants.h"
#include "graphics/orbits.h"
#include "math/vector/vector2.h"
#include "graphics/body.h"

double* drawCircle(vector2 c, float r, int num_segments);
int render(body_2d* bodies_array[], enum REFERENCE_FRAME, float timeskip,
           int num_bodies, bool debug);

#endif
