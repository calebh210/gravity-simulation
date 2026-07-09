#include <stdlib.h>
#include <stdio.h>
#include <glad.h>
#include <GLFW/glfw3.h>
#include "graphics/render.h"
#include "body.h"
#include "graphics/orbits.h"
#include "graphics/shapes/circle.h"
#include "physics/gravity.h"
#include "physics/cr3bp.h"
#include "math/math_funcs.h"
#include "math/vector/vector2.h"
#include "math/matrix/matrix4.h"
#include "utils/shaders_parser.h"

// OpenGL reference: https://antongerdelan.net/opengl/hellotriangle.html
// Render Timestep reference: https://gafferongames.com/post/fix_your_timestep/

void initBodies(body_2d* bodies_array[], int num_bodies) {

    for(int i = 0; i < num_bodies; i++) {

        body_2d* b = bodies_array[i];

        // Init the orbit list for each body
        bodies_array[i]->orbit = initOrbit();

        glGenBuffers(1, &b->vbo);
        glGenVertexArrays(1, &b->vao);

        vector2 coords = normalize_vec2(b->pos, SPACE_MIN, SPACE_MAX);

        int num_segments = 30; // How many segments in da circles
        double* points =
            drawCircle(coords, normalize(b->radius, 0, SPACE_MAX), num_segments);

        glBindBuffer(GL_ARRAY_BUFFER, b->vbo);
        glBufferData(GL_ARRAY_BUFFER, num_segments * 3 * sizeof(double), points,
                     GL_STATIC_DRAW);

        glBindVertexArray(b->vao);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, b->vbo);
        glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, NULL);
    }
}

static void print_debug_stats(GLFWwindow* window, double currentTime,
                              double* lastTime, int* nbFrames, int run,
                              body_2d* bodies[], int num_bodies, bool debug) {
    (*nbFrames)++;

    if(currentTime - *lastTime >= 1.0 &&
       debug) { // If last printf() was more than 1 sec ago
        // printf and reset timer
        // debug printf statements
        printf("\nCurrent Frame = %d", run);
        printf("\n%f ms/frame", 1000.0 / (double)(*nbFrames));
        printf("\nRendering With: %s", glGetString(GL_RENDERER));
        for(int i = 0; i < num_bodies; i++) {
            printf("\n B%d Velocity = {%f, %f}", i, bodies[i]->velocity.x,
                   bodies[i]->velocity.y);
            printf("\n B%d Position = {%f, %f}", i, bodies[i]->pos.x,
                   bodies[i]->pos.y);
        }

        // FPS Counter
        double fps = (double)(*nbFrames) / 1.0;
        char tmp[256];
        sprintf(tmp, "FPS %.2lf", fps);
        glfwSetWindowTitle(window, tmp);

        *nbFrames = 0;
        *lastTime += 1.0;
    }
}

void step_physics_2d(body_2d* bodies[], int num_bodies,
                     enum REFERENCE_FRAME frame, float sim_dt) {
    if(frame == INERTIAL) {
        rk4_equation_of_motion(bodies[0], bodies[1], sim_dt);
        // vector2 cent_of_m = find_cog(body1->mass, body1->pos, body2->mass,
        // body2->pos);
    } else if(frame == RELATIVE) {
        rk4_relative_equation_of_motion(bodies[0], bodies[1], sim_dt);
        // vector2 cent_of_m = find_cog(body1->mass, body1->pos, body2->mass,
        // body2->pos);

    } else if(frame == CENTER_OF_GRAVITY) {
        relative_equation_of_motion(bodies[0], bodies[1], sim_dt);
        // vector2 cent_of_m = find_cog(body1->mass, body1->pos, body2->mass,
        // body2->pos);

    } else if(frame == CR3BP) {
        solve_cr3bp(bodies[0], bodies[1], bodies[2], sim_dt);

    } else if(frame == N_BODY) {
        rk4_nbody(0, sim_dt, bodies, num_bodies);

        // vector2 cent_of_m = find_nbody_cog(bodies_array, num_bodies);

    } else {

        exit(1); // should never be reached
    }
}

