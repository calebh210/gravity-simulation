#include "math/vector/vector4.h"

// Scale up a vector vec by s
vector4 scale_vec4(vector4 vec, double s){
    return (vector4){vec.x * s, vec.y * s, vec.z * s, vec.w * s};
}

// Add two vectors and return their sum
vector4 add_vec4s(vector4 vec1, vector4 vec2){
    vector4 sum = {vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z, vec1.w + vec2.w};
    return sum;
}

//subtract two vector4s from eachother
vector4 subtract_vec4s(vector4 vec1, vector4 vec2){
    vector4 difference = {vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z, vec1.w - vec2.w};
    return difference;
}

//4d dot product
//a1b1 + a2b2 + a3b3 + a4b4
double dot_vec4s(vector4 vec1, vector4 vec2){
    return (double){(vec1.x * vec2.x) + (vec1.y * vec2.y) + (vec1.z * vec2.z) + (vec1.w * vec2.w)};
}