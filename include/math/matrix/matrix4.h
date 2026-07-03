#ifndef MATRIX4_H
#define MATRIX4_H

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

// copy from src to dst
void matrix4_copy(matrix4 src, matrix4 dst);

// matrix4* matrix4_position_translation(matrix4* model, vector3 position);
void matrix4_position_translation(matrix4 model, vector3 position);

#endif