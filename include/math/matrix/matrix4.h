#ifndef MATRIX4_H
#define MATRIX4_H

#include "math/vector/vector3.h"

// 4x4 matrix represented as a 2d array
// This is technically column-major (even if it looks row major) due to how memory is mapped
// For example, location [3][0] is the first element of the 4th column

// Should I define this a struct?
typedef float matrix4 [4][4];

// Inspired by CGLM
#define MATRIX4_IDENTITY_MATRIX_INIT  {{1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}}

#define MATRIX4_IDENTITY_MATRIX       ((matrix4)MATRIX4_IDENTITY_MATRIX_INIT)

// init matrix4 as an identity matrix
void matrix4_init_identity(matrix4 m);

// takes a float[16] and fills in the matrix4 from it
void matrix4_init_from_float(float* in, matrix4 out);

// copy from src to dst
void matrix4_copy(matrix4 src, matrix4 dst);

// matrix4* matrix4_position_translation(matrix4* model, vector3 position);
void matrix4_position_translation(matrix4 model, vector3 position);

// rot_speed is the step from the previous frame that the object is rotated by
void matrix4_rotation_transformation(vector3 rot_speed, matrix4 rot_matrix);

// matrix4x4 times matrix4x4
void matrix4_by_matrix4(matrix4 m1, matrix4 m2, matrix4 dest);

#endif