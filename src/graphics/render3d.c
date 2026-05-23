#include <string.h>
#include "graphics/orbits.h"
#include "graphics/render3d.h"
#include "graphics/controls.h"
#include "graphics/legend.h"
#include "math/math_funcs.h"
#include "math/vector/vector3.h"
#include "math/vector/vector4.h"
#include "math/matrix/matrix4.h"
#include "physics/gravity3d.h"
#include "utils/shaders_parser.h"

[[gnu::pure]] vector3* drawSphere(vector3 s, float r, int NUM_SEGMENTS) {

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

        int num_segments = 80; // How many segments in da spheres
        bodies_array[i]->resolution = num_segments;

        // normalizing radii of bodies need to be from 0->SPACE_MAX
        vector3* vertices = drawSphere(coords, normalize(b->radius,0,SPACE_MAX), num_segments);

        // Init the orbit path
        bodies_array[i]->orbit = init_list();
        initOrbit(bodies_array[i]->orbit);

        // this is magic number-y. I should fix this
        int idx = num_segments * num_segments * 12;

        glBindBuffer( GL_ARRAY_BUFFER, b->vbo );
        glBufferData( GL_ARRAY_BUFFER, idx * sizeof(vector3), vertices, GL_STATIC_DRAW );
        
        // vertices can be freed once it is in the OpenGL buffer
        free(vertices);

        glBindVertexArray( b->vao );
        glEnableVertexAttribArray( 0 );
        // Doubles are needed for large values. Floats get overflown too easily
        // this is for the vertices
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (void*)0);
        glEnableVertexAttribArray(1);
        // this is for the normals, used in shading
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (void*)sizeof(vector3) );

    }

}

