#include "graphics/render3d.h"
#include "math/vector/vector3.h"
#include "math/vector/vector4.h"
#include "math/matrix/matrix4.h"
#include "physics/gravity3d.h"
#include "math/math_funcs.h"
#include "utils/shaders_parser.h"
#include <string.h>

vector3* drawSphere(vector3 s, float r, int NUM_SEGMENTS) {

    // Center of sphere
    float cx = s.x;
    float cy = s.y;
    float cz = s.z;

    int idx = 0;

    int n = NUM_SEGMENTS;

    vector3 *vertices = malloc(sizeof(vector3) * (13 * (n * n)));
    
    for (float i = 0.0f; i < n ; i++) {

        float theta = 3.141592f * (i / n);
        float theta1 = 3.141592f * ((i+1) / n);

        for(float j = 0.0f; j < n; j++){

            float phi = 2.0f * 3.1415926535f * (j / n);
            float phi1 = 2.0f * 3.141592f * ((j+1) / n);


            float x1 = cx + (sinf(theta) * cosf(phi) * r);
            float y1 = cy + (sinf(theta) * sinf(phi) * r);
            float z1 = cz + (cosf(theta) * r);

            float x2 = cx + (sinf(theta1) * cosf(phi) * r);
            float y2 = cy + (sinf(theta1) * sinf(phi) * r);
            float z2 = cz + (cosf(theta1) * r);

            float x3 = cx + (sinf(theta) * cosf(phi1) * r);
            float y3 = cy + (sinf(theta) * sinf(phi1) * r);
            float z3 = cz + (cosf(theta) * r);

            float x4 = cx + (sinf(theta1) * cosf(phi1) * r);
            float y4 = cy + (sinf(theta1) * sinf(phi1) * r);
            float z4 = cz + (cosf(theta1) * r);


            vector3 p1 = {x1, y1, z1};
            vector3 p2 = {x2, y2, z2};
            vector3 p3 = {x3, y3, z3};
            vector3 p4 = {x4, y4, z4};


            //normals for the mesh
            vector3 p12 = subtract_vec3s(p2, p1);
            vector3 p13 = subtract_vec3s(p3, p1);

            vector3 n1 = cross_product(p12, p13);

            // triangle 1
            vertices[idx++] = p1;
            vertices[idx++] = n1;

            vertices[idx++] = p2;
            vertices[idx++] = n1;

            vertices[idx++] = p3;
            vertices[idx++] = n1;

            // triangle 2
            vertices[idx++] = p3;
            vertices[idx++] = n1;

            vertices[idx++] = p4;
            vertices[idx++] = n1;

            vertices[idx++] = p2;
            vertices[idx++] = n1;

        }

    }

    return vertices;

}

GLFWwindow* init_render(){

     if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return NULL;
    }

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    GLFWwindow* window = glfwCreateWindow(1600, 900, "OpenGL Window", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);        
    
    
    if (!gladLoadGL()) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return NULL;
    }

    // Print renderer to ensure I'm using my GPU
    printf( "Renderer: %s.\n", glGetString( GL_RENDERER ) );
    printf( "OpenGL version supported %s.\n", glGetString( GL_VERSION ) );

    return window;

}

// inits the shaders, returns the shader program
GLuint init_shaders(){

    //Make these paths configurable?
    const char* fpath = "shaders/sphere.vert";
    char* vertex_shader = parse_shader_file(fpath);

    const char* frag_path = "shaders/sphere.frag";
    char* fragment_shader = parse_shader_file(frag_path);

    GLuint vs = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vs, 1, (const GLchar**)&vertex_shader, NULL );
    glCompileShader( vs );
    free(vertex_shader);

    GLuint fs = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fs, 1, (const GLchar**)&fragment_shader, NULL );
    glCompileShader( fs );
    free(fragment_shader);

    GLuint shader_program = glCreateProgram();
    glAttachShader( shader_program, fs );
    glAttachShader( shader_program, vs );
    glLinkProgram( shader_program );

    GLint success;

    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    { 
        printf("Failed to compile vertex shader.\n");
        return -1; 
    }

    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    { 
        printf("Failed to compile fragment shader.\n");
        return -1; 
    }

    // Init the ambient light for all bodies
    glUseProgram( shader_program );
    GLuint lightColorLoc = glGetUniformLocation(shader_program, "lightColor");
    if(lightColorLoc == -1){
        printf("failed to get lightColor uniform\n");
    }
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);

    return shader_program;

}

