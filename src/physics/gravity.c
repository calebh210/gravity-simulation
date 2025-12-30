#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <glad.h>
#include <unistd.h>
#include <GLFW/glfw3.h>
#include "math/math_funcs.h"
#include "math/vector/vector2.h"
#include "math/vector/vector4.h"
#include "math/matrix/matrix4.h"
#include "physics/gravity.h"
#include "physics/cr3bp.h"
#include "utils/shaders_parser.h"

// find the center of gravity from a N-body system
vector2 find_nbody_cog(body_2d* bodies[], int num_bodies){
    
    // sum up the mass of the bodies
    double total_m = 0;
    vector2 pos = {0.0, 0.0};
    for(int i = 0; i < num_bodies; i++){
        total_m += bodies[i]->mass;
        vector2 w = scale_vec2(bodies[i]->pos, bodies[i]->mass);
        pos = add_vec2s(pos, w);
    }

    pos = scale_vec2(pos, (1 / total_m));

    return pos;
}

// Function to draw a circle at (cx, cy) with radius 
double* drawCircle(vector2 c, float r, int num_segments) {

    // Center of circle
    double cx = c.x;
    double cy = c.y;

    int num_vertices = (num_segments+1) * 3;

    int idx = 0;

    double *vertices = malloc(num_vertices * sizeof(double));
    
    for (int i = 0; i <= num_segments; i++) {
        float angle = 2.0f * PI * i / num_segments;
        double x = cx + cosf(angle) * r;
        double y = cy + sinf(angle) * r;

        vertices[idx] = x;
        vertices[idx+1] = y;
        vertices[idx+2] = 0.0;

        idx += 3;

    }

    return vertices;

}

void initBodies(body_2d* bodies_array[], int num_bodies){
  
    for(int i=0; i < num_bodies ;i++){

        body_2d* b = bodies_array[i];

        // Init the orbit list for each body
        bodies_array[i]->orbit = init_list();
        initOrbit(bodies_array[i]->orbit);

        glGenBuffers( 1, &b->vbo );
        glGenVertexArrays( 1, &b->vao );

        vector2 coords = normalize_vec2(b->pos,SPACE_MIN,SPACE_MAX);

        int num_segments = 30; // How many segments in da circles
        double* points = drawCircle(coords, normalize(b->radius,0,SPACE_MAX), num_segments);

        glBindBuffer( GL_ARRAY_BUFFER, b->vbo );
        glBufferData( GL_ARRAY_BUFFER, num_segments * 3 * sizeof( double ), points, GL_STATIC_DRAW );

        glBindVertexArray( b->vao );
        glEnableVertexAttribArray( 0 );
        glBindBuffer( GL_ARRAY_BUFFER, b->vbo );
        glVertexAttribPointer( 0, 3, GL_DOUBLE, GL_FALSE, 0, NULL );
    
    }

}

void initOrbit(points_list* orbit){

    //this can overflow
    int ORBIT_LIMIT = 10000;
    
    glGenBuffers( 1, &orbit->vbo );
    glGenVertexArrays( 1, &orbit->vao );

    glBindVertexArray(orbit->vao);
    glBindBuffer(GL_ARRAY_BUFFER, orbit->vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vector3) * ORBIT_LIMIT,
                 NULL, GL_DYNAMIC_DRAW);

    // Consider a way to use GL_FLOAT to speed this up
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vector3), (void*)0);
    glEnableVertexAttribArray(0);
}

void updateOrbits(points_list* orbit, vector3 coords){

    //this can overflow
    int ORBIT_LIMIT = 10000;

    point *new_point = ( point * )malloc(sizeof(point));
    new_point->pos = coords;
    new_point->next = NULL;

    add_to_list(orbit, new_point);

    //Check the buffer isn't being overflown
    // Stop drawing for now, eventually overwrite old ones
    if(orbit->count >= ORBIT_LIMIT){
        return;
    }

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, orbit->vbo);
    // Add data to the VBO
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vector3) * (orbit->count - 1), sizeof(vector3), &new_point->pos);

}

//trying to draw an orbit path
void drawOrbit(const points_list* orbit){
    glBindVertexArray(orbit->vao);
    glBindBuffer(GL_ARRAY_BUFFER, orbit->vbo);
    glDrawArrays(GL_POINTS, 0, orbit->count);
}


