#include "physics/gravity3d.h"
#include "math/math_funcs.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// RK4 Helper Function
[[gnu::pure]] static vector3 f_x(double t, vector3 x, vector3 v){
    return v;
}

// RK4 Helper Functional
// Determine the acceleration on body i from N other bodies in the system
// Newtonian Physics
// N = NUM_BODIES
[[gnu::pure]] static vector3 f_v_nbody_3d(double t, vector3 pos_self, body_3d* bodies[], int index, int N){

    vector3 accel = {0,0}; // init accel

    // iterate the bodies list, and calculate the force to self
    for(int i = 0; i < N; i++){

        if(i == index){ // Skip self in list

            // puts("Body in list is self, skipping...");
            continue;

        }

        double r = vec3_distance_between(pos_self, bodies[i]->pos);

        dvector3 ddx = scale_dvec3(vector3_to_dvector3(subtract_vec3s(bodies[i]->pos, pos_self)), G*bodies[i]->mass); 

        //prevent accel from going to infinity
        // using a dampner maybe? will look into
        const double epsilon = 10E-4;
        if (r < epsilon) {
            printf("\nObject %d and object %d probably collided...` Closing Simulation\n", i, index);
            exit(0);
        } 

        vector3 velocity = bodies[i]->velocity;
        // check if anything is going FTL. if yes, then it means the sim exploded
        if(abs(velocity.x) > 300000000 || abs(velocity.y) > 300000000 || abs(velocity.z) > 300000000){
            printf("\nObject detected going Faster-Than-Light. Simulation has likely blown up. Shutting down...");
            exit(0);
        }

        ddx = scale_dvec3(ddx, 1 / (r * r * r));

        // There is precision lost here
        accel = add_vec3s(accel, dvector3_to_vector3(ddx));

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

        k1_x[i] = f_x(t,x[i], v[i]); 
        k1_v[i] = f_v_nbody_3d(t,x[i],bodies,i,N);

        temp_bodies[i] = malloc(sizeof(body_3d)); //make a new space in memory for each one. this way I don't overwrite the real positions
        *temp_bodies[i] = *bodies[i]; 

        temp_bodies[i]->pos = add_vec3s(x[i], scale_vec3(k1_x[i], h * 0.5));
        temp_bodies[i]->velocity = add_vec3s(v[i], scale_vec3(k1_v[i], h * 0.5));
    }

    //Step 2
    for(int i = 0; i < N; i++){

        k2_x[i] = f_x(t + h * 0.5, temp_bodies[i]->pos, temp_bodies[i]->velocity); //multiply by 0.5 is faster than /2
        k2_v[i] = f_v_nbody_3d(t + h * 0.5, temp_bodies[i]->pos, temp_bodies,i,N);
    }

    //Step3 Pre-Calc
    for(int i = 0; i < N; i++){

        temp_bodies[i]->pos = add_vec3s(x[i], scale_vec3(k2_x[i], h * 0.5));
        temp_bodies[i]->velocity = add_vec3s(v[i], scale_vec3(k2_v[i], h * 0.5));
    }

    // Step 3
    for(int i = 0; i < N; i++){

        k3_x[i] = f_x(t + h * 0.5, temp_bodies[i]->pos, temp_bodies[i]->velocity);
        k3_v[i] = f_v_nbody_3d(t + h * 0.5, temp_bodies[i]->pos, temp_bodies,i,N);
    }

    // Step 4 pre-calc
    for(int i = 0; i < N; i++){

        temp_bodies[i]->pos = add_vec3s(x[i], scale_vec3(k3_x[i], h));
        temp_bodies[i]->velocity = add_vec3s(v[i], scale_vec3(k3_v[i], h));
    }

    // Step 4
    for(int i = 0; i < N; i++){

        k4_x[i] = f_x(t + h, temp_bodies[i]->pos, temp_bodies[i]->velocity); 
        k4_v[i] = f_v_nbody_3d(t + h, temp_bodies[i]->pos ,temp_bodies,i,N);


        bodies[i]->pos = add_vec3s(x[i], scale_vec3(add_4vec3s(k1_x[i], scale_vec3(k2_x[i], 2), scale_vec3(k3_x[i], 2), k4_x[i]), h / 6.0));
        bodies[i]->velocity = add_vec3s(v[i], scale_vec3(add_4vec3s(k1_v[i], scale_vec3(k2_v[i], 2), scale_vec3(k3_v[i], 2), k4_v[i]), h / 6.0));

    }

    // Free the memory taken by the temp bodies
    for(int i = 0; i < N; i++){
        free(temp_bodies[i]);
    }

}


