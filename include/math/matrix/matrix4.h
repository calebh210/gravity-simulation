#ifndef MATRIX4_H
#define MATRIX4_H

// 4x4 matrix represented as a 2d array
// This is technically row-major, but in memory it is stored column-major (so it works for OpenGL)
// typedef float matrix4 [4][4];

// Switching open to representing matrix4 as a float[16] for simplicity

typedef float matrix4[16];

extern const float identityMatrix4[16];

#endif