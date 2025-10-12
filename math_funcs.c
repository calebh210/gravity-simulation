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

//Add 4 vecs (used for RK4)
two_d_vector add_vecs4(two_d_vector vec1, two_d_vector vec2, two_d_vector vec3, two_d_vector vec4){
    return add_vecs(add_vecs(vec1, vec2), add_vecs(vec3, vec4));
}

//subtract two vec2s from eachother
two_d_vector subtract_vec2s(two_d_vector vec1, two_d_vector vec2){
    two_d_vector diff = { vec1.x - vec2.x, vec1.y - vec2.y};
    return diff;
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

// RK4 Helper Function
// This models acceleration in a two body system, intertial frame
two_d_vector f_v_inertial(double t,two_d_vector self_pos, two_d_vector other_pos, double mass_other){

    // these are two position vectors
    two_d_vector x = self_pos;
    two_d_vector y = other_pos;
    // TODO: Add some vector funcs here to clean this up
    double r = sqrt(((y.x -x.x)*(y.x - x.x)) + ((y.y - x.y)*(y.y - x.y)));

    // this does: G * m1 * (x1 - x2)/(r * r * r)
    two_d_vector ddx = scale_vec2(subtract_vec2s(y,x), G*mass_other); 
    ddx = scale_vec2(ddx, 1 / (r * r * r));
    return ddx;
}

// Fourth-Order Runge-Kutta algorithm
// implemented for 2nd ODEs
// co-integrates two bodies acting upon eachother for maximum accuracy
// t is the current time. Will be 0 for our case here (Newtonian Physiscs)
// h is the step size
// b1 and b2 are two dimensional bodies who are acting upon eachother in euclidean spacetime
// f_x() and f_v() are the functions that return the derivates of x and v (so v and a)
// in our sim, f_v() = the accel calc, and f_x() is a step

void coint_runge_kutta(double t, double h, two_d_body *body1, two_d_body *body2){

    // pos, vel, and mass of b1
    two_d_vector x1 = body1->pos;
    two_d_vector v1 = body1->velocity; 
    double m1 =  body1->mass;

    // pos, vel, and mass of b2
    two_d_vector x2 = body2->pos;
    two_d_vector v2 = body2->velocity; 
    double m2 =  body2->mass;

    //rk4 steps

    //k1 step
    two_d_vector k1_x1 = f_x(t,x1,v1); // explicit euler. equal to k1 = h * f_x
    two_d_vector k1_v1 = f_v_inertial(t,x1,x2,m2);
    two_d_vector k1_x2 = f_x(t,x2,v2); 
    two_d_vector k1_v2 = f_v_inertial(t,x2,x1,m1);

    //k2
    two_d_vector k2_x1 = f_x(t + h * 0.5, add_vecs(x1, scale_vec2(k1_x1, h * 0.5)), add_vecs(v1, scale_vec2(k1_v1, h * 0.5))); //multiply by 0.5 is faster than /2
    two_d_vector k2_v1 = f_v_inertial(t + h * 0.5, add_vecs(x1, scale_vec2(k1_x1, h * 0.5)), add_vecs(x2, scale_vec2(k1_x2, h * 0.5)),m2);
    two_d_vector k2_x2 = f_x(t + h * 0.5, add_vecs(x2, scale_vec2(k1_x2, h * 0.5)), add_vecs(v2, scale_vec2(k1_v2, h * 0.5))); //multiply by 0.5 is faster than /2
    two_d_vector k2_v2 = f_v_inertial(t + h * 0.5, add_vecs(x2, scale_vec2(k1_x2, h * 0.5)), add_vecs(x1, scale_vec2(k1_x1, h * 0.5)),m1);


    two_d_vector k3_x1 = f_x(t + h * 0.5, add_vecs(x1, scale_vec2(k2_x1, h * 0.5)), add_vecs(v1, scale_vec2(k2_v1, h * 0.5)));
    two_d_vector k3_v1 = f_v_inertial(t + h * 0.5, add_vecs(x1, scale_vec2(k2_x1, h * 0.5)), add_vecs(x2, scale_vec2(k2_x2, h * 0.5)),m2);
    two_d_vector k3_x2 = f_x(t + h * 0.5, add_vecs(x2, scale_vec2(k2_x2, h * 0.5)), add_vecs(v2, scale_vec2(k2_v2, h * 0.5)));
    two_d_vector k3_v2 = f_v_inertial(t + h * 0.5, add_vecs(x2, scale_vec2(k2_x2, h * 0.5)), add_vecs(x1, scale_vec2(k2_x1, h * 0.5)),m1);


    two_d_vector k4_x1 = f_x(t + h, add_vecs(x1, scale_vec2(k3_x1, h)), add_vecs(v1, scale_vec2(k3_v1, h))); // implicit euler
    two_d_vector k4_v1 = f_v_inertial(t + h, add_vecs(x1, scale_vec2(k3_x1, h)), add_vecs(x2, scale_vec2(k3_x2, h)),m2);
    two_d_vector k4_x2 = f_x(t + h, add_vecs(x2, scale_vec2(k3_x2, h)), add_vecs(v2, scale_vec2(k3_v2, h))); 
    two_d_vector k4_v2 = f_v_inertial(t + h, add_vecs(x2, scale_vec2(k3_x2, h)), add_vecs(x1, scale_vec2(k3_x1, h)),m1);



    body1->pos = add_vecs(x1, scale_vec2(add_vecs4(k1_x1, scale_vec2(k2_x1, 2), scale_vec2(k3_x1, 2), k4_x1), h / 6.0));
    body1->velocity = add_vecs(v1, scale_vec2(add_vecs4(k1_v1, scale_vec2(k2_v1, 2), scale_vec2(k3_v1, 2), k4_v1), h / 6.0));

    body2->pos = add_vecs(x2, scale_vec2(add_vecs4(k1_x2, scale_vec2(k2_x2, 2), scale_vec2(k3_x2, 2), k4_x2), h / 6.0));
    body2->velocity = add_vecs(v2, scale_vec2(add_vecs4(k1_v2, scale_vec2(k2_v2, 2), scale_vec2(k3_v2, 2), k4_v2), h / 6.0));

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