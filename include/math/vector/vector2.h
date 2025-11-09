#ifndef VECTOR2_H
#define VECTOR2_H

//2 Dimensional Vector
typedef struct 
{
    double x; 
    double y;
} vector2;

// normalize a two_d_vectpr to something that opengl can render
vector2 normalize_vec2(vector2 vec, double min, double max);

// Scale up a vector vec by s
vector2 scale_vec2(vector2 vec, double s);

// Add two vectors and return their sum
vector2 add_vec2s(vector2 vec1, vector2 vec2);

//Add 4 vector2s (used for RK4)
vector2 add_4vec2s(vector2 vec1, vector2 vec2, vector2 vec3, vector2 vec4);

//subtract two vec2s from eachother
vector2 subtract_vec2s(vector2 vec1, vector2 vec2);

//magnitude of a vec2
double magnitude_vec2(vector2 vec);

//Distance between two vec2s
double vec2_distance_between(vector2 v1, vector2 v2);

//Dot Product
double dot_vec2s(vector2 vec1, vector2 vec2);

#endif