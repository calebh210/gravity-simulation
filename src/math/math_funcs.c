#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include "math/math_funcs.h"
#include "math/vector/vector2.h"

// normalize values to something that opengl can render
float normalize(double value, double min, double max) {

    float norm = (float)(value - min) / (max - min);

    if(norm < 0.0002f){ // I want to be able to atleast kind of see it
       return 0.0002f;
    }

    if(norm > 1.0f){
        return 1.0f;
    } // clamping it to 1.0f

    return norm; 
}


float denormalize(double value, double min, double max){
    float original = value * (max - min) + min;
    return original;
}

// to find the center of gravity
 vector2 find_cog(double m1, vector2 pos1, double m2,  vector2 pos2){

    vector2 barycenter;

    barycenter.x = ((m1 * pos1.x) + (m2 * pos2.x));
    barycenter.y = ((m1 * pos1.y) + (m2 * pos2.y));

    barycenter.x = barycenter.x / (m1 + m2);
    barycenter.y = barycenter.y / (m1 + m2);

    return barycenter;

}

// Models u = G(m1+m2) 
// Since this is applicable in cases where m1 >> m2, we can assume u ~= Gm1
double standard_gravitational_parameter(double m1, double m2){
    return (G * (m1 + m2));
}

// take the mass (kg) of an object and determine its scharzchild radius
double scharzchild_radius(double mass){
    return (2 * G * mass) / (double)(c * c);
}

// RK4 Helper Function
vector2 f_x(double t, vector2 x, vector2 v){
    return v;
}

// RK4 Helper Function
// This models acceleration in a two body system, relative frame
vector2 f_v(double t ,vector2 x, vector2 v, double m){

    double r = sqrt(((0.0 -x.x)*(0.0 - x.x)) + ((0.0 - x.y)*(0.0 - x.y)));

    vector2 ddx = scale_vec2(x, -G*m); 
    ddx = scale_vec2(ddx, 1 / (r * r * r));
    return ddx;
}

// RK4 Helper Function
// This models acceleration in a two body system, intertial frame
vector2 f_v_inertial(double t,vector2 self_pos, vector2 other_pos, double mass_other){

    // these are two position vectors
    vector2 x = self_pos;
    vector2 y = other_pos;
    double r = vec2_distance_between(x,y);

    // this does: G * m1 * (x1 - x2)/(r * r * r)
    vector2 ddx = scale_vec2(subtract_vec2s(y,x), G*mass_other); 
    ddx = scale_vec2(ddx, 1 / (r * r * r));
    return ddx; 
}


// RK4 Helper Function
// Models acceleration relative to the Center of Gravity
vector2 f_v_rel_cog(double t, vector2 self_pos, vector2 other_pos, double mass_other, double mass_self){

    vector2 r_vec = subtract_vec2s(other_pos, self_pos);

    double r = sqrt(r_vec.x * r_vec.x + r_vec.y * r_vec.y);

    const float epsilon = 1e-5;
    if (r < epsilon) {
        return (vector2){0.0, 0.0};
    }

    // Gravitational parameter μ = G * (m1 + m2)
    double u = standard_gravitational_parameter(mass_self, mass_other);

    // Scale factor for acceleration relative to self mass fraction cubed:
    double mm = mass_self / (mass_self + mass_other);
    double u_tick = (mm * mm * mm) * u;

    // Acceleration vector: -(u') / r^3 * r_vec
    // NOTE this is supposed to be negative??? but was reversing my orbits so idk
    double scale = (1) * (u_tick) / (r * r * r);

    vector2 acceleration = scale_vec2(r_vec, scale);

    return acceleration;
}

// RK4 Helper Functional
// Determine the acceleration on body i from N other bodies in the system
// Newtonian Physics
// N = NUM_BODIES
vector2 f_v_nbody(double t, vector2 pos_self, body_2d* bodies[], int index, int N){

    vector2 accel = {0,0}; // init accel

    // iterate the bodies list, and calculate the force to self
    for(int i = 0; i < N; i++){

        if(i == index){ // Skip self in list

            // puts("Body in list is self, skipping...");
            continue;

        }


        double r = vec2_distance_between(pos_self, bodies[i]->pos);

        vector2 ddx = scale_vec2(subtract_vec2s(bodies[i]->pos, pos_self), G*bodies[i]->mass); 

        //prevent accel from going to infinity
        // using a dampner maybe? will look into
        const double epsilon = 10E8;
        if (r < epsilon) {
            printf("\nObject %d and object %d probably collided... Closing Simulation\n", i, index);
            exit(0);
        } 

        ddx = scale_vec2(ddx, 1 / (r * r * r));
        

        accel = add_vec2s(accel ,ddx);

    }

    return accel;


}