void init_3d_bodies(body_3d* bodies_array[], int num_bodies){
  
    for(int i=0; i < num_bodies ;i++){

        body_3d* b = bodies_array[i];

        // Init the orbit list for each body
        // bodies_array[i]->orbit = init_list();
        // initOrbit(bodies_array[i]->orbit);

        glGenBuffers( 1, &b->vbo );
        glGenVertexArrays( 1, &b->vao );

        vector3 coords = normalize_vec3(b->pos,SPACE_MIN,SPACE_MAX);

        int num_segments = 50; // How many segments in da spheres
        bodies_array[i]->resolution = num_segments;
        // normalizing radii of bodies need to be from 0->SPACE_MAX
        vector3* vertices = drawSphere(coords, normalize(b->radius,0,SPACE_MAX), num_segments);

        // this is magic number-y. I should fix this
        int idx = num_segments * num_segments * 12;

        glBindBuffer( GL_ARRAY_BUFFER, b->vbo );
        glBufferData( GL_ARRAY_BUFFER, idx * sizeof(vector3), vertices, GL_STATIC_DRAW );
        
        // vertices can be freed once it is in the OpenGL buffer
        free(vertices);

        glBindVertexArray( b->vao );
        glEnableVertexAttribArray( 0 );
        // Are doubles needed? Could I use floats for faster processing??
        // this is for the vertices
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (void*)0);
        glEnableVertexAttribArray(1);
        // this is for the normals, used in shading
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 2* sizeof(vector3), (void*)sizeof(vector3) );

    }

}

GLuint init_grid(grid *g){

    GLuint vao, vbo;

    glGenVertexArrays( 1, &vao );
    g->vao = vao;
    glBindVertexArray( g->vao );

    glGenBuffers( 1, &vbo );
    g->vbo = vbo;


    // I admit, the generation code isn't the cleanest, but it works ¯\_(ツ)_/¯
    // I need to look into an index buffer, a lot of this is re-used

    // The "step" is how far apart to draw each row/col
    // x and z define the bounds to draw
    // y is hardcoded to 0 as the reference point
    // CHANGING THESE REQUIRES A MANUAL CHANGE TO glDrawArrays() in draw_grid!
    float step = 0.05f;

    float x_init = -4.0f;
    float z_init = -4.0f;

    int rows = (int)ceilf((fabsf(x_init) + fabsf(x_init)) / step);

    int vertices = (2 * rows * rows) * 2;

    vector3* points = malloc(vertices * sizeof(vector3));

    int index = 0;

    for(int i=0; i < rows; i++){ 
        float x = x_init + i * step;
        for(int j=0; j < rows; j++){
            float z = z_init + j * step;
            points[index++] = (vector3){ x, 0.0f, z};
            z = z + step;
            points[index++] = (vector3){ x, 0.0f, z};
        }
    }

    for(int i=0; i < rows; i++){ 
        float z = z_init + i * step;
        for(int j=0; j < rows; j++){
            float x = x_init + j * step;
            points[index++] = (vector3){ x, 0.0f, z};
            x = x + step;
            points[index++] = (vector3){ x, 0.0f, z};
        }
    }

    glBindBuffer( GL_ARRAY_BUFFER, g->vbo );
    glBufferData( GL_ARRAY_BUFFER, vertices * sizeof(vector3), points, GL_STATIC_DRAW );

    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(vector3), (void*)0);
    glBindVertexArray(0);

    free(points); // loaded in the vbo, doesn't need to be on heap anymore

    // Shader init
    const char* vert_path = "shaders/grid.vert";
    char* vertex_shader = parse_shader_file(vert_path);

    const char* frag_path = "shaders/grid.frag";
    char* fragment_shader = parse_shader_file(frag_path);

    // vert shader setup / check
    GLuint vs = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vs, 1, (const GLchar**)&vertex_shader, NULL );
    glCompileShader( vs );
    free(vertex_shader);

    GLint success;

    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    { 
        printf("Failed to compile vertex shader.\n");
        return -1;
    }

    // frag shader setup / check
    GLuint fs = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fs, 1, (const GLchar**)&fragment_shader, NULL );
    glCompileShader( fs );
    free(fragment_shader);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    { 
        printf("Failed to compile fragment shader.\n");
        return -1;
    }
    
    // linking grid shader to the program
    GLuint grid_shaders = glCreateProgram();
    glAttachShader( grid_shaders, fs );
    glAttachShader( grid_shaders, vs );
    glLinkProgram( grid_shaders );

    glGetProgramiv(grid_shaders, GL_LINK_STATUS, &success);
    if (success != GL_TRUE)
    {
        printf("Failed to link grid shader program.\n");
        glDeleteProgram(grid_shaders);
        return -1;
    }

    return grid_shaders;
}

