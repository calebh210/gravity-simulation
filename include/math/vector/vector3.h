#ifndef VECTOR3_H
#define VECTOR3_H

//3 Dimensional Vector
typedef struct 
{
    double x; 
    double y;
    double z;
} vector3;

// normalize a two_d_vectpr to something that opengl can render
vector3 normalize_vec3(vector3 vec, double min, double max);

// Scale up a vector vec by s
vector3 scale_vec3(vector3 vec, double s);

// Add two vectors and return their sum
vector3 add_vec3s(vector3 vec1, vector3 vec2);

//subtract two vec2s from eachother
vector3 subtract_vec3s(vector3 vec1, vector3 vec2);

#endif