// to find the center of gravity
// This is a LOSSY function, due to the casting from double -> float
// Should mostly work, but will produce inaccurate results at bigger values
[[gnu::pure]] static vector3 find_cog(double m1, vector3 pos1, double m2,  vector3 pos2){

    dvector3 barycenter;

    barycenter.x = (m1 * pos1.x) + (m2 * pos2.x);
    barycenter.y = ((m1 * pos1.y) + (m2 * pos2.y));
    barycenter.z = ((m1 * pos1.z) + (m2 * pos2.z));

    barycenter.x = barycenter.x / (m1 + m2);
    barycenter.y = barycenter.y / (m1 + m2);
    barycenter.z = barycenter.z / (m1 + m2);

    vector3 result = dvector3_to_vector3(barycenter);

    return result;

}

// RK4 Helper Function
// Models acceleration relative to the Center of Gravity
[[gnu::pure]] static vector3 f_v_rel_cog(double t, vector3 self_pos, vector3 other_pos, double mass_other, double mass_self){

    vector3 r_vec = subtract_vec3s(self_pos, other_pos);

    double r = sqrt((r_vec.x * r_vec.x) + (r_vec.y * r_vec.y) + (r_vec.z * r_vec.z));

    const float epsilon = 1e3;
    if (r < epsilon) {
        printf("\nObjects probably collided... Closing Simulation\n");
        exit(0);
    }

    // Gravitational parameter μ = G * (m1 + m2)
    double u = standard_gravitational_parameter(mass_self, mass_other);

    // Acceleration vector: -(u') / r^3 * r_vec
    double scale = (-1) * (u) / (r * r * r);

    vector3 acceleration = scale_vec3(r_vec, scale);

    return acceleration;
}

