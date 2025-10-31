#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "math/math_funcs.h"
#include "physics/gravity.h"
#include "physics/cr3bp.h"


double gravity_equation(double m1, double m2, double r);
double gravity_acceleration(double M, double r);

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
void drawOrbits(points_list *orbit){
    glBegin(GL_POINTS);

        glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        point *iterator = orbit->head;  
        while(iterator != NULL){
            glVertex2f(iterator->pos.x, iterator->pos.y);
            iterator = iterator->next;
        }

    glEnd();
}

int render(two_d_body* bodies_array[], int REF_FRAME_CODE, float TIME_DELTA, bool DEBUG) {
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

    //used for normalization
    double min = -1.5 * AU;
    double max = 1.5 * AU;    


    //list of points where the planet was previously. used to draw orbits
    int num_bodies = 2;
    points_list* orbits_list[num_bodies];
    for(int i = 0; i < 2; i++){
        orbits_list[i] = init_list();
    }


    // Temp code while I figure out the best way to do the body arrays

    two_d_body* body1 = bodies_array[0];
    two_d_body* body2 = bodies_array[1];

    // Render loop
    // -1 is defined as the infinite run condition
   while (!glfwWindowShouldClose(window) && (RUN_LIMIT == -1 || run <= RUN_LIMIT)) {


        // Frame timer
        double currentTime = glfwGetTime();
        nbFrames++;

        if ( currentTime - lastTime >= 1.0 && DEBUG){ // If last prinf() was more than 1 sec ago
            // printf and reset timer
            // debug printf statements
            double r = sqrt(((body1->pos.x - body2->pos.x)*(body1->pos.x - body2->pos.x)) + ((body1->pos.y - body2->pos.y)*(body1->pos.y - body2->pos.y)));

            printf("\n Current Frame = %d", run);
            printf("\n%f ms/frame", 1000.0/(double)(nbFrames));
            printf("\n Distance between B1 and B2 = %lf", r);
            printf("\n B1 Velocity = {%lf, %lf}", body1->velocity.x, body1->velocity.y);
            printf("\n B1 Position = {%lf, %lf}", body1->pos.x, body1->pos.y);
            printf("\n B2 Velocity = {%lf, %lf}", body2->velocity.x, body2->velocity.y);
            printf("\n B2 Position = {%lf, %lf}", body2->pos.x, body2->pos.y);
            nbFrames = 0;
            lastTime += 1.0;
        }


        if(REF_FRAME_CODE == 100){
            rk4_equation_of_motion(body1, body2, TIME_DELTA);

        }else if(REF_FRAME_CODE == 101){
           rk4_relative_equation_of_motion(body1, body2, TIME_DELTA);

        }else if(REF_FRAME_CODE == 102){
            relative_equation_of_motion(body1, body2, TIME_DELTA);

        }else if(REF_FRAME_CODE == 103){
            two_d_body* t = bodies_array[2];
            solve_cr3bp(body1, body2, t, TIME_DELTA);

            //the third body in the CR3BP
            glColor3f(0.3f, 0.7f, 1.0f);  
            vector2 m2_grid_coords = normalize_vec2(t->pos,min,max);
            drawCircle(m2_grid_coords, 0.01f, 100);

        }else{
            
            exit(1); // should never be reached
        }

        vector2 cent_of_m = find_cog(body1->mass, body1->pos, body2->mass, body2->pos);


        // vector2 *cent_of_m = equation_of_motion(body1, body2, 100000.0f);

        // Mass 1
        glColor3f(1.0f, 0.5f, 0.2f); 
        vector2 m1_grid_coords = normalize_vec2(body1->pos, min, max);
        drawCircle(m1_grid_coords, normalize(body1->radius,min, max), 100); //add smth to normalize size of bodies
        // drawCircle(normalized_pos[0], normalized_pos[1], 0.03f, 100);

        // Mass 2
        glColor3f(0.2f, 0.7f, 1.0f);  
        vector2 m2_grid_coords = normalize_vec2(body2->pos,min,max);
        drawCircle(m2_grid_coords, normalize(2 * body2->radius,min, max), 100);
        // drawCircle(normalized_pos[2], normalized_pos[3], 0.02f, 100);

        point *new_point = ( point * )malloc(sizeof(point));
        new_point->pos = m1_grid_coords;
        add_to_list(orbits_list[0], new_point);

        point *new_point2 = ( point * )malloc(sizeof(point));
        new_point2->pos = m2_grid_coords;
        add_to_list(orbits_list[1], new_point2);

        drawOrbits(orbits_list[0]);
        drawOrbits(orbits_list[1]);

        // Center of Mass
        cent_of_m = normalize_vec2(cent_of_m, min, max);
        glColor3f(0.2f, 1.0f, 0.3f);  
        drawCircle(cent_of_m, 0.005f, 100);


        glfwSwapBuffers(window);

        glfwPollEvents();

        run++;

        //unreliable fps cap
        glfwWaitEventsTimeout(0.008);

        // Clear the screen
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
    }

    free_list(orbits_list[0]);
    free_list(orbits_list[1]);

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();

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