// Fourth-Order Runge-Kutta algorithm
// implemented for 2nd ODEs
// co-integrates two bodies acting upon eachother for maximum accuracy
// t is the current time. Will be 0 for our case here (Newtonian Physiscs)
// h is the step size
// b1 and b2 are two dimensional bodies who are acting upon eachother in euclidean spacetime
// f_x() and f_v() are the functions that return the derivates of x and v (so v and a)
// in our sim, f_v() = the accel calc, and f_x() is a step

void coint_runge_kutta(double t, double h, body_2d *body1, body_2d *body2){

    // pos, vel, and mass of b1
    vector2 x1 = body1->pos;
    vector2 v1 = body1->velocity; 
    double m1 =  body1->mass;

    // pos, vel, and mass of b2
    vector2 x2 = body2->pos;
    vector2 v2 = body2->velocity; 
    double m2 =  body2->mass;

    //rk4 steps

    //k1 step
    vector2 k1_x1 = f_x(t,x1,v1); // explicit euler. equal to k1 = h * f_x
    vector2 k1_v1 = f_v_inertial(t,x1,x2,m2);
    vector2 k1_x2 = f_x(t,x2,v2); 
    vector2 k1_v2 = f_v_inertial(t,x2,x1,m1);

    //k2
    vector2 k2_x1 = f_x(t + h * 0.5, add_vec2s(x1, scale_vec2(k1_x1, h * 0.5)), add_vec2s(v1, scale_vec2(k1_v1, h * 0.5))); //multiply by 0.5 is faster than /2
    vector2 k2_v1 = f_v_inertial(t + h * 0.5, add_vec2s(x1, scale_vec2(k1_x1, h * 0.5)), add_vec2s(x2, scale_vec2(k1_x2, h * 0.5)),m2);
    vector2 k2_x2 = f_x(t + h * 0.5, add_vec2s(x2, scale_vec2(k1_x2, h * 0.5)), add_vec2s(v2, scale_vec2(k1_v2, h * 0.5))); //multiply by 0.5 is faster than /2
    vector2 k2_v2 = f_v_inertial(t + h * 0.5, add_vec2s(x2, scale_vec2(k1_x2, h * 0.5)), add_vec2s(x1, scale_vec2(k1_x1, h * 0.5)),m1);


    vector2 k3_x1 = f_x(t + h * 0.5, add_vec2s(x1, scale_vec2(k2_x1, h * 0.5)), add_vec2s(v1, scale_vec2(k2_v1, h * 0.5)));
    vector2 k3_v1 = f_v_inertial(t + h * 0.5, add_vec2s(x1, scale_vec2(k2_x1, h * 0.5)), add_vec2s(x2, scale_vec2(k2_x2, h * 0.5)),m2);
    vector2 k3_x2 = f_x(t + h * 0.5, add_vec2s(x2, scale_vec2(k2_x2, h * 0.5)), add_vec2s(v2, scale_vec2(k2_v2, h * 0.5)));
    vector2 k3_v2 = f_v_inertial(t + h * 0.5, add_vec2s(x2, scale_vec2(k2_x2, h * 0.5)), add_vec2s(x1, scale_vec2(k2_x1, h * 0.5)),m1);


    vector2 k4_x1 = f_x(t + h, add_vec2s(x1, scale_vec2(k3_x1, h)), add_vec2s(v1, scale_vec2(k3_v1, h))); // implicit euler
    vector2 k4_v1 = f_v_inertial(t + h, add_vec2s(x1, scale_vec2(k3_x1, h)), add_vec2s(x2, scale_vec2(k3_x2, h)),m2);
    vector2 k4_x2 = f_x(t + h, add_vec2s(x2, scale_vec2(k3_x2, h)), add_vec2s(v2, scale_vec2(k3_v2, h))); 
    vector2 k4_v2 = f_v_inertial(t + h, add_vec2s(x2, scale_vec2(k3_x2, h)), add_vec2s(x1, scale_vec2(k3_x1, h)),m1);



    body1->pos = add_vec2s(x1, scale_vec2(add_4vec2s(k1_x1, scale_vec2(k2_x1, 2), scale_vec2(k3_x1, 2), k4_x1), h / 6.0));
    body1->velocity = add_vec2s(v1, scale_vec2(add_4vec2s(k1_v1, scale_vec2(k2_v1, 2), scale_vec2(k3_v1, 2), k4_v1), h / 6.0));

    body2->pos = add_vec2s(x2, scale_vec2(add_4vec2s(k1_x2, scale_vec2(k2_x2, 2), scale_vec2(k3_x2, 2), k4_x2), h / 6.0));
    body2->velocity = add_vec2s(v2, scale_vec2(add_4vec2s(k1_v2, scale_vec2(k2_v2, 2), scale_vec2(k3_v2, 2), k4_v2), h / 6.0));

}

