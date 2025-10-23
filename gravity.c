#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "math_funcs.h"
#include "gravity.h"

double gravity_equation(double m1, double m2, double r);
double gravity_acceleration(double M, double r);


points_list* init_list(){

    points_list *pL = (points_list *)malloc(sizeof(points_list));
    if(pL == NULL){
        printf("uh oh");
        exit(1);
    }
    pL->head = NULL;
    return pL;
}

// add new point P to head of list
void add_to_list(points_list *pL, point *p){
    //check if the data is similar. if it's within a certain range, don't add it.
    p->next = pL->head;
    pL->head = p;    
}

void free_list(points_list *pL){
    point *iterator;   
    while(pL->head != NULL){
        iterator = pL->head;
        pL->head = pL->head->next;
        free(iterator);
    }
}

// Function to draw a circle at (cx, cy) with radius 
void drawCircle(two_d_vector c, float r, int num_segments) {
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

int render( two_d_body* body1,  two_d_body* body2, bool DEBUG) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    GLFWwindow* window = glfwCreateWindow(1280, 900, "OpenGL Window", NULL, NULL);
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
    glViewport(0, 0, 1280, 1000);
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


    //list of points in the planet was at. used to draw orbits
    int num_bodies = 2;
    points_list* orbits_list[num_bodies];
    for(int i = 0; i < 2; i++){
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

        two_d_vector *cent_of_m = rk4_equation_of_motion(body1, body2, 5000.0f);
        // two_d_vector *cent_of_m = equation_of_motion(body1, body2, 100000.0f);
        // two_d_vector *cent_of_m = relative_equation_of_motion(body1, body2, 10000.0f);
        // two_d_vector *cent_of_m = rk4_relative_equation_of_motion(body1, body2, 5000.0f);


        // Clear the screen
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Mass 1
        glColor3f(1.0f, 0.5f, 0.2f); 
        two_d_vector m1_grid_coords = normalize_vec2(body1->pos, min, max);
        drawCircle(m1_grid_coords, normalize(2 * body1->radius,min, max), 100); //add smth to normalize size of bodies
        // drawCircle(normalized_pos[0], normalized_pos[1], 0.03f, 100);

        // Mass 2
        glColor3f(0.2f, 0.7f, 1.0f);  
        two_d_vector m2_grid_coords = normalize_vec2(body2->pos,min,max);
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
        *cent_of_m = normalize_vec2(*cent_of_m, min, max);
        glColor3f(0.2f, 1.0f, 0.3f);  
        drawCircle(*cent_of_m, 0.005f, 100);


        glfwSwapBuffers(window);

        glfwPollEvents();

        run++;

        //unreliable fps cap
        glfwWaitEventsTimeout(0.008);
        
        free(cent_of_m);
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
two_d_vector* equation_of_motion( two_d_body *b1,  two_d_body *b2, float delta_t){
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

    two_d_vector *barycenter = find_cog(m1, b1->pos, m2, b2->pos);

    return barycenter;

}

// equation of motion in reference frame attached to b1, where mass b1 >> b2
// still using 2d equation
 two_d_vector* relative_equation_of_motion( two_d_body *b1,  two_d_body *b2, float delta_t){

    double x = b2->pos.x;
    double y = b2->pos.y;
    //m is the mass of the stationary object, body1
    runge_kutta(0, delta_t, b1->mass , b2);

    double r = sqrt(((0.0 - x)*(0.0 - x)) + ((0.0 - y)*(0.0 - y))); // since this is relative frame, the other set would be (0)

    double alt = fabs(r) - b1->radius;
    printf("\nAltitude is: %lf", alt);
    if(alt < 0.0){
        printf("\nObject crashed into planet!");
        exit(0);
    }

     two_d_vector *barycenter = find_cog(b1->mass, b1->pos, b2->mass, b2->pos);

    return barycenter;

}

two_d_vector* rk4_equation_of_motion( two_d_body *b1, two_d_body *b2, float delta_t){

    coint_runge_kutta(0, delta_t, b1, b2);

    double r = sqrt(((b1->pos.x - b2->pos.x)*(b1->pos.x - b2->pos.x)) + ((b1->pos.y - b2->pos.y)*(b1->pos.y - b2->pos.y)));

    if (r <= b1->radius + b2->radius){
        printf("\n B1 and B2 have collided. Ending simulation....");
        exit(0);
    }


    two_d_vector *barycenter = find_cog(b1->mass, b1->pos, b2->mass, b2->pos);

    return barycenter;
}

// Put the COG on 0,0, and run calcualtions relative to that
two_d_vector* rk4_relative_equation_of_motion( two_d_body *b1, two_d_body *b2, float delta_t){

    cog_ref_runge_kutta(0, delta_t, b1, b2);

    double r = sqrt(((b1->pos.x - b2->pos.x)*(b1->pos.x - b2->pos.x)) + ((b1->pos.y - b2->pos.y)*(b1->pos.y - b2->pos.y)));

    if (r <= b1->radius + b2->radius){
        printf("\n B1 and B2 have collided. Ending simulation....");
        exit(0);
    }


    //this is done just for visualization
    two_d_vector *barycenter = find_cog(b1->mass, b1->pos, b2->mass, b2->pos);

    return barycenter;
}


// take the mass (kg) of an object and determine its scharzchild radius
double scharzchild_radius(double mass){
    return (2 * G * mass) / (double)(c * c);
}
