#include "utils/structures"

// normalize a vec3 to something that opengl can render
vector3 normalize_vec3(vector3 vec, double min, double max){
    vector3 n_vec = {(2.0 * (vec.x - min) / (max - min) - 1.0), (2.0 * (vec.y - min) / (max - min) - 1.0), (2.0 * (vec.z - min) / (max - min) - 1.0)};
    return n_vec;
}

// Scale up a vector vec by s
vector3 scale_vec3(vector3 vec, double s){
    return vector3 scaled = {vec.x * s, vec.y * s, vec.z * s};
}

// Add two vectors and return their sum
vector3 add_vec3s(vector3 vec1, vector3 vec2){
    vector3 sum = {vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z};
    return sum;
}

//subtract two vector3s from eachother
vector3 subtract_vec3s(vector3 vec1, vector3 vec2){
    vector3 difference = {vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z};
    return difference;
}