void draw_grid(grid *g, GLuint projLoc, GLuint viewLoc, GLuint modelLoc, float* view, float* projection){


    //GLuint projLoc = glGetUniformLocation(grid_shaders, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);

    //GLuint viewLoc = glGetUniformLocation(grid_shaders, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);

    //GLuint modelLoc = glGetUniformLocation(grid_shaders, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, identityMatrix4);
    

    glBindVertexArray( g->vao );
    // THE DRAW BYTES ARE HARDCODED
    glDrawArrays(GL_LINES, 0, 512000);

}

void show_debug_message(int run, int nbFrames, body_3d* bodies_array[], int num_bodies){

        // printf and reset timer
        // debug printf statements
        printf("\nCurrent Frame = %d", run);
        printf("\n%f ms/frame", 1000.0/(double)(nbFrames));
        printf("\nFPS: %f", (double)nbFrames / 1.0);
        printf("\nRendering With: %s", glGetString(GL_RENDERER));
        for(int i=0;i<num_bodies;i++){
            printf("\nB%d Velocity = {%f, %f, %f}", i+1, bodies_array[i]->velocity.x, bodies_array[i]->velocity.y, bodies_array[i]->velocity.z);
            printf("\nB%d Position = {%f, %f, %f}", i+1, bodies_array[i]->pos.x, bodies_array[i]->pos.y, bodies_array[i]->pos.z);
        }
}

