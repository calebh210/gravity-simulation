#include <stdlib.h>
#include <stdio.h>
#include <math.h>


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


// normalize values to something that opengl can render
float normalize(double value, double min, double max) {
    return (float)(2.0 * (value - min) / (max - min) - 1.0);
}

// Scale up a vector vec by s
two_d_vector scale_vec2(two_d_vector vec, double s){
    two_d_vector scaled = {vec.x * s, vec.y * s};
    return scaled;
}

// Add two vectors and return their sum
two_d_vector add_vecs(two_d_vector vec1, two_d_vector vec2){
    two_d_vector sum = {vec1.x + vec2.x, vec1.y + vec2.y};
    return sum;
}

two_d_vector add_vecs4(two_d_vector vec1, two_d_vector vec2, two_d_vector vec3, two_d_vector vec4){
    return add_vecs(add_vecs(vec1, vec2), add_vecs(vec3, vec4));
}


// RK4 Helper Function
two_d_vector f_x(double t, two_d_vector x, two_d_vector v){
    return v;
}

// RK4 Helper Function
// This models acceleration in a two body system, relative frame
two_d_vector f_v(double t ,two_d_vector x, two_d_vector v, double m){

    double r = sqrt(((0.0 -x.x)*(0.0 - x.x)) + ((0.0 - x.y)*(0.0 - x.y)));

    two_d_vector ddx = scale_vec2(x, -G*m); 
    ddx = scale_vec2(ddx, 1 / (r * r * r));
    return ddx;
}

// Fourth-Order Runge-Kutta algorithm
// implemented for 2nd ODEs
// t is the current time. Will be 0 for our case here (Newtonian Physiscs)
// h is the step size
// x is position
// v is velocity (derivative of velocity)
// r is the distance between two masses in 2d space (needed for f_v())
// m is the mass of the object 
// f_x() and f_v() are the functions that return the derivates of x and v (so v and a)
// in our sim, f_v() = the accel calc, and f_x() is a step

void runge_kutta(double t, double h, double m, two_d_body *b){

    two_d_vector x = b->pos;
    two_d_vector v = b->velocity; 


    //define steps
    two_d_vector k1_x;
    two_d_vector k1_v;
    two_d_vector k2_x;
    two_d_vector k2_v;
    two_d_vector k3_x;
    two_d_vector k3_v;
    two_d_vector k4_x;
    two_d_vector k4_v;

    k1_x = f_x(t,x,v); // explicit euler. equal to k1 = h * f_x
    k1_v = f_v(t,x,v,m);
    k2_x = f_x(t + h * 0.5, add_vecs(x, scale_vec2(k1_x, h * 0.5)), add_vecs(v, scale_vec2(k1_v, h * 0.5))); //multiply by 0.5 is faster than /2
    k2_v = f_v(t + h * 0.5, add_vecs(x, scale_vec2(k1_x, h * 0.5)), add_vecs(v, scale_vec2(k1_v, h * 0.5)),m);
    k3_x = f_x(t + h * 0.5, add_vecs(x, scale_vec2(k2_x, h * 0.5)), add_vecs(v, scale_vec2(k2_v, h * 0.5)));
    k3_v = f_v(t + h * 0.5, add_vecs(x, scale_vec2(k2_x, h * 0.5)), add_vecs(v, scale_vec2(k2_v, h * 0.5)),m);

    k4_x = f_x(t + h, add_vecs(x, scale_vec2(k3_x, h)), add_vecs(v, scale_vec2(k3_v, h))); // implicit euler
    k4_v = f_v(t + h, add_vecs(x, scale_vec2(k3_x, h)), add_vecs(v, scale_vec2(k3_v, h)),m);

    b->pos = add_vecs(x, scale_vec2(add_vecs4(k1_x, scale_vec2(k2_x, 2), scale_vec2(k3_x, 2), k4_x), h / 6.0));

    b->velocity = add_vecs(v, scale_vec2(add_vecs4(k1_v, scale_vec2(k2_v, 2), scale_vec2(k3_v, 2), k4_v), h / 6.0));

}