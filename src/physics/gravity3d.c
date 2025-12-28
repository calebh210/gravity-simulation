#include "physics/gravity3d.h"
#include "math/math_funcs.h"
#include <stdio.h>
#include <stdlib.h>

// RK4 Helper Function
static vector3 f_x_3d(double t, vector3 x, vector3 v){
    return v;
}

// RK4 Helper Functional
// Determine the acceleration on body i from N other bodies in the system
// Newtonian Physics
// N = NUM_BODIES
static vector3 f_v_nbody_3d(double t, vector3 pos_self, body_3d* bodies[], int index, int N){

    vector3 accel = {0,0}; // init accel

    // iterate the bodies list, and calculate the force to self
    for(int i = 0; i < N; i++){

        if(i == index){ // Skip self in list

            // puts("Body in list is self, skipping...");
            continue;

        }


        double r = vec3_distance_between(pos_self, bodies[i]->pos);

        vector3 ddx = scale_vec3(subtract_vec3s(bodies[i]->pos, pos_self), G*bodies[i]->mass); 

        //prevent accel from going to infinity
        // using a dampner maybe? will look into
        const double epsilon = 10E8;
        if (r < epsilon) {
            printf("\nObject %d and object %d probably collided... Closing Simulation\n", i, index);
            exit(0);
        } 

        ddx = scale_vec3(ddx, 1 / (r * r * r));
        

        accel = add_vec3s(accel ,ddx);

    }

    return accel;


}

//Calculate the force each body exerts on every other
//Uses Runge-Kutta 4
// t is start time
// h is time step
// bodies is an array of body_3d pointers
// N is the number of bodies in the system
void rk4_nbody_3d(double t, double h, body_3d* bodies[], int N){

    //Define the sub-steps as arrays to hold each body
    // kX_x is for position,kX_v is for velocity
    vector3 x[N], v[N];
    vector3 k1_x[N], k1_v[N];
    vector3 k2_x[N], k2_v[N];
    vector3 k3_x[N], k3_v[N];
    vector3 k4_x[N], k4_v[N];

    // temp bodies to use in RK4 steps
    body_3d* temp_bodies[N];

    // Step 1 and Step 1 pre-calc
    for(int i = 0; i < N; i++){

        //get the vel and pos of my current object to pass into fx and fv functions
        // also pass in i, so that the body isn't calculated against itself
        x[i] = bodies[i]->pos;
        v[i] = bodies[i]->velocity; 

        k1_x[i] = f_x_3d(t,x[i], v[i]); 
        k1_v[i] = f_v_nbody_3d(t,x[i],bodies,i,N);

        temp_bodies[i] = malloc(sizeof(body_3d)); //make a new space in memory for each one. this way I don't overwrite the real positions
        *temp_bodies[i] = *bodies[i]; 

        temp_bodies[i]->pos = add_vec3s(x[i], scale_vec3(k1_x[i], h * 0.5));
        temp_bodies[i]->velocity = add_vec3s(v[i], scale_vec3(k1_v[i], h * 0.5));
    }

    //Step 2
    for(int i = 0; i < N; i++){

        k2_x[i] = f_x_3d(t + h * 0.5, temp_bodies[i]->pos, temp_bodies[i]->velocity); //multiply by 0.5 is faster than /2
        k2_v[i] = f_v_nbody_3d(t + h * 0.5, temp_bodies[i]->pos, temp_bodies,i,N);
    }

    //Step3 Pre-Calc
    for(int i = 0; i < N; i++){

        temp_bodies[i]->pos = add_vec3s(x[i], scale_vec3(k2_x[i], h * 0.5));
        temp_bodies[i]->velocity = add_vec3s(v[i], scale_vec3(k2_v[i], h * 0.5));
    }

    // Step 3
    for(int i = 0; i < N; i++){

        k3_x[i] = f_x_3d(t + h * 0.5, temp_bodies[i]->pos, temp_bodies[i]->velocity);
        k3_v[i] = f_v_nbody_3d(t + h * 0.5, temp_bodies[i]->pos, temp_bodies,i,N);
    }

    // Step 4 pre-calc
    for(int i = 0; i < N; i++){

        temp_bodies[i]->pos = add_vec3s(x[i], scale_vec3(k3_x[i], h));
        temp_bodies[i]->velocity = add_vec3s(v[i], scale_vec3(k3_v[i], h));
    }

    // Step 4
    for(int i = 0; i < N; i++){

        k4_x[i] = f_x_3d(t + h, temp_bodies[i]->pos, temp_bodies[i]->velocity); 
        k4_v[i] = f_v_nbody_3d(t + h, temp_bodies[i]->pos ,temp_bodies,i,N);


        bodies[i]->pos = add_vec3s(x[i], scale_vec3(add_4vec3s(k1_x[i], scale_vec3(k2_x[i], 2), scale_vec3(k3_x[i], 2), k4_x[i]), h / 6.0));
        bodies[i]->velocity = add_vec3s(v[i], scale_vec3(add_4vec3s(k1_v[i], scale_vec3(k2_v[i], 2), scale_vec3(k3_v[i], 2), k4_v[i]), h / 6.0));

    }

    // Free the memory taken by the temp bodies
    for(int i = 0; i < N; i++){
        free(temp_bodies[i]);
    }


}