// Fourth-Order Runge-Kutta algorithm
// implemented for one body system (m1 >> m2)
// t is the current time. Will be 0 for our case here (Newtonian Physiscs)
// h is the step size
// x is position
// v is velocity (derivative of velocity)
// m is the mass of the object 
// f_x() and f_v() are the functions that return the derivates of x and v (so v and a)
// in our sim, f_v() = the accel calc, and f_x() is a step

void runge_kutta(double t, double h, double m, body_2d *b){

    vector2 x = b->pos;
    vector2 v = b->velocity; 


    //define steps
    vector2 k1_x;
    vector2 k1_v;
    vector2 k2_x;
    vector2 k2_v;
    vector2 k3_x;
    vector2 k3_v;
    vector2 k4_x;
    vector2 k4_v;

    k1_x = f_x(t,x,v); // explicit euler. equal to k1 = h * f_x
    k1_v = f_v(t,x,v,m);
    k2_x = f_x(t + h * 0.5, add_vec2s(x, scale_vec2(k1_x, h * 0.5)), add_vec2s(v, scale_vec2(k1_v, h * 0.5))); //multiply by 0.5 is faster than /2
    k2_v = f_v(t + h * 0.5, add_vec2s(x, scale_vec2(k1_x, h * 0.5)), add_vec2s(v, scale_vec2(k1_v, h * 0.5)),m);
    k3_x = f_x(t + h * 0.5, add_vec2s(x, scale_vec2(k2_x, h * 0.5)), add_vec2s(v, scale_vec2(k2_v, h * 0.5)));
    k3_v = f_v(t + h * 0.5, add_vec2s(x, scale_vec2(k2_x, h * 0.5)), add_vec2s(v, scale_vec2(k2_v, h * 0.5)),m);

    k4_x = f_x(t + h, add_vec2s(x, scale_vec2(k3_x, h)), add_vec2s(v, scale_vec2(k3_v, h))); // implicit euler
    k4_v = f_v(t + h, add_vec2s(x, scale_vec2(k3_x, h)), add_vec2s(v, scale_vec2(k3_v, h)),m);

    b->pos = add_vec2s(x, scale_vec2(add_4vec2s(k1_x, scale_vec2(k2_x, 2), scale_vec2(k3_x, 2), k4_x), h / 6.0));

    b->velocity = add_vec2s(v, scale_vec2(add_4vec2s(k1_v, scale_vec2(k2_v, 2), scale_vec2(k3_v, 2), k4_v), h / 6.0));

}