// RK4 function
// Uses the center of gravity between two objects as the reference point
void cog_ref_runge_kutta_3d(double t, double h, body_3d *body1, body_3d *body2){

    // pos, vel, and mass of b1
    vector3 x1_init = body1->pos;
    vector3 v1_init = body1->velocity; 
    double m1 =  body1->mass;

    // pos, vel, and mass of b2
    vector3 x2_init = body2->pos;
    vector3 v2_init = body2->velocity; 
    double m2 =  body2->mass;


    //frame conversion
    vector3 v_com = {
    (m1 * v1_init.x + m2 * v2_init.x) / (m1 + m2),
    (m1 * v1_init.y + m2 * v2_init.y) / (m1 + m2),
    (m1 * v1_init.z + m2 * v2_init.z) / (m1 + m2)
    };

    
    vector3 com = find_cog(m1, x1_init, m2, x2_init);

    vector3 x1 = subtract_vec3s(x1_init, com);
    vector3 x2 = subtract_vec3s(x2_init, com);

    vector3 v1 = subtract_vec3s(v1_init, v_com);
    vector3 v2 = subtract_vec3s(v2_init, v_com);

    //rk4 steps

    //k1 step
    vector3 k1_x1 = f_x(t,x1,v1); // explicit euler. equal to k1 = h * f_x
    vector3 k1_v1 = f_v_rel_cog(t,x1,x2,m2,m1);
    vector3 k1_x2 = f_x(t,x2,v2); 
    vector3 k1_v2 = f_v_rel_cog(t,x2,x1,m1,m2);

    //k2
    vector3 k2_x1 = f_x(t + h * 0.5, add_vec3s(x1, scale_vec3(k1_x1, h * 0.5)), add_vec3s(v1, scale_vec3(k1_v1, h * 0.5))); //multiply by 0.5 is faster than /2
    vector3 k2_v1 = f_v_rel_cog(t + h * 0.5, add_vec3s(x1, scale_vec3(k1_x1, h * 0.5)), add_vec3s(x2, scale_vec3(k1_x2, h * 0.5)),m2,m1);
    vector3 k2_x2 = f_x(t + h * 0.5, add_vec3s(x2, scale_vec3(k1_x2, h * 0.5)), add_vec3s(v2, scale_vec3(k1_v2, h * 0.5))); //multiply by 0.5 is faster than /2
    vector3 k2_v2 = f_v_rel_cog(t + h * 0.5, add_vec3s(x2, scale_vec3(k1_x2, h * 0.5)), add_vec3s(x1, scale_vec3(k1_x1, h * 0.5)),m1,m2);

    vector3 k3_x1 = f_x(t + h * 0.5, add_vec3s(x1, scale_vec3(k2_x1, h * 0.5)), add_vec3s(v1, scale_vec3(k2_v1, h * 0.5)));
    vector3 k3_v1 = f_v_rel_cog(t + h * 0.5, add_vec3s(x1, scale_vec3(k2_x1, h * 0.5)), add_vec3s(x2, scale_vec3(k2_x2, h * 0.5)),m2,m1);
    vector3 k3_x2 = f_x(t + h * 0.5, add_vec3s(x2, scale_vec3(k2_x2, h * 0.5)), add_vec3s(v2, scale_vec3(k2_v2, h * 0.5)));
    vector3 k3_v2 = f_v_rel_cog(t + h * 0.5, add_vec3s(x2, scale_vec3(k2_x2, h * 0.5)), add_vec3s(x1, scale_vec3(k2_x1, h * 0.5)),m1,m2);

    vector3 k4_x1 = f_x(t + h, add_vec3s(x1, scale_vec3(k3_x1, h)), add_vec3s(v1, scale_vec3(k3_v1, h))); // implicit euler
    vector3 k4_v1 = f_v_rel_cog(t + h, add_vec3s(x1, scale_vec3(k3_x1, h)), add_vec3s(x2, scale_vec3(k3_x2, h)),m2,m1);
    vector3 k4_x2 = f_x(t + h, add_vec3s(x2, scale_vec3(k3_x2, h)), add_vec3s(v2, scale_vec3(k3_v2, h))); 
    vector3 k4_v2 = f_v_rel_cog(t + h, add_vec3s(x2, scale_vec3(k3_x2, h)), add_vec3s(x1, scale_vec3(k3_x1, h)),m1,m2);

    // body1->pos = add_vec3s(x1, scale_vec3(add_4vec3s(k1_x1, scale_vec3(k2_x1, 2), scale_vec3(k3_x1, 2), k4_x1), h / 6.0));
    // body1->velocity = add_vec3s(v1, scale_vec3(add_4vec3s(k1_v1, scale_vec3(k2_v1, 2), scale_vec3(k3_v1, 2), k4_v1), h / 6.0));

    // body2->pos = add_vec3s(x2, scale_vec3(add_4vec3s(k1_x2, scale_vec3(k2_x2, 2), scale_vec3(k3_x2, 2), k4_x2), h / 6.0));
    // body2->velocity = add_vec3s(v2, scale_vec3(add_4vec3s(k1_v2, scale_vec3(k2_v2, 2), scale_vec3(k3_v2, 2), k4_v2), h / 6.0));

    vector3 x1_final = add_vec3s(x1, scale_vec3(add_4vec3s(k1_x1, scale_vec3(k2_x1, 2), scale_vec3(k3_x1, 2), k4_x1), h / 6.0));
    vector3 v1_final = add_vec3s(v1, scale_vec3(add_4vec3s(k1_v1, scale_vec3(k2_v1, 2), scale_vec3(k3_v1, 2), k4_v1), h / 6.0));

    vector3 x2_final = add_vec3s(x2, scale_vec3(add_4vec3s(k1_x2, scale_vec3(k2_x2, 2), scale_vec3(k3_x2, 2), k4_x2), h / 6.0));
    vector3 v2_final = add_vec3s(v2, scale_vec3(add_4vec3s(k1_v2, scale_vec3(k2_v2, 2), scale_vec3(k3_v2, 2), k4_v2), h / 6.0));

    // Convert back into world frame
    body1->pos = add_vec3s(x1_final, com);
    body2->pos = add_vec3s(x2_final, com);

    body1->velocity = add_vec3s(v1_final, v_com);
    body2->velocity = add_vec3s(v2_final, v_com);


}
