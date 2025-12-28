#ifndef UTILS_H
#define UTILS_H

#include <glad.h>
#include "math/vector/vector3.h"

typedef struct point{
    vector3 pos;
    struct point *next;
} point;

typedef struct points_list{
    point *head;
    GLuint vao;
    GLuint vbo;
    int count;
} points_list;

points_list* init_list();
void add_to_list(points_list *pL, point *p);
void add_to_list_head(points_list *pL, point *p);
void free_list(points_list *pL);

#endif