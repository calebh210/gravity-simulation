#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "constants.h"
// #include "graphics.c"
#include "math_funcs.c"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

double gravity_equation(double m1, double m2, double r);
double gravity_acceleration(double M, double r);


struct two_d_vector
{
    double x; // this may need to NOT be an int in the future, but idk rn
    double y;
};

struct two_d_body
{
    double mass; // in kg
    struct two_d_vector pos;
    struct two_d_vector velocity;
};

struct two_d_vector* equation_of_motion(struct two_d_body *b1, struct two_d_body *b2);

// Function to draw a circle at (cx, cy) with radius r
void drawCircle(float cx, float cy, float r, int num_segments) {
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

int render(struct two_d_body* body1, struct two_d_body* body2) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    GLFWwindow* window = glfwCreateWindow(1280, 1000, "OpenGL Window", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

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

    int run = 0;

    double x_min = -5000E4;
    double x_max = 5000E4;

    double y_min = -5000E4;
    double y_max = 5000E4;


    // Render loop
    
    while (!glfwWindowShouldClose(window) && run <= RUN_LIMIT) {

        float normalized_pos[4];

        normalized_pos[0] = normalize(body1->pos.x, x_min, x_max);
        normalized_pos[1] = normalize(body1->pos.y, y_min, y_max);
        normalized_pos[2] = normalize(body2->pos.x, x_min, x_max);
        normalized_pos[3] = normalize(body2->pos.y, y_min, y_max);


        //normalize the center of mass
        // float com_x = normalize(barycenter->x, x_min, x_max);
        // float com_y = normalize(barycenter->y, y_min, y_max);

        struct two_d_vector *cent_of_m = equation_of_motion(body1, body2);

        cent_of_m->x = normalize(cent_of_m->x, x_min, x_max);
        cent_of_m->y = normalize(cent_of_m->y, y_min, y_max);
        
        glfwWaitEventsTimeout(0.016);

        // Clear the screen
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Mass 1
        glColor3f(1.0f, 0.5f, 0.2f); 
        drawCircle(normalized_pos[0], normalized_pos[1], 0.02f, 100);

        // Mass 2
        glColor3f(0.2f, 0.7f, 1.0f);  
        drawCircle(normalized_pos[2], normalized_pos[3], 0.02f, 100);

        // Center of Mass
        glColor3f(0.2f, 1.0f, 0.3f);  
        drawCircle(cent_of_m->x, cent_of_m->y, 0.005f, 100);


        glfwSwapBuffers(window);

        glfwPollEvents();

        run++;
        
        free(cent_of_m);

    }

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
struct two_d_vector* equation_of_motion(struct two_d_body *b1, struct two_d_body *b2){
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


    // printf("r before sqrt: %lf",r);


    if(isnan(r)){
        printf("ERROR: r IS NAN. ABORTING...\n");
        exit(1);
    }
    printf("r is = %lf \n", r);

    // double dot is accel
    // dot is velocity

    // Solving

    double ddx_1_0 = G * m2 * (x2 - x1)/(r * r * r); // double dot x for b1, the _0 indicates initial position
    double ddy_1_0 = G * m2 * (y2 - y1)/(r * r * r);

    double ddx_2_0 = G * m1 * (x1 - x2)/(r * r * r);
    double ddy_2_0 = G * m1 * (y1 - y2)/(r * r * r);

    printf("Initial acceleration of b1 in X = %lf \n", ddx_1_0);
    printf("Initial acceleration of b1 in Y = %lf \n", ddy_1_0);

    printf("Initial acceleration of b2 in X = %lf \n", ddx_2_0);
    printf("Initial acceleration of b2 in Y = %lf \n", ddy_2_0);

    double delta_t = 0.1F;

    // calculate the current velocities
    double dx_1_1 = ddx_1_0 * delta_t + dx_1_0;
    double dy_1_1 = ddy_1_0 * delta_t + dy_1_0;
    double dx_2_1 = ddx_2_0 * delta_t + dx_2_0;
    double dy_2_1 = ddy_2_0 * delta_t + dy_2_0;


    printf("Velocity X = %lf\n", dx_1_1);
    printf("Velocity Y = %lf\n", dy_1_1);
    printf("Velocity X = %lf\n", dx_2_1);
    printf("Velocity Y = %lf\n", dy_2_1);


    // calculate positions
    //CHECK THIS EQUATION
    double x_1_1 = dx_1_0 * delta_t + x1;
    double y_1_1 = dy_1_0 * delta_t + y1;
    double x_2_1 = dx_2_0 * delta_t + x2;
    double y_2_1 = dy_2_0 * delta_t + y2;


    printf("m1 Position X = %lf\n", x_1_1);
    printf("m1 Position Y = %lf\n", y_1_1);
    printf("m2 Position X = %lf\n", x_2_1);
    printf("m2 Position Y = %lf\n", y_2_1);


    b1->pos.x = x_1_1;
    b1->pos.y = y_1_1;
    b1->velocity.x = dx_1_1;
    b1->velocity.y = dy_1_1;

    b2->pos.x = x_2_1;
    b2->pos.y = y_2_1;
    b2->velocity.x = dx_2_1;
    b2->velocity.y = dy_2_1;

    struct two_d_vector *barycenter = (struct two_d_vector*) malloc(sizeof(struct two_d_vector)); 
    //calculate the center of mass

    // idk why i have to split this up
    barycenter->x = ((m1 * x1) + (m2 * x2));
    barycenter->y = ((m1 * y1) + (m2 * y2));

    barycenter->x = barycenter->x / (m1 + m2);
    barycenter->y = barycenter->y / (m1 + m2);

    return barycenter;

}

int main(){

    struct two_d_body *body1 = (struct two_d_body*) malloc(sizeof(struct two_d_body)*2);

    struct two_d_body *body2 = (struct two_d_body*) malloc(sizeof(struct two_d_body)*2);

    // gravity_equation();

    double mass = 1.0E27;

    body1->mass = mass;
    body2->mass =  mass;

    //E3 to convert from KM to M
    // ORANGE IN SIM
    body1->pos.x = -30000E3;
    body1->pos.y = -30000E3;
    body1->velocity.x = 100E3;
    body1->velocity.y = 400E3;

    //BLUE IN SIM
    body2->pos.x = 30000E3;
    body2->pos.y = 30000E3;
    body2->velocity.x = -300E3;
    body2->velocity.y = -200E3;

    //just doing this temporarily
    // barycenter->x = 0;
    // barycenter->y = 0;
    //double distance = (double)AU;
    // gravity_acceleration(mass, distance);
    
    // gravity_equation(mass, mass, 43139309);




    return render(body1, body2);

}