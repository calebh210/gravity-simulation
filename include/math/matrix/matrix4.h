#ifndef MATRIX4_H
#define MATRIX4_H

//4x4 matrix represented as a 2d array
typedef float matrix4 [4][4];

float* getIdentityMatrix();

extern const float identityMatrix4[16];

#endif