#include <math.h>
#include "math/vector/vector3.h"
#include "math/matrix/matrix4.h"
#include <stdlib.h>


void matrix4_position_translation(matrix4 model, vector3 position){

    model[3][0] = position.x;
    model[3][1] = position.y;
    model[3][2] = position.z;
    model[3][3] = 1.0f;

}

void matrix4_copy(matrix4 src, matrix4 dst){

    dst[0][0] = src[0][0];
    dst[0][1] = src[0][1];
    dst[0][2] = src[0][2]; 
    dst[0][3] = src[0][3];
    dst[1][0] = src[1][0];
    dst[1][1] = src[1][1];
    dst[1][2] = src[1][2];
    dst[1][3] = src[1][3];
    dst[2][0] = src[2][0];
    dst[2][1] = src[2][1];
    dst[2][2] = src[2][2];
    dst[2][3] = src[2][3];
    dst[3][0] = src[3][0];
    dst[3][1] = src[3][1];
    dst[3][2] = src[3][2];
    dst[3][3] = src[3][3];

}

void matrix4_rotation_transformation(matrix4 model, vector3 rotationX, vector3 rotationY, vector3 rotationZ){

    // todo!

}

void matrix4_init_identity(matrix4 m){
    matrix4 temp = MATRIX4_IDENTITY_MATRIX_INIT;

    matrix4_copy(temp, m);
    
}

// TODO!
// multiply two matrix4's together.
void matrix4_by_matrix4(){

}