//https://antongerdelan.net/opengl/hellotriangle.html
int render(body_2d* bodies_array[], int REF_FRAME_CODE, float timeskip, int num_bodies, bool debug) {


    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 1;
    }

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    GLFWwindow* window = glfwCreateWindow(1600, 900, "OpenGL Window", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);        

    if (!gladLoadGL()) {
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

    GLuint vs = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vs, 1, (const GLchar**)&vertex_shader, NULL );
    glCompileShader( vs );
    GLuint fs = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fs, 1, (const GLchar**)&fragment_shader, NULL );
    glCompileShader( fs );

    GLuint shader_program = glCreateProgram();
    glAttachShader( shader_program, fs );
    glAttachShader( shader_program, vs );
    glLinkProgram( shader_program );

    // The orbit shaders
    // I should maybe tuck this into a function eventually
    const char* orbit_frag = "shaders/orbit.frag";
    char* orbit_frag_shader = parse_shader_file(orbit_frag);

    const char* orbit_vertex = "shaders/orbit.vert";
    char* orbit_vert_shader = parse_shader_file(orbit_vertex);

    GLuint orbit_vs = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( orbit_vs, 1, (const GLchar**)&orbit_vert_shader, NULL );
    glCompileShader( orbit_vs );

    GLuint orbit_fs = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( orbit_fs, 1, (const GLchar**)&orbit_frag_shader, NULL );
    glCompileShader( orbit_fs );

    GLuint orbit_shader = glCreateProgram();
    glAttachShader( orbit_shader, orbit_fs );
    glAttachShader( orbit_shader, orbit_vs );
    glLinkProgram( orbit_shader );


    //Projection Matrix
    // This should be useful for 3D (when we get there!)
    float left = -1.0f, right = 1.0f;
    float bottom = -1.0f, top = 1.0f;
    float nearPlane = -1.0f, farPlane = 1.0f;

    float projection[16] = {
        2.0f/(right-left), 0.0f, 0.0f, 0.0f,                       
        0.0f, 2.0f/(top-bottom), 0.0f, 0.0f,                      
        0.0f, 0.0f, -2.0f/(farPlane-nearPlane), 0.0f,            
        -(right+left)/(right-left), -(top+bottom)/(top-bottom), -(farPlane+nearPlane)/(farPlane-nearPlane), 1.0f 
    };

    // for bodies
    GLuint projLoc = glGetUniformLocation(shader_program, "projection");
    glUseProgram(shader_program);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);

    //for orbits
    GLuint projLoc2 = glGetUniformLocation(orbit_shader, "projection");
    glUseProgram(orbit_shader);
    glUniformMatrix4fv(projLoc2, 1, GL_FALSE, projection);

    float m[] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    //View Matrix (just an identity for now)
    GLuint viewLoc = glGetUniformLocation(shader_program, "view");
    glUseProgram(shader_program);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, m);

    //for orbits
    GLuint viewLoc2 = glGetUniformLocation(orbit_shader, "view");
    glUseProgram(orbit_shader);
    glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, m);

    // used to measure frametime
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    int run = 0;  

    //init the bodies here
    initBodies(bodies_array, num_bodies);

    // Setup the title
    char title[256];
    sprintf( title, "Simulation Window");
    glfwSetWindowTitle(window, title );


    // This is sorta-temp code while I figure out how I want to do the translations long-term
    vector2 init_bodies_pos[num_bodies];
    for(int i=0; i < num_bodies; i++){
        init_bodies_pos[i] = bodies_array[i]->pos;
    }

   
            
    // Render loop
    // -1 is defined as the infinite run condition
   while (!glfwWindowShouldClose(window) && (RUN_LIMIT == -1 || run <= RUN_LIMIT)) {

        // Frame timer
        double currentTime = glfwGetTime();
        nbFrames++;

        if ( currentTime - lastTime >= 1.0 && debug){ // If last prinf() was more than 1 sec ago
            // printf and reset timer
            // debug printf statements
            printf("\nCurrent Frame = %d", run);
            printf("\n%f ms/frame", 1000.0/(double)(nbFrames));
            printf("\nRendering With: %s", glGetString(GL_RENDERER));
            for(int i=0;i<num_bodies;i++){
                printf("\n B%d Velocity = {%f, %f}", i, bodies_array[i]->velocity.x, bodies_array[i]->velocity.y);
                printf("\n B%d Position = {%f, %f}", i, bodies_array[i]->pos.x, bodies_array[i]->pos.y);
            }

            // FPS Counter
            double fps = (double)nbFrames / 1.0;
            char tmp[256];
            sprintf( tmp, "FPS %.2lf", fps );
            glfwSetWindowTitle(window, tmp );

            nbFrames = 0;
            lastTime += 1.0;
        }

        if(REF_FRAME_CODE == 100){
            rk4_equation_of_motion(bodies_array[0], bodies_array[1], timeskip);
            // vector2 cent_of_m = find_cog(body1->mass, body1->pos, body2->mass, body2->pos);
        }else if(REF_FRAME_CODE == 101){
           rk4_relative_equation_of_motion(bodies_array[0], bodies_array[1], timeskip);
            // vector2 cent_of_m = find_cog(body1->mass, body1->pos, body2->mass, body2->pos);

        }else if(REF_FRAME_CODE == 102){
            relative_equation_of_motion(bodies_array[0], bodies_array[1], timeskip);
            // vector2 cent_of_m = find_cog(body1->mass, body1->pos, body2->mass, body2->pos);

        }else if(REF_FRAME_CODE == 103){
            solve_cr3bp(bodies_array[0], bodies_array[1], bodies_array[2], timeskip);

        }else if(REF_FRAME_CODE == 200){
            rk4_nbody(0, timeskip, bodies_array, num_bodies);

            // vector2 cent_of_m = find_nbody_cog(bodies_array, num_bodies);

        }else{
            
            exit(1); // should never be reached
        }


        // Update window events.
        glfwPollEvents();
        
        // Wipe the drawing surface clear.
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // Put the shader program, and the VAO, in focus in OpenGL's state machine.
        glUseProgram( shader_program ); 

        // Iterate through all bodies and get their VAOs
        for(int i=0;i<num_bodies;i++){ 

            body_2d* b = bodies_array[i];

            int modelLocationBody = glGetUniformLocation(shader_program, "model");
            int modelLocationOrbit = glGetUniformLocation(orbit_shader, "model");

            if (modelLocationOrbit == -1){
                printf("failed to get model from orbit shader");
            }

            // orbits updating
            // use a vector3 for future-proofing, but set z to 0.0f cause its 2d

            // Completely arbitrary number I made up to not tank framerate with the orbit lines
            // This variable is probably poorly named, but it essentially puts a line in the orbit path once every N frames
            // where N is the ORBIT_SAMPLING var
            int ORBIT_SAMPLING = 500;
            if(run % ORBIT_SAMPLING == 0){
                vector2 coord_pos = normalize_vec2(b->pos, SPACE_MIN, SPACE_MAX);
                vector3 coord = {coord_pos.x, coord_pos.y, 0.0f};
                updateOrbits(b->orbit, coord);

            }
            matrix4 identity = {
                {1.0, 0.0, 0.0, 0.0},
                {0.0, 1.0, 0.0, 0.0},
                {0.0, 0.0, 1.0, 0.0},
                {0.0, 0.0, 0.0, 1.0}
            };

            glUseProgram( orbit_shader );
            glUniformMatrix4fv(modelLocationOrbit, 1, GL_TRUE, (const GLfloat *)identity);
            drawOrbit(b->orbit);
            

            // So these transformations use relative coords, so I'm storing the init positions then just subtracting from current
            // This way I don't have the re-write VBOs and its faster, but it feels...ugly
            vector2 n_pos = normalize_vec2(subtract_vec2s(b->pos,init_bodies_pos[i]), SPACE_MIN, SPACE_MAX);

            matrix4 m = {
                {1.0, 0.0, 0.0, n_pos.x},
                {0.0, 1.0, 0.0, n_pos.y},
                {0.0, 0.0, 1.0, 0.0},
                {0.0, 0.0, 0.0, 1.0}
            };

            glUseProgram( shader_program );
            glUniformMatrix4fv(modelLocationBody, 1, GL_TRUE, (const GLfloat *)m);

            //get the color
            int objectColorLoc = glGetUniformLocation(shader_program, "objectColor");
            glUniform3f(objectColorLoc, b->color.r, b->color.g, b->color.b);

            glBindVertexArray( bodies_array[i]->vao );
            glDrawArrays(GL_TRIANGLE_FAN, 0, 30);
        }
        
        glfwSwapBuffers( window);
        
        run++;
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    // glDeleteBuffers(1, &vbo);
    // glDeleteVertexArrays(1, &vao);

    for(int i = 0; i < num_bodies; i++){
        free_list(bodies_array[i]->orbit);
    }

    puts("\nSimulation Ending...");
    return EXIT_SUCCESS;
}


// equation of motion in reference frame attached to b1, where mass b1 >> b2
// still using 2d equation
void relative_equation_of_motion( body_2d *b1,  body_2d *b2, float delta_t){

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

void rk4_equation_of_motion( body_2d *b1, body_2d *b2, float delta_t){

    coint_runge_kutta(0, delta_t, b1, b2);

    double r = sqrt(((b1->pos.x - b2->pos.x)*(b1->pos.x - b2->pos.x)) + ((b1->pos.y - b2->pos.y)*(b1->pos.y - b2->pos.y)));

    if (r <= b1->radius + b2->radius){
        printf("\n B1 and B2 have collided. Ending simulation....");
        exit(0);
    }

}

//Find the COG, and run calcualtions relative to that
void rk4_relative_equation_of_motion( body_2d *b1, body_2d *b2, float delta_t){

    cog_ref_runge_kutta(0, delta_t, b1, b2);

    double r = sqrt(((b1->pos.x - b2->pos.x)*(b1->pos.x - b2->pos.x)) + ((b1->pos.y - b2->pos.y)*(b1->pos.y - b2->pos.y)));

    if (r <= b1->radius + b2->radius){
        printf("\n B1 and B2 have collided. Ending simulation....");
        exit(0);
    }
}

