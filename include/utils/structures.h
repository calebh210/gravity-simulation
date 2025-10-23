#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef struct 
{
    double x; 
    double y;
} two_d_vector;

typedef struct 
{
    double mass; // in kg
    double radius; //in m
    two_d_vector pos;
    two_d_vector velocity;
} two_d_body;

#endif