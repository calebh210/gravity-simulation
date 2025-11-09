#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "math/math_funcs.h"
#include "math/vector/vector2.h"
#include "physics/gravity.h"
#include "physics/cr3bp.h"


double gravity_equation(double m1, double m2, double r);
double gravity_acceleration(double M, double r);


// find the center of gravity from a N-body system
vector2 find_nbody_cog(two_d_body* bodies[], int NUM_BODIES){
    
    // sum up the mass of the bodies
    double total_m = 0;
    vector2 pos = {0.0, 0.0};
    for(int i = 0; i < NUM_BODIES; i++){
        total_m += bodies[i]->mass;
        vector2 w = scale_vec2(bodies[i]->pos, bodies[i]->mass);
        pos = add_vec2s(pos, w);
    }

    pos = scale_vec2(pos, (1 / total_m));

    return pos;
}

// Function to draw a circle at (cx, cy) with radius 
void drawCircle(vector2 c, float r, int num_segments) {
    float cx = c.x;
    float cy = c.y;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);  // Center of circle

    for (int i = 0; i <= num_segments; i++) {
        float angle = 2.0f * PI * i / num_segments;
        float x = cx + cosf(angle) * r;
        float y = cy + sinf(angle) * r;
        glVertex2f(x, y);
    }

    glEnd();
}


//trying to draw an orbit path
//use while to iterate over the points in the linked list, and draw them
// this never clears the old points, and may cause memory issues
void drawOrbits(points_list* orbits_list[], int N){

    glBegin(GL_POINTS);

        glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        for(int i = 0; i < N; i++){

            points_list *orbit = orbits_list[i];

            point *iterator = orbit->head;  

            // A way to limit the length of the orbit trails 
            // int limit = iterator->count - 3000;
            int limit = 0; // setting it to 0 makes the trails last forever

            while(iterator != NULL){
                if(iterator->count - limit <= 0){
                    break;
                }
                glVertex2f(iterator->pos.x, iterator->pos.y);
                iterator = iterator->next;
            }
        }

    glEnd();
}

// Draw the bodies in bodies_array at their updated position
// N is the number of bodies
void drawBodies(two_d_body* bodies_array[], points_list* orbits_list[], int N){

    //used for normalization
    double min = -1.5 * AU;
    double max = 1.5 * AU;   

    for(int i=0; i < N;i++){
        glColor3f(0.8f, 0.7f, 0.1f);  
        vector2 coords = normalize_vec2(bodies_array[i]->pos,min,max);
        drawCircle(coords, normalize(bodies_array[i]->radius,min,max), 100);

        // updateOrbits
        // Since I've already calculated the coords here, it makes sense to update the orbit line here too
        updateOrbits(orbits_list[i], coords);
    }

}

void updateOrbits(points_list* orbits_list, vector2 coords){

    point *new_point = ( point * )malloc(sizeof(point));
    new_point->pos = coords;
    add_to_list(orbits_list, new_point);

}

// Free the memory reserved in an orbits list
void freeOrbitsList(points_list* orbits_list[], int N){
    for(int i = 0; i < N; i++){
        free_list(orbits_list[i]);
    }

    puts("\nCorrectly freed all orbits list memory");
}

