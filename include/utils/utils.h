#ifndef UTILS_H
#define UTILS_H

#include "utils/structures.h"

typedef struct point{
    vector2 pos;
    struct point *next;
    int count;
} point;

typedef struct{
    point *head;
} points_list;

points_list* init_list();
void add_to_list(points_list *pL, point *p);
void free_list(points_list *pL);

#endif