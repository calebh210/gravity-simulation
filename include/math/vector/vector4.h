#ifndef VECTOR4_H
#define VECTOR4_H

//3 Dimensional Vector
typedef struct 
{
    double x; 
    double y;
    double z;
    double w;
} vector4;

// normalize a 4d vector to something that opengl can render
// Is this needed?
// vector4 normalize_vec4(vector4 vec, double min, double max);

// Scale up a vector vec by s
vector4 scale_vec4(vector4 vec, double s);

// Add two vectors and return their sum
vector4 add_vec4s(vector4 vec1, vector4 vec2);

//subtract two vec2s from eachother
vector4 subtract_vec4s(vector4 vec1, vector4 vec2);

#endif