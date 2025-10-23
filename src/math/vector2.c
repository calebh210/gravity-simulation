#include "math/vector/vector2.h"

// normalize a vector2 to something that opengl can render
vector2 normalize_vec2(vector2 vec, double min, double max) {
    vector2 n_vec = {(2.0 * (vec.x - min) / (max - min) - 1.0), (2.0 * (vec.y - min) / (max - min) - 1.0)};
    return n_vec;
}

// Scale up a vector vec by s
vector2 scale_vec2(vector2 vec, double s){
    vector2 scaled = {vec.x * s, vec.y * s};
    return scaled;
}

// Add two vectors and return their sum
vector2 add_vec2s(vector2 vec1, vector2 vec2){
    vector2 sum = {vec1.x + vec2.x, vec1.y + vec2.y};
    return sum;
}

//Add 4 vecs (used for RK4)
vector2 add_4vec2s(vector2 vec1, vector2 vec2, vector2 vec3, vector2 vec4){
    return add_vec2s(add_vec2s(vec1, vec2), add_vec2s(vec3, vec4));
}

//subtract two vec2s from eachother
vector2 subtract_vec2s(vector2 vec1, vector2 vec2){
    vector2 diff = { vec1.x - vec2.x, vec1.y - vec2.y};
    return diff;
}