void cog_ref_runge_kutta(double t, double h, body_2d *body1, body_2d *body2){

    // pos, vel, and mass of b1
    vector2 x1_init = body1->pos;
    vector2 v1_init = body1->velocity; 
    double m1 =  body1->mass;

    // pos, vel, and mass of b2
    vector2 x2_init = body2->pos;
    vector2 v2_init = body2->velocity; 
    double m2 =  body2->mass;


    //frame conversion
    vector2 v_com = {
    (m1 * v1_init.x + m2 * v2_init.x) / (m1 + m2),
    (m1 * v1_init.y + m2 * v2_init.y) / (m1 + m2)
    };
    vector2 com = find_cog(m1, x1_init, m2, x2_init);

    vector2 x1 = { x1_init.x - com.x, x1_init.y - com.y };
    vector2 x2 = { x2_init.x - com.x, x2_init.y - com.y };

    vector2 v1 = { v1_init.x - v_com.x, v1_init.y - v_com.y };
    vector2 v2 = { v2_init.x - v_com.x, v2_init.y - v_com.y };

    //rk4 steps

    //k1 step
    vector2 k1_x1 = f_x(t,x1,v1); // explicit euler. equal to k1 = h * f_x
    vector2 k1_v1 = f_v_rel_cog(t,x1,x2,m2,m1);
    vector2 k1_x2 = f_x(t,x2,v2); 
    vector2 k1_v2 = f_v_rel_cog(t,x2,x1,m1,m2);

    //k2
    vector2 k2_x1 = f_x(t + h * 0.5, add_vec2s(x1, scale_vec2(k1_x1, h * 0.5)), add_vec2s(v1, scale_vec2(k1_v1, h * 0.5))); //multiply by 0.5 is faster than /2
    vector2 k2_v1 = f_v_rel_cog(t + h * 0.5, add_vec2s(x1, scale_vec2(k1_x1, h * 0.5)), add_vec2s(x2, scale_vec2(k1_x2, h * 0.5)),m2,m1);
    vector2 k2_x2 = f_x(t + h * 0.5, add_vec2s(x2, scale_vec2(k1_x2, h * 0.5)), add_vec2s(v2, scale_vec2(k1_v2, h * 0.5))); //multiply by 0.5 is faster than /2
    vector2 k2_v2 = f_v_rel_cog(t + h * 0.5, add_vec2s(x2, scale_vec2(k1_x2, h * 0.5)), add_vec2s(x1, scale_vec2(k1_x1, h * 0.5)),m1,m2);


    vector2 k3_x1 = f_x(t + h * 0.5, add_vec2s(x1, scale_vec2(k2_x1, h * 0.5)), add_vec2s(v1, scale_vec2(k2_v1, h * 0.5)));
    vector2 k3_v1 = f_v_rel_cog(t + h * 0.5, add_vec2s(x1, scale_vec2(k2_x1, h * 0.5)), add_vec2s(x2, scale_vec2(k2_x2, h * 0.5)),m2,m1);
    vector2 k3_x2 = f_x(t + h * 0.5, add_vec2s(x2, scale_vec2(k2_x2, h * 0.5)), add_vec2s(v2, scale_vec2(k2_v2, h * 0.5)));
    vector2 k3_v2 = f_v_rel_cog(t + h * 0.5, add_vec2s(x2, scale_vec2(k2_x2, h * 0.5)), add_vec2s(x1, scale_vec2(k2_x1, h * 0.5)),m1,m2);


    vector2 k4_x1 = f_x(t + h, add_vec2s(x1, scale_vec2(k3_x1, h)), add_vec2s(v1, scale_vec2(k3_v1, h))); // implicit euler
    vector2 k4_v1 = f_v_rel_cog(t + h, add_vec2s(x1, scale_vec2(k3_x1, h)), add_vec2s(x2, scale_vec2(k3_x2, h)),m2,m1);
    vector2 k4_x2 = f_x(t + h, add_vec2s(x2, scale_vec2(k3_x2, h)), add_vec2s(v2, scale_vec2(k3_v2, h))); 
    vector2 k4_v2 = f_v_rel_cog(t + h, add_vec2s(x2, scale_vec2(k3_x2, h)), add_vec2s(x1, scale_vec2(k3_x1, h)),m1,m2);



    body1->pos = add_vec2s(x1, scale_vec2(add_4vec2s(k1_x1, scale_vec2(k2_x1, 2), scale_vec2(k3_x1, 2), k4_x1), h / 6.0));
    body1->velocity = add_vec2s(v1, scale_vec2(add_4vec2s(k1_v1, scale_vec2(k2_v1, 2), scale_vec2(k3_v1, 2), k4_v1), h / 6.0));

    body2->pos = add_vec2s(x2, scale_vec2(add_4vec2s(k1_x2, scale_vec2(k2_x2, 2), scale_vec2(k3_x2, 2), k4_x2), h / 6.0));
    body2->velocity = add_vec2s(v2, scale_vec2(add_4vec2s(k1_v2, scale_vec2(k2_v2, 2), scale_vec2(k3_v2, 2), k4_v2), h / 6.0));

}