void render3d(body_3d* bodies_array[], int ref_frame_code, int timeskip, const int num_bodies, const bool debug){

    //start glfw and glad
    GLFWwindow* window = init_render();

    //load and compile the shaders
    GLuint shaders = init_shaders();

    if(shaders == -1){
        printf("Exiting...\n");
        return;
    }else{
        puts("Shaders compiled successfully");
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // enable some blending for the grid
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Set the clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //define the projection matrix
    float fov = 60.0f * 3.14159f / 180.0f;
    float aspect = 1600.0f / 900.0f;
    float near = 0.001f;
    float far = 1000.0f;

    float f = 1.0f / tanf(fov / 2.0f);

    float projection[16] = {
        f/aspect, 0, 0,  0,
        0, f, 0,  0,
        0, 0, (far+near)/(near-far), -1,
        0, 0, (2*far*near)/(near-far), 0
    };


    //setup the camera
    camera *cam = malloc(sizeof(camera));
    vector3 camera_pos = {0,-0.3f,-5};
    cam->pos = camera_pos;
    const float cameraSpeedDefault = 0.00005f; 
    float cameraSpeed = cameraSpeedDefault;

    const float cameraRotSpeedDefault = 0.002f;
    float cameraRotSpeed = cameraRotSpeedDefault;

    float angle_x, angle_y, angle_z;
    angle_x = angle_y = angle_z = 0.0f;

    // Init the bodies
    init_3d_bodies(bodies_array, num_bodies);

    // Init the grid
    grid *g = ( grid* )malloc(sizeof(grid));
    GLuint grid_shaders = init_grid(g);
    GLuint warpLoc = glGetUniformLocation(grid_shaders, "warp");
    GLuint gridPosLoc = glGetUniformLocation(grid_shaders, "gridPos");

    glUseProgram( grid_shaders );
    GLuint gridPosCountLoc = glGetUniformLocation(grid_shaders, "gridPosCount");
    if(gridPosCountLoc == -1){
        printf("Failed to get gridPosCount uniform");
    }
    glUniform1i(gridPosCountLoc, num_bodies);

    // arrays in the grid shaders
    vector3 planetGridPos[num_bodies];
    float warp[num_bodies];
    float grid_r_s[num_bodies];
    float grid_radius[num_bodies];


    // This is sorta-temp code while I figure out how I want to do the translations long-term
    vector3 init_bodies_pos[num_bodies];
    for(int i=0; i < num_bodies; i++){

        init_bodies_pos[i] = bodies_array[i]->pos;
        grid_r_s[i] = normalize(scharzchild_radius(bodies_array[i]->mass), 0, SPACE_MAX);
        grid_radius[i] = normalize(bodies_array[i]->radius, 0, SPACE_MAX);
        printf("Schwarzchild Radius of %d = %f, Noramlized = %f\n", i+1, scharzchild_radius(bodies_array[i]->mass), grid_r_s[i]);

    }

    double lastTime = glfwGetTime();
    int nbFrames = 0;
    int run = 0;
    matrix4 lightModel;

    glUseProgram( shaders );
    // get the uniform locations
    // the model view and projection of the bodies
    GLuint modelLoc = glGetUniformLocation(shaders, "model");
    GLuint projLoc = glGetUniformLocation(shaders, "projection");
    GLuint viewLoc = glGetUniformLocation(shaders, "view");
    ///
    GLuint ambientStrengthLoc = glGetUniformLocation(shaders, "ambientStrength");
    GLuint diffuseStrengthLoc = glGetUniformLocation(shaders, "diffuseStrength");
    GLuint lightModelLoc = glGetUniformLocation(shaders, "lightModel");
    if(lightModelLoc == -1){
        printf("failed to get lightModel uniform");
    }
    GLuint objColorLoc = glGetUniformLocation(shaders, "objectColor");
    // Set the pos of the diffuse light
    GLuint lightPos = glGetUniformLocation(shaders, "lightPos");
    if(lightPos == -1){
        printf("Failed to get uniform lightPos");
    }

    // There actually just shouldn't be a light source if theres no stars
    glUniform3f(lightPos, 0.0f, 0.0f, 0.0f);

    // get the grid shader uniform locations
    GLuint gridProjLoc = glGetUniformLocation(grid_shaders, "projection");
    GLuint gridViewLoc = glGetUniformLocation(grid_shaders, "view");
    GLuint gridModelLoc = glGetUniformLocation(grid_shaders, "model");
    //GLuint gridRealPos = glGetUniformLocation(grid_shaders, "realPos");
    GLuint gridBodyRadius = glGetUniformLocation(grid_shaders, "radius");
    GLuint scharzchildLoc = glGetUniformLocation(grid_shaders, "r_s");


    
    while ( !glfwWindowShouldClose( window ) ) {
        nbFrames++;
        run++;
        // Frame timer
        double currentTime = glfwGetTime();
        if ( currentTime - lastTime >= 1.0 && debug){ // If last prinf() was more than 1 sec ago
            show_debug_message(run, nbFrames, bodies_array, num_bodies);
            lastTime += 1.0;
            nbFrames = 0;
        }

        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
            glfwSetWindowShouldClose(window, true);
        }


        // move camera forward / back
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
            cam->pos.z += cameraSpeed; 
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
            cam->pos.z -= cameraSpeed; 
        }
        // move camera left / right
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
            cam->pos.x += cameraSpeed; 
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
            cam->pos.x -= cameraSpeed; 
        }
        // move cam up
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
            cam->pos.y -= cameraSpeed; 
        }
        // move cam down
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
            cam->pos.y += cameraSpeed; 
        }

        // x rotation
        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS){
            angle_x += cameraRotSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS){
            angle_x -= cameraRotSpeed; 
        }

        // y rotation
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS){
            angle_y += cameraRotSpeed; 
        }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS){
            angle_y -= cameraRotSpeed; 
        }

        // this rotates along z axis
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
            angle_z += cameraRotSpeed; 
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
            angle_z -= cameraRotSpeed; 
        }

        // this adjusts the camera speed
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){
            cameraSpeed = cameraSpeedDefault;
            cameraRotSpeed = cameraRotSpeedDefault;
            puts("Camera speed set to 1x");
        }
        // 2x speed
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS){
            cameraSpeed = cameraSpeedDefault * 2;
            cameraRotSpeed = cameraRotSpeedDefault * 2;
            puts("Camera speed set to 2x");
        }
        // 5x speed
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS){
            cameraSpeed = cameraSpeedDefault * 5;
            cameraRotSpeed = cameraRotSpeedDefault * 5;
            puts("Camera speed set to 5x");
        }

        float cx = cos(angle_x * 3.14159f / 180.0f);
        float sx = sin(angle_x * 3.14159f / 180.0f);
        float cy = cos(angle_y * 3.14159f / 180.0f);
        float sy = sin(angle_y * 3.14159f / 180.0f);
        float cz = cos(angle_z * 3.14159f / 180.0f);
        float sz = sin(angle_z * 3.14159f / 180.0f);

        float x = cam->pos.x;
        float y = cam->pos.y;
        float z = cam->pos.z;

        // full x,y,z rotation matrix
        // this isn't actually a good way to do this, has many issues
        // should fix this 
        float view[16] = {
            cz * cy, -sz, -sy, 0,
            sz, cx * cz, sx, 0,
            sy, -sx, cx * cy, 0,
            x, y, z, 1,
        };


        glfwPollEvents();
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glUseProgram( shaders );
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);
        rk4_nbody_3d(0, timeskip, bodies_array, num_bodies);

        for(int i=0;i<num_bodies;i++){ 

            glUseProgram( shaders );

            body_3d *b = bodies_array[i];

            vector3 n_pos = normalize_vec3(subtract_vec3s(bodies_array[i]->pos,init_bodies_pos[i]), SPACE_MIN, SPACE_MAX);

            vector3 b_pos = normalize_vec3(b->pos, SPACE_MIN, SPACE_MAX);

            //glUniform3f(gridRealPos, n_pos.x, n_pos.y, n_pos.z);

            // this is SLOW AS FUCK.... but proves my concept atleast so yay
            // EXPERIMENTAL GRID FEAT
           
            vector3 n_pos_grid = { roundf(b_pos.x * 20.0f)/20.0f, 0.0f, roundf(b_pos.z * 20.0f)/20.0f };      

            planetGridPos[i] = n_pos_grid;

            matrix4 model = {
                {1.0, 0.0, 0.0, n_pos.x},
                {0.0, 1.0, 0.0, n_pos.y},
                {0.0, 0.0, 1.0, n_pos.z},
                {0.0, 0.0, 0.0, 1.0}
            };


            //Should this exit the program?
            if (modelLoc == -1){
                printf("failed to get model from shader\n");
            }

            glUniformMatrix4fv(modelLoc, 1, GL_TRUE, (const GLfloat *)model);
            glBindVertexArray( bodies_array[i]->vao );

            // Setup obj color
            if(objColorLoc == -1){
                printf("failed to get objectColor uniform");
            }
            glUniform3f(objColorLoc, b->color.r, b->color.g, b->color.b);

            //update the lighting

            if(ambientStrengthLoc == -1){
                printf("failed to get ambientStrength uniform");
            }

            // Check if a body is defined as a star
            // If yes, give is an ambient strength of max and add it as a light source (iffy)
            if(b->type == STAR){\
                glUniform1f(diffuseStrengthLoc, 1.0f);
                memcpy(lightModel, model, sizeof(model));
                // keep it always bright (like a star)
                glUniform1f(ambientStrengthLoc, 1.0f);
            } else {
                glUniform1f(ambientStrengthLoc, 0.08f);
            }

            glUniformMatrix4fv(lightModelLoc, 1, GL_TRUE, (const GLfloat *)lightModel);

            int res = bodies_array[i]->resolution;
            // bytes to render. the 6 is the vertices in the quad for the sphere
            int b_render = res * res * 6; 
            glDrawArrays(GL_TRIANGLES, 0, b_render);

            // Send the radius to the shader for this body
            // Used to calculate Flamm's
            glUseProgram( grid_shaders );
            glUniform1fv(gridBodyRadius, num_bodies, (const GLfloat *)grid_radius);
            glUniform1fv(scharzchildLoc, num_bodies, (const GLfloat *)grid_r_s);
            
        }

        glUseProgram( grid_shaders );

        glUniform3fv(gridPosLoc, num_bodies, (const GLfloat *)planetGridPos);
        glUniform1fv(warpLoc, num_bodies, (const GLfloat *)warp);

        draw_grid(g, gridProjLoc, gridViewLoc, gridModelLoc, view, projection);
        glfwSwapBuffers( window );

    }
    glfwDestroyWindow(window);
    glfwTerminate();

    //Free some stuff
    free(cam);

    puts("\nSimulation Ending...");
}