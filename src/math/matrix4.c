#include <math.h>
#include "math/vector/vector3.h"
#include "math/matrix/matrix4.h"

const float identityMatrix4[16] = {
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0,

};


void matrix4_position_transformation(matrix4* model, vector3 position){

    model[3][0] = position.x;
    model[3][1] = position.y;
    model[3][1] = position.z;

}