int render(two_d_body* bodies_array[], int REF_FRAME_CODE, float TIME_DELTA, int NUM_BODIES, bool DEBUG) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    GLFWwindow* window = glfwCreateWindow(1600, 900, "OpenGL Window", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);        
    if (!gladLoadGL()) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Set viewport and projection to match OpenGL's default -1 to 1 coordinate system
    glViewport(0, 0, 1600, 900);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);  // 2D orthographic projection
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    // used to measure frametime
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    int run = 0;  

    //list of points where the planet was previously. used to draw orbits
    points_list* orbits_list[NUM_BODIES];
    for(int i = 0; i < NUM_BODIES; i++){
        orbits_list[i] = init_list();
    }

    // Render loop
    // -1 is defined as the infinite run condition
   while (!glfwWindowShouldClose(window) && (RUN_LIMIT == -1 || run <= RUN_LIMIT)) {

        // Frame timer
        double currentTime = glfwGetTime();
        nbFrames++;

        if ( currentTime - lastTime >= 1.0 && DEBUG){ // If last prinf() was more than 1 sec ago
            // printf and reset timer
            // debug printf statements

            printf("\nCurrent Frame = %d", run);
            printf("\n%f ms/frame", 1000.0/(double)(nbFrames));
            printf("\nRendering With: %s", glGetString(GL_RENDERER));
            for(int i=0;i<NUM_BODIES;i++){
                printf("\n B%d Velocity = {%lf, %lf}", i, bodies_array[i]->velocity.x, bodies_array[i]->velocity.y);
                printf("\n B%d Position = {%lf, %lf}", i, bodies_array[i]->pos.x, bodies_array[i]->pos.y);
            }

            nbFrames = 0;
            lastTime += 1.0;
        }

        if(REF_FRAME_CODE == 100){
            rk4_equation_of_motion(bodies_array[0], bodies_array[1], TIME_DELTA);
            // vector2 cent_of_m = find_cog(body1->mass, body1->pos, body2->mass, body2->pos);
        }else if(REF_FRAME_CODE == 101){
           rk4_relative_equation_of_motion(bodies_array[0], bodies_array[1], TIME_DELTA);
            // vector2 cent_of_m = find_cog(body1->mass, body1->pos, body2->mass, body2->pos);

        }else if(REF_FRAME_CODE == 102){
            relative_equation_of_motion(bodies_array[0], bodies_array[1], TIME_DELTA);
            // vector2 cent_of_m = find_cog(body1->mass, body1->pos, body2->mass, body2->pos);

        }else if(REF_FRAME_CODE == 103){
            solve_cr3bp(bodies_array[0], bodies_array[1], bodies_array[2], TIME_DELTA);

        }else if(REF_FRAME_CODE == 200){
            rk4_nbody(0, TIME_DELTA, bodies_array, NUM_BODIES);

            // vector2 cent_of_m = find_nbody_cog(bodies_array, NUM_BODIES);

        }else{
            
            exit(1); // should never be reached
        }

        // Draw the bodies in OpenGL
        drawBodies(bodies_array, orbits_list, NUM_BODIES);

        // Draw the orbits in OpenGL
        drawOrbits(orbits_list, NUM_BODIES);

        glfwSwapBuffers(window);

        glfwPollEvents();

        //unreliable fps cap
        glfwWaitEventsTimeout(0.008);

        // Clear the screen
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        run++;
    }




    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    freeOrbitsList(orbits_list, NUM_BODIES);

    puts("\nSimulation Ending...");
    return EXIT_SUCCESS;
}


double gravity_equation(double m1, double m2, double r){
    double F; // gravitational force
    // long m1; // mass one in KG
    // long m2; // mass two in KG
    // double r; //distance between m1 and m2 in meters
    // G is defined in constant

    F = G * ((m1 * m2) / (r * r));

    printf("%.10lf \n", F); // holy decimal precision!

    return F;
}


// This function does g = GM/r^2
double gravity_acceleration(double M, double r){
    double g; // feel like this should be a double for edge cases

    g = (G * M) / (r * r);

    printf("%lf \n", g);
    
    return g;

}