//Calculate the force each body exerts on every other
//Uses Runge-Kutta 4
// t is start time
// h is time step
// bodies is an array of body_2d pointers
// N is the number of bodies in the system
void rk4_nbody(double t, double h, body_2d* bodies[], int N){

    //Define the sub-steps as arrays to hold each body
    // kX_x is for position,kX_v is for velocity
    vector2 x[N], v[N];
    vector2 k1_x[N], k1_v[N];
    vector2 k2_x[N], k2_v[N];
    vector2 k3_x[N], k3_v[N];
    vector2 k4_x[N], k4_v[N];

    // temp bodies to use in RK4 steps
    body_2d* temp_bodies[N];

    // Step 1 and Step 1 pre-calc
    for(int i = 0; i < N; i++){

        //get the vel and pos of my current object to pass into fx and fv functions
        // also pass in i, so that the body isn't calculated against itself
        x[i] = bodies[i]->pos;
        v[i] = bodies[i]->velocity; 

        k1_x[i] = f_x(t,x[i], v[i]); 
        k1_v[i] = f_v_nbody(t,x[i],bodies,i,N);

        temp_bodies[i] = malloc(sizeof(body_2d)); //make a new space in memory for each one. this way I don't overwrite the real positions
        *temp_bodies[i] = *bodies[i]; 

        temp_bodies[i]->pos = add_vec2s(x[i], scale_vec2(k1_x[i], h * 0.5));
        temp_bodies[i]->velocity = add_vec2s(v[i], scale_vec2(k1_v[i], h * 0.5));
    }

    //Step 2
    for(int i = 0; i < N; i++){

        k2_x[i] = f_x(t + h * 0.5, temp_bodies[i]->pos, temp_bodies[i]->velocity); //multiply by 0.5 is faster than /2
        k2_v[i] = f_v_nbody(t + h * 0.5, temp_bodies[i]->pos, temp_bodies,i,N);
    }

    //Step3 Pre-Calc
    for(int i = 0; i < N; i++){

        temp_bodies[i]->pos = add_vec2s(x[i], scale_vec2(k2_x[i], h * 0.5));
        temp_bodies[i]->velocity = add_vec2s(v[i], scale_vec2(k2_v[i], h * 0.5));
    }

    // Step 3
    for(int i = 0; i < N; i++){

        k3_x[i] = f_x(t + h * 0.5, temp_bodies[i]->pos, temp_bodies[i]->velocity);
        k3_v[i] = f_v_nbody(t + h * 0.5, temp_bodies[i]->pos, temp_bodies,i,N);
    }

    // Step 4 pre-calc
    for(int i = 0; i < N; i++){

        temp_bodies[i]->pos = add_vec2s(x[i], scale_vec2(k3_x[i], h));
        temp_bodies[i]->velocity = add_vec2s(v[i], scale_vec2(k3_v[i], h));
    }

    // Step 4
    for(int i = 0; i < N; i++){

        k4_x[i] = f_x(t + h, temp_bodies[i]->pos, temp_bodies[i]->velocity); 
        k4_v[i] = f_v_nbody(t + h, temp_bodies[i]->pos ,temp_bodies,i,N);


        bodies[i]->pos = add_vec2s(x[i], scale_vec2(add_4vec2s(k1_x[i], scale_vec2(k2_x[i], 2), scale_vec2(k3_x[i], 2), k4_x[i]), h / 6.0));
        bodies[i]->velocity = add_vec2s(v[i], scale_vec2(add_4vec2s(k1_v[i], scale_vec2(k2_v[i], 2), scale_vec2(k3_v[i], 2), k4_v[i]), h / 6.0));

    }

    // Free the memory taken by the temp bodies
    for(int i = 0; i < N; i++){
        free(temp_bodies[i]);
    }


}