// Main render loop for 2D rendering
int render(body_2d* bodies_array[], enum REFERENCE_FRAME REF_FRAME,
           float timeskip, int num_bodies, bool debug) {

    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1600, 900, "OpenGL Window", NULL, NULL);
    if(!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if(!gladLoadGL()) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // SETUP THE SHADERS
    // yes I know it's hardcoded.... I don't care
    const char* fpath = "shaders/circle.vert";

    char* vertex_shader = parse_shader_file(fpath);

    const char* frag_path = "shaders/circle.frag";
    char* fragment_shader = parse_shader_file(frag_path);

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, (const GLchar**)&vertex_shader, NULL);
    glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, (const GLchar**)&fragment_shader, NULL);
    glCompileShader(fs);

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, fs);
    glAttachShader(shader_program, vs);
    glLinkProgram(shader_program);

    // Projection Matrix
    float left = -1.0f, right = 1.0f;
    float bottom = -1.0f, top = 1.0f;
    float nearPlane = -1.0f, farPlane = 1.0f;

    float projection[16] = {2.0f / (right - left),
                            0.0f,
                            0.0f,
                            0.0f,
                            0.0f,
                            2.0f / (top - bottom),
                            0.0f,
                            0.0f,
                            0.0f,
                            0.0f,
                            -2.0f / (farPlane - nearPlane),
                            0.0f,
                            -(right + left) / (right - left),
                            -(top + bottom) / (top - bottom),
                            -(farPlane + nearPlane) / (farPlane - nearPlane),
                            1.0f};

    // for bodies
    GLuint projLoc = glGetUniformLocation(shader_program, "projection");
    glUseProgram(shader_program);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);

    // View Matrix (just an identity for now)
    GLuint viewLoc = glGetUniformLocation(shader_program, "view");
    glUseProgram(shader_program);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE,
                       (const GLfloat*)MATRIX4_IDENTITY_MATRIX);

    GLuint orbit_shader = init_orbit_shaders();

    // for orbits
    GLuint viewLoc2 = glGetUniformLocation(orbit_shader, "view");
    GLuint projLoc2 = glGetUniformLocation(orbit_shader, "projection");
    glUseProgram(orbit_shader);
    glUniformMatrix4fv(viewLoc2, 1, GL_FALSE,
                       (const GLfloat*)MATRIX4_IDENTITY_MATRIX);
    glUniformMatrix4fv(projLoc2, 1, GL_FALSE, projection);

    // used to measure frametime
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    int run = 0;

    // init the bodies here
    initBodies(bodies_array, num_bodies);

    // Setup the title
    char title[256];
    sprintf(title, "Simulation Window");
    glfwSetWindowTitle(window, title);

    // This is sorta-temp code while I figure out how I want to do the
    // translations long-term
    vector2 init_bodies_pos[num_bodies];
    for(int i = 0; i < num_bodies; i++) {
        init_bodies_pos[i] = bodies_array[i]->pos;
    }

    // constants for decoupled render
    const double FIXED_DT = 1.0 / 240.0; // num physics steps per second
    const double MAX_FRAME_TIME = 0.25;
    double accumulator = 0.0;
    double previousTime = glfwGetTime();

    // Render loop
    // -1 is defined as the infinite run condition
    while(!glfwWindowShouldClose(window) &&
          (RUN_LIMIT == -1 || run <= RUN_LIMIT)) {

        // Frame timer
        double currentTime = glfwGetTime();

        print_debug_stats(window, currentTime, &lastTime, &nbFrames, run,
                          bodies_array, num_bodies, debug);

        // decoupled frame and physics rendering by using the physics stepper as a
        // consumer of dt
        double frameTime = currentTime - previousTime;
        previousTime = currentTime;
        if(frameTime > MAX_FRAME_TIME) {
            frameTime = MAX_FRAME_TIME;
        }
        accumulator += frameTime;

        while(accumulator >= FIXED_DT) {

            step_physics_2d(bodies_array, num_bodies, REF_FRAME, timeskip);
            accumulator -= FIXED_DT;
        }

        // Update window events.
        glfwPollEvents();

        // Wipe the drawing surface clear.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // Put the shader program, and the VAO, in focus in OpenGL's state machine.
        glUseProgram(shader_program);

        // Iterate through all bodies and get their VAOs
        for(int i = 0; i < num_bodies; i++) {

            body_2d* b = bodies_array[i];

            int modelLocationBody = glGetUniformLocation(shader_program, "model");
            int modelLocationOrbit = glGetUniformLocation(orbit_shader, "model");

            if(modelLocationOrbit == -1) {
                printf("failed to get model from orbit shader");
            }

            // orbits updating
            // use a vector3 for future-proofing, but set z to 0.0f cause its 2d
            // Completely arbitrary number I made up to not tank framerate with the
            // orbit lines This variable is probably poorly named, but it essentially
            // puts a line in the orbit path once every N frames where N is the
            // ORBIT_SAMPLING var
            int ORBIT_SAMPLING = 500;
            if(run % ORBIT_SAMPLING == 0) {
                vector2 coord_pos = normalize_vec2(b->pos, SPACE_MIN, SPACE_MAX);
                vector3 coord = {coord_pos.x, coord_pos.y, 0.0f};
                updateOrbits(b->orbit, coord);
            }

            glUseProgram(orbit_shader);
            glUniformMatrix4fv(modelLocationOrbit, 1, GL_TRUE,
                               (const GLfloat*)MATRIX4_IDENTITY_MATRIX);
            drawOrbit(b->orbit, orbit_shader);

            // So these transformations use relative coords, so I'm storing the init
            // positions then just subtracting from current This way I don't have the
            // re-write VBOs and its faster, but it feels...ugly
            vector2 n_pos = normalize_vec2(subtract_vec2s(b->pos, init_bodies_pos[i]),
                                           SPACE_MIN, SPACE_MAX);

            vector3 translation_pos = {n_pos.x, n_pos.y, 0.0f};

            matrix4 m = {1.0, 0.0, 0.0, 0, 0.0, 1.0, 0.0, 0,
                         0.0, 0.0, 1.0, 0.0, n_pos.x, n_pos.y, 0.0, 1.0};

            glUseProgram(shader_program);
            glUniformMatrix4fv(modelLocationBody, 1, GL_FALSE, (const GLfloat*)m);

            // get the color
            int objectColorLoc = glGetUniformLocation(shader_program, "objectColor");
            glUniform3f(objectColorLoc, b->color.r, b->color.g, b->color.b);

            glBindVertexArray(bodies_array[i]->vao);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 30);
        }

        glfwSwapBuffers(window);

        run++;
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    // glDeleteBuffers(1, &vbo);
    // glDeleteVertexArrays(1, &vao);

    for(int i = 0; i < num_bodies; i++) {
        free_list(bodies_array[i]->orbit->points);
    }

    puts("\nSimulation Ending...");
    return 0;
}