// For simplicity, I'm implementing this in 2D for now, thus ignoring the Z axis
// this returns the center of mass between the objects as a 2d vector
void equation_of_motion( two_d_body *b1,  two_d_body *b2, float delta_t){
    //double R; //this is the absolute gravitational accel between the two, and our result

    double m1, m2; //mass of the object
    double x1, x2, y1, y2; //x and y positions of the object
    double dx_1_0, dy_1_0, dx_2_0, dy_2_0; //initial velocities 

    m1 = b1->mass;
    m2 = b2->mass;

    x1 = b1->pos.x;
    x2 = b2->pos.x;

    y1 = b1->pos.y;
    y2 = b2->pos.y;

    dx_1_0 = b1->velocity.x;
    dy_1_0 = b1->velocity.y; 

    dx_2_0 = b2->velocity.x;
    dy_2_0 = b2->velocity.y;


    // vector math !!
    // double r =  sqrt(abs(((x2 - x1) * (x2-x1))) + abs(((y2-y1) * (y2-y1))) );
    double r = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));


    if(isnan(r)){
        printf("ERROR: r IS NAN. ABORTING...\n");
        exit(1);
    }

    if(r < 1){
        printf("\n The objects collided");
        exit(0);
    }

    // double dot is accel
    // dot is velocity

    // Solving

    double ddx_1_0 = G * m2 * (x2 - x1)/(r * r * r); // double dot x for b1, the _0 indicates initial position
    double ddy_1_0 = G * m2 * (y2 - y1)/(r * r * r);

    double ddx_2_0 = G * m1 * (x1 - x2)/(r * r * r);
    double ddy_2_0 = G * m1 * (y1 - y2)/(r * r * r);


    //this is in seconds

    // calculate the current velocities
    double dx_1_1 = ddx_1_0 * delta_t + dx_1_0;
    double dy_1_1 = ddy_1_0 * delta_t + dy_1_0;
    double dx_2_1 = ddx_2_0 * delta_t + dx_2_0;
    double dy_2_1 = ddy_2_0 * delta_t + dy_2_0;


    // calculate positions
    //CHECK THIS EQUATION
    double x_1_1 = (dx_1_1 * delta_t) + x1;
    double y_1_1 = dy_1_1 * delta_t + y1;
    double x_2_1 = dx_2_1 * delta_t + x2;
    double y_2_1 = dy_2_1 * delta_t + y2;

    b1->pos.x = x_1_1;
    b1->pos.y = y_1_1;
    b1->velocity.x = dx_1_1;
    b1->velocity.y = dy_1_1;

    b2->pos.x = x_2_1;
    b2->pos.y = y_2_1;
    b2->velocity.x = dx_2_1;
    b2->velocity.y = dy_2_1;

}

// equation of motion in reference frame attached to b1, where mass b1 >> b2
// still using 2d equation
void relative_equation_of_motion( two_d_body *b1,  two_d_body *b2, float delta_t){

    double x = b2->pos.x;
    double y = b2->pos.y;
    //m is the mass of the stationary object, body1
    runge_kutta(0, delta_t, b1->mass , b2);

    double r = sqrt(((0.0 - x)*(0.0 - x)) + ((0.0 - y)*(0.0 - y))); // since this is relative frame, the other set would be (0)

    double alt = fabs(r) - b1->radius;
    if(alt < 0.0){
        printf("\nObject crashed into planet!");
        exit(0);
    }

}

void rk4_equation_of_motion( two_d_body *b1, two_d_body *b2, float delta_t){

    coint_runge_kutta(0, delta_t, b1, b2);

    double r = sqrt(((b1->pos.x - b2->pos.x)*(b1->pos.x - b2->pos.x)) + ((b1->pos.y - b2->pos.y)*(b1->pos.y - b2->pos.y)));

    if (r <= b1->radius + b2->radius){
        printf("\n B1 and B2 have collided. Ending simulation....");
        exit(0);
    }

}

//Find the COG, and run calcualtions relative to that
void rk4_relative_equation_of_motion( two_d_body *b1, two_d_body *b2, float delta_t){

    cog_ref_runge_kutta(0, delta_t, b1, b2);

    double r = sqrt(((b1->pos.x - b2->pos.x)*(b1->pos.x - b2->pos.x)) + ((b1->pos.y - b2->pos.y)*(b1->pos.y - b2->pos.y)));

    if (r <= b1->radius + b2->radius){
        printf("\n B1 and B2 have collided. Ending simulation....");
        exit(0);
    }
}


// take the mass (kg) of an object and determine its scharzchild radius
double scharzchild_radius(double mass){
    return (2 * G * mass) / (double)(c * c);
}
