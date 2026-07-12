#ifndef GENERIC_H
#define GENERIC_H

#include "graphics/body.h"

// this "generic" body object might help
typedef struct body_t {
    enum BODY_DIMENSIONS dimensions;
    union {
        body_2d* as_2d;
        body_3d* as_3d;
    } t;

} body_t;

#endif