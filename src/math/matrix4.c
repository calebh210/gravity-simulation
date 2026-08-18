#include <math.h>
#include "math/matrix/matrix4.h"
#include <stdlib.h>

// Heavily inspired by https://github.com/recp/cglm/blob/master/include/cglm/mat4.h

void matrix4_copy(matrix4 src, matrix4 dst) {

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

void matrix4_init_identity(matrix4 m) {
    matrix4 temp = MATRIX4_IDENTITY_MATRIX_INIT;

    matrix4_copy(temp, m);
}

void matrix4_init_empty(matrix4 m) {
    matrix4 temp = MATRIX4_EMPTY_MATRIX_INIT;

    matrix4_copy(temp, m);
}

// takes a float[16] and fills in the matrix4 from it
void matrix4_init_from_float(float* in, matrix4 out) {

    out[0][0] = in[0];
    out[0][1] = in[1];
    out[0][2] = in[2];
    out[0][3] = in[3];
    out[1][0] = in[4];
    out[1][1] = in[5];
    out[1][2] = in[6];
    out[1][3] = in[7];
    out[2][0] = in[8];
    out[2][1] = in[9];
    out[2][2] = in[10];
    out[2][3] = in[11];
    out[3][0] = in[12];
    out[3][1] = in[13];
    out[3][2] = in[14];
    out[3][3] = in[15];
}

// multiply two matrix4's together.
void matrix4_by_matrix4(matrix4 m1, matrix4 m2, matrix4 out) {

    float a00 = m1[0][0], a01 = m1[0][1], a02 = m1[0][2], a03 = m1[0][3],
          a10 = m1[1][0], a11 = m1[1][1], a12 = m1[1][2], a13 = m1[1][3],
          a20 = m1[2][0], a21 = m1[2][1], a22 = m1[2][2], a23 = m1[2][3],
          a30 = m1[3][0], a31 = m1[3][1], a32 = m1[3][2], a33 = m1[3][3],

          b00 = m2[0][0], b01 = m2[0][1], b02 = m2[0][2], b03 = m2[0][3],
          b10 = m2[1][0], b11 = m2[1][1], b12 = m2[1][2], b13 = m2[1][3],
          b20 = m2[2][0], b21 = m2[2][1], b22 = m2[2][2], b23 = m2[2][3],
          b30 = m2[3][0], b31 = m2[3][1], b32 = m2[3][2], b33 = m2[3][3];

    out[0][0] = a00 * b00 + a10 * b01 + a20 * b02 + a30 * b03;
    out[0][1] = a01 * b00 + a11 * b01 + a21 * b02 + a31 * b03;
    out[0][2] = a02 * b00 + a12 * b01 + a22 * b02 + a32 * b03;
    out[0][3] = a03 * b00 + a13 * b01 + a23 * b02 + a33 * b03;
    out[1][0] = a00 * b10 + a10 * b11 + a20 * b12 + a30 * b13;
    out[1][1] = a01 * b10 + a11 * b11 + a21 * b12 + a31 * b13;
    out[1][2] = a02 * b10 + a12 * b11 + a22 * b12 + a32 * b13;
    out[1][3] = a03 * b10 + a13 * b11 + a23 * b12 + a33 * b13;
    out[2][0] = a00 * b20 + a10 * b21 + a20 * b22 + a30 * b23;
    out[2][1] = a01 * b20 + a11 * b21 + a21 * b22 + a31 * b23;
    out[2][2] = a02 * b20 + a12 * b21 + a22 * b22 + a32 * b23;
    out[2][3] = a03 * b20 + a13 * b21 + a23 * b22 + a33 * b23;
    out[3][0] = a00 * b30 + a10 * b31 + a20 * b32 + a30 * b33;
    out[3][1] = a01 * b30 + a11 * b31 + a21 * b32 + a31 * b33;
    out[3][2] = a02 * b30 + a12 * b31 + a22 * b32 + a32 * b33;
    out[3][3] = a03 * b30 + a13 * b31 + a23 * b32 + a33 * b33;
}

void matrix4_position_translation(matrix4 model, vector3 position) {

    model[3][0] = position.x;
    model[3][1] = position.y;
    model[3][2] = position.z;
    model[3][3] = 1.0f;
}

void matrix4_rotation_transformation(vector3 rot_speed, matrix4 rot_matrix) {

    matrix4 rotZ_matrix;
    matrix4_init_identity(rotZ_matrix);
    rotZ_matrix[0][0] = cos(rot_speed.z);
    rotZ_matrix[0][1] = -sin(rot_speed.z);
    rotZ_matrix[1][0] = sin(rot_speed.z);
    rotZ_matrix[1][1] = cos(rot_speed.z);

    matrix4 rotY_matrix;
    matrix4_init_identity(rotY_matrix);

    rotY_matrix[0][0] = cos(rot_speed.y);
    rotY_matrix[0][2] = -sin(rot_speed.y);
    rotY_matrix[2][0] = sin(rot_speed.y);
    rotY_matrix[2][2] = cos(rot_speed.y);

    // x-axis
    matrix4 rotX_matrix;
    matrix4_init_identity(rotX_matrix);
    rotX_matrix[1][1] = cos(rot_speed.x);
    rotX_matrix[1][2] = -sin(rot_speed.x);
    rotX_matrix[2][1] = sin(rot_speed.x);
    rotX_matrix[2][2] = cos(rot_speed.x);

    matrix4_by_matrix4(rotX_matrix, rotY_matrix, rot_matrix);
    matrix4_by_matrix4(rotZ_matrix, rot_matrix, rot_matrix);
}