GLuint init_grid(Grid *g){

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

    float x_init = -5.0f;
    float z_init = -5.0f;

    int rows = (int)ceilf((fabsf(x_init) + fabsf(z_init)) / step);

    int vertices = (2 * rows * rows) * 2;

    vector3* points = malloc(vertices * sizeof(vector3));

    int index = 0;

    for(int i=0; i < rows; i++){ 
        float x = x_init + (i * step);
        for(int j=0; j < rows; j++){
            float z = z_init + (j * step);
            points[index++] = (vector3){ x, 0.0f, z};
            z = z + step;
            points[index++] = (vector3){ x, 0.0f, z};
        }
    }

    for(int i=0; i < rows; i++){ 
        float z = z_init + (i * step);
        for(int j=0; j < rows; j++){
            float x = x_init + (j * step);
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

void draw_grid(Grid *g, GLuint projLoc, GLuint viewLoc, GLuint modelLoc, const float* view, float* projection){

    //GLuint projLoc = glGetUniformLocation(grid_shaders, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);

    //GLuint viewLoc = glGetUniformLocation(grid_shaders, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);

    //GLuint modelLoc = glGetUniformLocation(grid_shaders, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, identityMatrix4);
    
    glBindVertexArray( g->vao );
    // THE DRAW BYTES ARE HARDCODED
    glDrawArrays(GL_LINES, 0, 160000);

}

void show_debug_message(int run, int nbFrames, body_3d* bodies_array[], int num_bodies){

        // debug statement
        printf("\nCurrent Frame = %d", run);
        printf("\n%f ms/frame", 1000.0/(double)(nbFrames));
        printf("\nFPS: %f", (double)nbFrames / 1.0);
        printf("\nRendering With: %s", glGetString(GL_RENDERER));
        for(int i=0;i<num_bodies;i++){
            printf("\nB%d Velocity = {%f, %f, %f}", i+1, bodies_array[i]->velocity.x, bodies_array[i]->velocity.y, bodies_array[i]->velocity.z);
            printf("\nB%d Position = {%f, %f, %f}", i+1, bodies_array[i]->pos.x, bodies_array[i]->pos.y, bodies_array[i]->pos.z);
        }
}

void render3d(body_3d* bodies_array[], Settings* config_settings){


    int num_bodies = config_settings->num_bodies;
    bool debug = config_settings->debug;
    float timeskip = config_settings->time_delta;
    int ref_frame_code = config_settings->ref_frame_code;

    //start glfw and glad
    GLFWwindow* window = init_render();
    //load and compile the shaders
    GLuint shaders = init_shaders();

    // setup the text
    FT_Setup* ft = ft_setup(config_settings->font);

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
    float fov = 60.0f * DEG_TO_RAD;
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


    // Setup the camera
    Camera *cam = malloc(sizeof(Camera));

    vector3 cameraPosDefault = {0, 0.4f,1.5f};
    
    cam->pos = cameraPosDefault;
    cam->pitch = 0.0f;
    cam->yaw = -90.0f;
    cam->speedMultiplier = 3.0f;
    
    vector3 up = {0.0f, 1.0f, 0.0f};

    cam->tracking = false;
    cam->tracked_body = 0;
    cam->num_bodies = num_bodies;
    cam->tracking_vector = (svector3){0.0f, 0.0f, 0.0f};

    // Init the bodies
    init_3d_bodies(bodies_array, num_bodies);

    // Init the grid
    Grid *g = ( Grid* )malloc(sizeof(Grid));
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

    double lastTime = glfwGetTime(); // Time of the last debug message
    float lastFrame = 0.0f; // Time of the last frame
    float deltaTime = 0.0f;	// Time between current frame and last frame
    int nbFrames = 0;
    int run = 0;

    bool display_legend = false;


    glUseProgram( shaders );
    // get the uniform locations
    // the model view and projection of the bodies
    GLuint modelLoc = glGetUniformLocation(shaders, "model");
    if (modelLoc == -1){
        printf("failed to get model from shader\n");
    }

    GLuint projLoc = glGetUniformLocation(shaders, "projection");
    GLuint viewLoc = glGetUniformLocation(shaders, "view");
    
    /// Lighting-related uniforms
    GLuint ambientStrengthLoc = glGetUniformLocation(shaders, "ambientStrength");
    if(ambientStrengthLoc == -1){
        printf("failed to get ambientStrength uniform\n");
    }

    GLuint diffuseStrengthLoc = glGetUniformLocation(shaders, "diffuseStrength");
    GLuint lightModelLoc = glGetUniformLocation(shaders, "lightModel");
    if(lightModelLoc == -1){
        printf("failed to get lightModel uniform\n");
    }
    GLuint objColorLoc = glGetUniformLocation(shaders, "objectColor");
    if(objColorLoc == -1){
        printf("failed to get objectColor uniform\n");
    }
    // Set the pos of the diffuse light
    GLuint lightPos = glGetUniformLocation(shaders, "lightPos");
    if(lightPos == -1){
        printf("Failed to get uniform lightPos\n");
    }

    GLuint numLightSourcesLoc = glGetUniformLocation(shaders, "numLightSources");
    if(numLightSourcesLoc == -1){
        printf("Failed to get uniform lightPos\n");
    }

    // get the grid shader uniform locations
    GLuint gridProjLoc = glGetUniformLocation(grid_shaders, "projection");
    GLuint gridViewLoc = glGetUniformLocation(grid_shaders, "view");
    GLuint gridModelLoc = glGetUniformLocation(grid_shaders, "model");
    GLuint gridBodyRadius = glGetUniformLocation(grid_shaders, "radius");
    GLuint scharzchildLoc = glGetUniformLocation(grid_shaders, "r_s");

    // Magnitude is used to amplify Flamm's paraboloid, since for most objects (like the sun)
    // the curvature is so small it cannot be seen
    GLuint magnitudeLoc = glGetUniformLocation(grid_shaders, "magnitude");
    if(magnitudeLoc == -1){
        printf("Failed to get uniform magnitude\n");
    }

    // Loads the orbit vertex and frag shader
    GLuint orbit_shader = init_orbit_shaders();

    // This gets added to in the init bodies loop below
    int numLightSources = 0;

    // This is sorta-temp code while I figure out how I want to do the translations long-term
    vector3 init_bodies_pos[num_bodies];
    for(int i=0; i < num_bodies; i++){

        init_bodies_pos[i] = bodies_array[i]->pos;
        grid_r_s[i] = normalize(scharzchild_radius(bodies_array[i]->mass), 0, SPACE_MAX/2);
        grid_radius[i] = normalize(bodies_array[i]->radius, 0, SPACE_MAX);
        
        printf("Schwarzchild Radius of %d = %f, Normalized = %.8lf\n", i+1, scharzchild_radius(bodies_array[i]->mass), grid_r_s[i]);
        printf("Normalized Radius = %f\n", grid_radius[i]);

        // Checking to see if the body is a star, and adding to numLightSources if it is
        if(bodies_array[i]->type == STAR){
            numLightSources++;
        }
    }

    //light related arrays
    matrix4 lightModel[numLightSources];
    vector3 lightLocations[numLightSources]; 

    glUniform1i(numLightSourcesLoc, numLightSources);
    while ( !glfwWindowShouldClose( window ) ) {
        nbFrames++;
        run++;
        // Frame timer
        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;  
        if ( currentTime - lastTime >= 1.0 && debug){ // If last prinf() was more than 1 sec ago
            show_debug_message(run, nbFrames, bodies_array, num_bodies);
            if (cam->tracking) {
                vector3 body_pos = normalize_vec3(bodies_array[cam->tracked_body]->pos, SPACE_MIN, SPACE_MAX);
                printf("cam pos: (%.3f, %.3f, %.3f) | body pos: (%.3f, %.3f, %.3f) | offset: (%.3f, %.3f, %.3f)\n",
                    cam->pos.x, cam->pos.y, cam->pos.z,
                    body_pos.x, body_pos.y, body_pos.z,
                    cam->tracking_vector.r, cam->tracking_vector.az, cam->tracking_vector.el);
            }
            lastTime += 1.0;
            nbFrames = 0;
        }

        bool was_tracking = cam->tracking;
        int prev_tracked = cam->tracked_body;

        float yaw = cam->yaw * DEG_TO_RAD;
        float pitch = cam->pitch * DEG_TO_RAD;

        // This is used to handle rotation of the camera
        vector3 direction;
        direction.x = cos(yaw) * cos(pitch);
        direction.y = sin(pitch);
        direction.z = sin(yaw) * cos(pitch);
        vector3 cameraFront = vec3_unit_vector(direction);

        cam->speed = 0.3f * deltaTime * cam->speedMultiplier; 
        cam->rotSpeed = 3.0f * deltaTime * cam->speedMultiplier;
        cam->front = cameraFront;

        get_input(window, cam);

        // new tracking, capture tracking vector from body to camera in space
        // OR
        // cycled to a different body, recapture the tracking vector
        if ((!was_tracking && cam->tracking) || (was_tracking && cam->tracking && cam->tracked_body != prev_tracked)){
            vector3 normed_track = normalize_vec3(bodies_array[cam->tracked_body]->pos, SPACE_MIN, SPACE_MAX);
            cam->tracking_vector = cartesian_to_spherical(subtract_vec3s(cam->pos, normed_track));
        }
        // disabled tracking, rsync yaw/pitch from actual look direction
        if (was_tracking && !cam->tracking) {
            vector3 look = vec3_unit_vector(scale_vec3(spherical_to_cartesian(cam->tracking_vector), -1.0));
            cam->pitch = asinf(look.y) * RAD_TO_DEG;
            cam->yaw = atan2f(look.z, look.x) * RAD_TO_DEG;
            cameraFront = look;
        }
        // while tracking is on
        if (cam->tracking) {
            body_3d *target = bodies_array[cam->tracked_body];
            vector3 normalized_target = normalize_vec3(target->pos, SPACE_MIN, SPACE_MAX);
            cam->pos = add_vec3s(normalized_target, spherical_to_cartesian(cam->tracking_vector));
            cameraFront = vec3_unit_vector(subtract_vec3s(normalized_target, cam->pos));
        }


        // These are the steps to calculte the vectors needed for a lookAt matrix
        vector3 cameraTarget = add_vec3s(cameraFront, cam->pos);
        vector3 cameraDirection = vec3_unit_vector(subtract_vec3s(cam->pos, cameraTarget));
        vector3 cameraRight = vec3_unit_vector(cross_product(up, cameraDirection));
        vector3 cameraUp = cross_product(cameraDirection, cameraRight);

        // View  Rotation Matrix Matrix
        matrix4 view = {
            {cameraRight.x, cameraUp.x, cameraDirection.x, 0},
            {cameraRight.y, cameraUp.y, cameraDirection.y, 0},
            {cameraRight.z, cameraUp.z, cameraDirection.z, 0},
            {   (-1) * dot_vec3s(cameraRight, cam->pos),
                (-1) * dot_vec3s(cameraUp, cam->pos),
                (-1) * dot_vec3s(cameraDirection, cam->pos)
                ,1}
        };

        cam->right = cameraRight;
        cam->up = cameraUp;

        glfwPollEvents();
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glUseProgram( shaders );
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (const GLfloat *)view);

        //for orbits
        GLuint viewLoc2 = glGetUniformLocation(orbit_shader, "view");
        glUseProgram(orbit_shader);
        glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, (const GLfloat *)view);
        GLuint projLoc2 = glGetUniformLocation(orbit_shader, "projection");
        glUseProgram(orbit_shader);
        glUniformMatrix4fv(projLoc2, 1, GL_FALSE, projection);


        // TODO: Move this into controls.c
        static bool o_was_pressed = false;
        bool o_pressed = glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS;
        if (o_pressed && !o_was_pressed){
            display_legend = !display_legend;
        }
        o_was_pressed = o_pressed;

        if(display_legend){

            draw_legend(ft, cam, bodies_array, nbFrames);

        }

        // This is the main equation driving the physics
        if(ref_frame_code == 101){
            cog_ref_runge_kutta_3d(0, timeskip, bodies_array[0], bodies_array[1]);

        }else if(ref_frame_code == 200){
            rk4_nbody_3d(0, timeskip, bodies_array, num_bodies);

        }else{
            exit(1); // should never be reached
        }
      
        for(int i=0;i<num_bodies;i++){ 

            
            body_3d *b = bodies_array[i];

            glUseProgram( orbit_shader );
            int modelLocationOrbit = glGetUniformLocation(orbit_shader, "model");

            if (modelLocationOrbit == -1){
                printf("failed to get model from orbit shader");
            }

            // Normalized translation of body (difference from init position to current one)
            // This is used for the model translation
            vector3 n_pos = normalize_vec3(subtract_vec3s(bodies_array[i]->pos,init_bodies_pos[i]), SPACE_MIN, SPACE_MAX);

            // Normalized Positions of the bodies
            vector3 b_pos = normalize_vec3(b->pos, SPACE_MIN, SPACE_MAX);


            // orbits updating
            // This variable is probably poorly named, but it essentially puts a line in the orbit path once every N frames
            // where N is the ORBIT_SAMPLING var
            if(config_settings->draw_orbits){
                int ORBIT_SAMPLING = 500;

                if(run % ORBIT_SAMPLING == 0){
                    updateOrbits(b->orbit, b_pos);
                }
                
                glUniformMatrix4fv(modelLocationOrbit, 1, GL_TRUE, (const GLfloat *)identityMatrix4);
                drawOrbit(b->orbit);
            }

            //glUniform3f(gridRealPos, n_pos.x, n_pos.y, n_pos.z);
           
            // Get normalized b_pos, rounded to the nearest 0.01.
            vector3 n_pos_grid = { roundf(b_pos.x * 100.0f)/100.0f, 0.0f, roundf(b_pos.z * 100.0f)/100.0f };      

            // Used in the grid vert shader to render Flamm's Parabloid
            planetGridPos[i] = n_pos_grid;

            matrix4 model = {
                {1.0, 0.0, 0.0, 0.0},
                {0.0, 1.0, 0.0, 0.0},
                {0.0, 0.0, 1.0, 0.0},
                {n_pos.x, n_pos.y, n_pos.z, 1.0},
            };

            glUseProgram( shaders );
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (const GLfloat *)model);
            glBindVertexArray( bodies_array[i]->vao );

            // Check if a body is defined as a star
            // If yes, give is an ambient strength of max and add it as a light source (iffy)
            if(b->type == STAR){
                glUniform1f(diffuseStrengthLoc, 1.0f);
                memcpy(lightModel[i], model, sizeof(model));

                // keep it always bright (like a star)
                glUniform1f(ambientStrengthLoc, 1.0f);
                lightLocations[i] = b_pos;        

            } else {
                glUniform1f(ambientStrengthLoc, 0.08f);
            }

            // Setup obj color
            glUniform3f(objColorLoc, b->color.r, b->color.g, b->color.b);
            glUniform3fv(lightPos, numLightSources, (const GLfloat *)lightLocations);
            glUniformMatrix4fv(lightModelLoc, numLightSources, GL_FALSE, (const GLfloat *)lightModel);

            int res = bodies_array[i]->resolution;
            
            // bytes to render. the 6 is the vertices in the quad for the sphere
            int b_render = res * res * 6; 
            glDrawArrays(GL_TRIANGLES, 0, b_render);



            // Used to calculate Flamm's
            if(config_settings->draw_grid){
                glUseProgram( grid_shaders );
                glUniform1fv(gridBodyRadius, num_bodies, (const GLfloat *)grid_radius);
                glUniform1fv(scharzchildLoc, num_bodies, (const GLfloat *)grid_r_s);
            }

        }

        if(config_settings->draw_grid){
            glUseProgram( grid_shaders );
        
            // Magnitude multiplier for spacetime curvature
            glUniform1f(magnitudeLoc, 1.0f);
            glUniform3fv(gridPosLoc, num_bodies, (const GLfloat *)planetGridPos);
            glUniform1fv(warpLoc, num_bodies, (const GLfloat *)warp);
            draw_grid(g, gridProjLoc, gridViewLoc, gridModelLoc, (const GLfloat *)view, projection);
        }

        glfwSwapBuffers( window );

    }
    glfwDestroyWindow(window);
    glfwTerminate();

    //Free some stuff
    free(cam);

    puts("\nSimulation Ending...");
}
