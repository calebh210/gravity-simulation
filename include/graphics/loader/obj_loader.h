#ifndef LOADER_H
#define LOADER_H

#include "utils/dynamic_array.h"

void load_obj(const char *filename, vector3_da *out_vertices, vector2_da *out_uvs, vector3_da *out_normals);

#endif