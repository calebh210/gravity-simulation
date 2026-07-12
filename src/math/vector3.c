#include "math/vector/vector3.h"

// normalize a vec3 to something that opengl can render
[[gnu::pure]] vector3 normalize_vec3(vector3 vec, double min, double max) {
    vector3 n_vec = {(2.0 * (vec.x - min) / (max - min) - 1.0), (2.0 * (vec.y - min) / (max - min) - 1.0), (2.0 * (vec.z - min) / (max - min) - 1.0)};
    return n_vec;
}

// denormalize a vector
[[gnu::pure]] vector3 denormalize_vec3(vector3 vec, double min, double max) {
    vector3 dn_vec = {
        (vec.x + 1.0) * (max - min) * 0.5 + min,
        (vec.y + 1.0) * (max - min) * 0.5 + min,
        (vec.z + 1.0) * (max - min) * 0.5 + min};
    return dn_vec;
}

// Scale up a vector vec by s
[[gnu::pure]] vector3 scale_vec3(vector3 vec, double s) {
    vector3 scaled = {vec.x * s, vec.y * s, vec.z * s};
    return scaled;
}

// Add two vectors and return their sum
[[gnu::pure]] vector3 add_vec3s(vector3 vec1, vector3 vec3) {
    vector3 sum = {vec1.x + vec3.x, vec1.y + vec3.y, vec1.z + vec3.z};
    return sum;
}

// Add 4 vecs (used for RK4)
[[gnu::pure]] vector3 add_4vec3s(vector3 v1, vector3 v2, vector3 v3, vector3 v4) {
    return add_vec3s(add_vec3s(v1, v2), add_vec3s(v3, v4));
}

// subtract two vector3s from eachother
[[gnu::pure]] vector3 subtract_vec3s(vector3 vec1, vector3 vec2) {
    vector3 difference = {vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z};
    return difference;
}

// Equivalent to glm::normalize
[[gnu::pure]] vector3 vec3_unit_vector(vector3 v) {

    double length_of_v = sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
    return (vector3){v.x / length_of_v, v.y / length_of_v, v.z / length_of_v};
}

// calculate the distance between two vec3s
[[gnu::pure]] double vec3_distance_between(vector3 v1, vector3 v2) {
    double r = sqrt(((v2.x - v1.x) * (v2.x - v1.x)) + ((v2.y - v1.y) * (v2.y - v1.y)) + ((v2.z - v1.z) * (v2.z - v1.z)));
    return r;
}

// calculate the dot product
// a1b1 + a2b2 + a3b3
[[gnu::pure]] double dot_vec3s(vector3 vec1, vector3 vec3) {
    double product = {(vec1.x * vec3.x) + (vec1.y * vec3.y) + (vec1.z * vec3.z)};
    return product;
}

// cross product!
[[gnu::pure]] vector3 cross_product(vector3 vec1, vector3 vec3) {
    vector3 product;
    product.x = (vec1.y * vec3.z) - (vec1.z * vec3.y);
    product.y = (vec1.z * vec3.x) - (vec1.x * vec3.z);
    product.z = (vec1.x * vec3.y) - (vec1.y * vec3.x);
    return product;
}

// Double Vector functions

// A lot of precision is lost. use this sparingly
vector3 dvector3_to_vector3(dvector3 vec) {
    vector3 result;
    result.x = (float)vec.x;
    result.y = (float)vec.y;
    result.z = (float)vec.z;
    return result;
}

dvector3 vector3_to_dvector3(vector3 vec) {
    dvector3 result;
    result.x = (double)vec.x;
    result.y = (double)vec.y;
    result.z = (double)vec.z;
    return result;
}

// subtract two vector3s from eachother
[[gnu::pure]] dvector3 subtract_dvec3s(dvector3 vec1, dvector3 vec2) {
    dvector3 difference = {vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z};
    return difference;
}

// Scale up a dvector vec by s
[[gnu::pure]] dvector3 scale_dvec3(dvector3 vec, double s) {
    dvector3 scaled = {vec.x * s, vec.y * s, vec.z * s};
    return scaled;
}

[[gnu::pure]] svector3 cartesian_to_spherical(vector3 vec) {
    svector3 result;
    result.r = sqrtf((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
    result.az = atan2f(vec.z, vec.x);
    result.el = acosf(vec.y / result.r);
    return result;
}

[[gnu::pure]] vector3 spherical_to_cartesian(svector3 vec) {
    vector3 result;
    result.x = vec.r * sinf(vec.el) * cosf(vec.az);
    result.y = vec.r * cosf(vec.el);
    result.z = vec.r * sinf(vec.el) * sinf(vec.az);
    return result;
}
