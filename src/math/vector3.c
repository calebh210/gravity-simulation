#include "utils/structures.h"
#include "math/vector/vector3.h"

// normalize a vec3 to something that opengl can render
vector3 normalize_vec3(vector3 vec, double min, double max){
    vector3 n_vec = {(2.0 * (vec.x - min) / (max - min) - 1.0), (2.0 * (vec.y - min) / (max - min) - 1.0), (2.0 * (vec.z - min) / (max - min) - 1.0)};
    return n_vec;
}

// denormalize a vector
vector3 denormalize_vec3(vector3 vec, double min, double max){
    vector3 dn_vec = {
    (vec.x + 1.0) * (max - min) * 0.5 + min,
    (vec.y + 1.0) * (max - min) * 0.5 + min,
    (vec.z + 1.0) * (max - min) * 0.5 + min
    };
    return dn_vec;
}


// Scale up a vector vec by s
vector3 scale_vec3(vector3 vec, double s){
    vector3 scaled = {vec.x * s, vec.y * s, vec.z * s};
    return scaled;
}

// Add two vectors and return their sum
vector3 add_vec3s(vector3 vec1, vector3 vec3){
    vector3 sum = {vec1.x + vec3.x, vec1.y + vec3.y, vec1.z + vec3.z};
    return sum;
}

//Add 4 vecs (used for RK4)
vector3 add_4vec3s(vector3 v1, vector3 v2, vector3 v3, vector3 v4){
    return add_vec3s(add_vec3s(v1, v2), add_vec3s(v3, v4));
}

//subtract two vector3s from eachother
vector3 subtract_vec3s(vector3 vec1, vector3 vec3){
    vector3 difference = {vec1.x - vec3.x, vec1.y - vec3.y, vec1.z - vec3.z};
    return difference;
}

//calculate the distance between two vec3s
double vec3_distance_between(vector3 v1, vector3 v2){
    double r = sqrt(((v2.x - v1.x)*(v2.x - v1.x)) + ((v2.y - v1.y)*(v2.y - v1.y)) + ((v2.z - v1.z)*(v2.z - v1.z)));
    return r;
}

//calculate the dot product
//a1b1 + a2b2 + a3b3
double dot_vec3s(vector3 vec1, vector3 vec3){
    double product = { (vec1.x * vec3.x) + (vec1.y * vec3.y) + (vec1.z * vec3.z)};
    return product;
}

//cross product!
vector3 cross_product(vector3 vec1, vector3 vec3){
    vector3 product;
    product.x = (vec1.y * vec3.z) - (vec1.z * vec3.y);
    product.y = (vec1.z * vec3.x) - (vec1.x * vec3.z);
    product.z = (vec1.x * vec3.y) - (vec1.y * vec3.x);
    return product;
}