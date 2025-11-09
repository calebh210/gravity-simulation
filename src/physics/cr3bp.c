#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "math/math_funcs.h"
#include "physics/gravity.h"
#include "utils/structures.h"

//calculate angular velocity of cog ref frame between two bodies
// double angular_velocity(two_d_body *b1, two_d_body *b2){
//     double u = standard_gravitational_parameter(b1->mass, b2->mass);
//     double r = vec2_distance_between(b1->pos, b2->pos);
//     return sqrt(u / pow(r, 3));
// }

//calculates relative x coords in the CR3BP
// double cr3bp_x_coords(two_d_body *b1, two_d_body *b2){
//     double ratio1 = b1->mass / (b1->mass + b2->mass);
//     double ratio2 = 1 - ratio1;
//     double x1 = -ratio2 * r;
//     double x2 = ratio1 * r;
// }



/* t is the tertiary mass in the system
This ONLY solves the position and velocity of T
I still need to render the the equation of motion to update b1 and b2
*/
void solve_cr3bp(two_d_body *b1, two_d_body *b2, two_d_body *t, double h){

    // //get local ones to paly with
    // two_d_body *b1 = init_b1;
    // two_d_body *b2 = init_b2;


    // get angular velocity
    printf("The masses %lf, %lf \n", b1->mass, b2->mass);
    double u = standard_gravitational_parameter(b1->mass, b2->mass); // this is the problem
    printf("u = %lf\n", u);
    double r1_2 = vec2_distance_between(b1->pos, b2->pos);
    printf("vector r1_2 = %lf\n",r1_2);

    double angular_velocity = sqrt(u / pow(r1_2, 3));
    printf("Angular velocity = %lf\n", angular_velocity);
    // get x coords relative to the CoG
    double ratio1 = b1->mass / (b1->mass + b2->mass);
    double ratio2 = 1 - ratio1;
    
    /*  
    Define the tertiary mass (m) such that m.mass = 1 
    Define the position of m relative to the barycenter as:
    r = xi + yk + zk
    */

    //frame conversion
    vector2 com = find_cog(b1->mass,b1->pos,b2->mass, b2->pos);

    b1->pos.x -= com.x;
    b1->pos.y -= com.y;
    b2->pos.x -= com.x;
    b2->pos.y -= com.y;
    t->pos.x  -= com.x;
    t->pos.y  -= com.y;



    //NOTE this can be put into its own function
    // Calculate the position of the tertiary mass t from larger mass m1
    vector2 r_1;
    r_1.x = t->pos.x + (ratio2 * r1_2);
    r_1.y = t->pos.y;

    // Calculate the position of the tertiary mass t from larger mass m1
    vector2 r_2;
    r_2.x = t->pos.x - (ratio1 * r1_2);
    r_2.y = t->pos.y;

    // The scalar equations of motion for the tertiary body in the CR3BP are ugly

    double r1_mag = magnitude_vec2(r_1);
    double r2_mag = magnitude_vec2(r_2);

    // Acceleration of t due to m1 and m2 in x dir
    double ddx = 2*angular_velocity*t->velocity.y 
                + pow(angular_velocity, 2)*t->pos.x
                - (G*b1->mass * r_1.x)/pow(r1_mag,3)
                - (G*b2->mass * r_2.x)/pow(r2_mag,3);

    // Acceleration of t due to m1 and m2 in y dir
    double ddy = -2*angular_velocity*t->velocity.x
                + pow(angular_velocity, 2)*t->pos.y
                - (G*b1->mass * r_1.y)/pow(r1_mag,3)
                - (G*b2->mass * r_2.y)/pow(r2_mag,3);

    // Currently Euler integrating for simplicity

    double dx, dy; // The velocity of t after calculating ddx and ddy

    dx = (ddx * h) +  t->velocity.x;
    dy = (ddy * h) +  t->velocity.y;

    // Calculate new position
    t->pos.x = (dx * h) + t->pos.x;
    t->pos.y = (dy * h) + t->pos.y;

    t->velocity.x = dx;
    t->velocity.y = dy;


    printf("Tertiary Body Pos: %lf, %lf\n",t->pos.x, t->pos.y );
    printf("Tertiary Body Velocity: %lf, %lf\n",t->velocity.x, t->velocity.y );
    printf("Tertiary body distance from m1: %lf\n", r1_mag);
    printf("Tertiary body distance from m2: %lf\n", r2_mag);

}