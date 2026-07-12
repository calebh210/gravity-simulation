#include <string.h>
#include "graphics/orbits.h"
#include "graphics/render3d.h"
#include "graphics/grid.h"
#include "graphics/controls.h"
#include "graphics/shapes/sphere.h"
#include "math/math_funcs.h"
#include "math/vector/vector3.h"
#include "math/vector/vector4.h"
#include "math/matrix/matrix4.h"
#include "physics/gravity3d.h"
#include "graphics/textures/textures.h"
#include "utils/shaders_parser.h"
#include "graphics/loader/obj_loader.h"
#include "graphics/scene.h"

// OpenGL reference: https://antongerdelan.net/opengl/hellotriangle.html
// Render Timestep reference: https://gafferongames.com/post/fix_your_timestep/

GLFWwindow* init_render() {

    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return NULL;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1600, 900, "OpenGL Window", NULL, NULL);
    if(!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    if(!gladLoadGL()) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return NULL;
    }

    // Print renderer to ensure I'm using my GPU
    printf("Renderer: %s.\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s.\n", glGetString(GL_VERSION));

    return window;
}

// inits the shaders, returns the shader program
GLuint init_shaders() {

    // Make these paths configurable?
    const char* fpath = "shaders/sphere.vert";
    char* vertex_shader = parse_shader_file(fpath);

    const char* frag_path = "shaders/sphere.frag";
    char* fragment_shader = parse_shader_file(frag_path);

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, (const GLchar**)&vertex_shader, NULL);
    glCompileShader(vs);
    free(vertex_shader);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, (const GLchar**)&fragment_shader, NULL);
    glCompileShader(fs);
    free(fragment_shader);

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, fs);
    glAttachShader(shader_program, vs);
    glLinkProgram(shader_program);

    GLint success;

    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if(success != GL_TRUE) {
        printf("Failed to compile vertex shader.\n");
        return -1;
    }

    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if(success != GL_TRUE) {
        printf("Failed to compile fragment shader.\n");
        return -1;
    }

    // Init the ambient light for all bodies
    glUseProgram(shader_program);
    GLuint lightColorLoc = glGetUniformLocation(shader_program, "lightColor");
    if(lightColorLoc == -1) {
        printf("failed to get lightColor uniform\n");
    }
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);

    return shader_program;
}

void init_3d_bodies(body_3d* bodies_array[], int num_bodies) {

    for(int i = 0; i < num_bodies; i++) {

        body_3d* b = bodies_array[i];

        glGenBuffers(1, &b->vbo);
        glGenVertexArrays(1, &b->vao);

        // normalizing radii of bodies need to be from 0->SPACE_MAX

        vector3_da vertices;
        vector2_da uvs;
        vector3_da normals;

        vector3_da_init(&vertices);
        vector2_da_init(&uvs);
        vector3_da_init(&normals);

        // Check if these a custom .obj model to use. IF there's not, just draw a
        // sphere
        if(b->has_model) {

            printf("loading custom model filename = %s\n", b->model);
            char* filename = b->model;
            load_obj(filename, &vertices, &uvs, &normals);

            // Need to store this so I can have OpenGL draw the right num of bytes
            // later
            bodies_array[i]->resolution = vertices.size;
            ;

        } else {

            int sector_count = 36; // How many segments in da spheres
            int stack_count = 18;
            bodies_array[i]->resolution =
                (sector_count * stack_count) *
                6; // * 6 for the num of vertices in each quad

            drawSphere(normalize(b->radius, 0, SPACE_MAX), sector_count, stack_count,
                       &vertices, &normals, &uvs);
        }

        // check if a texture is defined
        if(b->has_texture) {
            load_texture(b);
        }

        // Init the orbit path
        bodies_array[i]->orbit = initOrbit();

        // Init the rotation to 0,0,0 (I might want to use quats eventually?)
        b->rotation = (vector3){0.0f, 0.0f, 0.0f};

        glBindBuffer(GL_ARRAY_BUFFER, b->vbo);

        size_t total_size = (vertices.size + normals.size) * sizeof(vector3) +
                            (uvs.size * sizeof(vector2));

        glBufferData(GL_ARRAY_BUFFER, total_size, NULL, GL_STATIC_DRAW);

        // Add the vertices / normals to the buffer
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size * sizeof(vector3),
                        &vertices.buf[0]);
        glBufferSubData(GL_ARRAY_BUFFER, vertices.size * sizeof(vector3),
                        normals.size * sizeof(vector3), &normals.buf[0]);
        glBufferSubData(GL_ARRAY_BUFFER,
                        (vertices.size + normals.size) * sizeof(vector3),
                        uvs.size * sizeof(vector2), &uvs.buf[0]);

        // TODO: Write method to free dynamic arrays
        // vertices can be freed once it is in the OpenGL buffer
        // free(vertices);

        glBindVertexArray(b->vao);
        glEnableVertexAttribArray(0);
        // Doubles are needed for large values. Floats get overflown too easily
        // this is for the vertices
        // glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3),
        // (void*)0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(1);
        // this is for the normals, used in shading
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0,
                              (void*)(vertices.size * sizeof(vector3)));

        glVertexAttribPointer(
            2, 2, GL_FLOAT, GL_FALSE, 0,
            (void*)((vertices.size + normals.size) * sizeof(vector3)));
        glEnableVertexAttribArray(2);
    }
}

void show_debug_message(int run, double nbFrames, body_3d* bodies_array[],
                        int num_bodies) {

    // debug statement
    printf("\nCurrent Frame = %d", run);
    printf("\n%f ms/frame", 1000.0 / (double)(nbFrames));
    printf("\nFPS: %f", (double)nbFrames / 1.0);
    printf("\nRendering With: %s", glGetString(GL_RENDERER));
    for(int i = 0; i < num_bodies; i++) {
        printf("\nB%d Velocity = {%f, %f, %f}", i + 1, bodies_array[i]->velocity.x,
               bodies_array[i]->velocity.y, bodies_array[i]->velocity.z);
        printf("\nB%d Position = {%f, %f, %f}", i + 1, bodies_array[i]->pos.x,
               bodies_array[i]->pos.y, bodies_array[i]->pos.z);
    }
}

void step_rotation_3d(body_3d* bodies[], int num_bodies) {

    // float rotational_period = 1.0f * 86400; // 1 day

    for(int i = 0; i < num_bodies; i++) {

        body_3d* b = bodies[i];

        // I can probably do this calc once earlier and save it somewhere?
        // TODO!
        if(b->rotational_period.x != 0.0f) {
            float angle_per_second_x = TWO_PI / b->rotational_period.x;
            b->rotation.x += angle_per_second_x;
        }
        if(b->rotational_period.y != 0.0f) {
            float angle_per_second_y = TWO_PI / b->rotational_period.y;
            b->rotation.y += angle_per_second_y;
        }
        if(b->rotational_period.z != 0.0f) {
            float angle_per_second_z = TWO_PI / b->rotational_period.z;
            b->rotation.z += angle_per_second_z;
        }
    }
}

void step_physics_3d(body_3d* bodies[], int num_bodies,
                     enum REFERENCE_FRAME frame, float sim_t, float sim_dt) {

    // This is the main equation driving the physics
    switch(frame) {

    case CENTER_OF_GRAVITY:
        cog_ref_runge_kutta_3d(0, sim_dt, bodies[0], bodies[1]);
        break;

    case N_BODY:
        rk4_nbody_3d(0, sim_dt, bodies, num_bodies);
        break;

    default:
        exit(1); // should never be reached
    }

    step_rotation_3d(bodies, num_bodies);
}

void render3d(Scene* scene) {

    int num_bodies = scene->num_bodies;
    bool debug = scene->config->debug;
    const float TIMESKIP = scene->config->time_delta;

    // start glfw and glad
    GLFWwindow* window = init_render();
    // load and compile the shaders
    GLuint shaders = init_shaders();

    GLuint orbit_shader = init_orbit_shaders();

    // setup the text
    scene->config->ft = ft_setup(scene->config->font);

    if(shaders == -1) {
        printf("Exiting...\n");
        return;
    } else {
        puts("Shaders compiled successfully");
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // enable some blending for the grid
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Set the clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // define the projection matrix
    float fov = 60.0f * DEG_TO_RAD;
    float aspect = 1600.0f / 900.0f;
    float near = 0.001f;
    float far = 10000.0f;
    float f = 1.0f / tanf(fov / 2.0f);

    matrix4 projection;
    matrix4_init_empty(projection);
    projection[0][0] = f / aspect;
    projection[1][1] = f;
    projection[2][2] = (far + near) / (near - far);
    projection[2][3] = -1;
    projection[3][2] = (2 * far * near) / (near - far);

    // Orbit.vert uses a static layout for the location of its uniforms
    glUseProgram(orbit_shader);
    glUniformMatrix4fv(1, 1, GL_FALSE, (const GLfloat*)MATRIX4_IDENTITY_MATRIX);
    glUniformMatrix4fv(3, 1, GL_FALSE, (const GLfloat*)projection);

    // Setup the camera
    scene->cam = malloc(sizeof(Camera));

    vector3 cameraPosDefault = {0, 0.4f, 1.5f};
    // vector3 cameraPosDefault = {0, 0.0f,0.0f};

    scene->cam->pos = cameraPosDefault;
    scene->cam->pitch = 0.0f;
    scene->cam->yaw = -90.0f;
    scene->cam->speedMultiplier = 3.0f;

    vector3 up = {0.0f, 1.0f, 0.0f};

    scene->cam->tracking = false;
    scene->cam->tracked_body = 0;
    scene->cam->tracking_vector = (svector3){0.0f, 0.0f, 0.0f};

    // Init the bodies
    init_3d_bodies(scene->bodies_array, num_bodies);

    // Init the grid
    Grid* g = (Grid*)malloc(sizeof(Grid));
    init_grid(g);
    GLuint gridPosLoc = glGetUniformLocation(g->shaders, "gridPos");

    glUseProgram(g->shaders);
    GLuint gridPosCountLoc = glGetUniformLocation(g->shaders, "gridPosCount");
    if(gridPosCountLoc == -1) {
        printf("Failed to get gridPosCount uniform");
    }
    glUniform1i(gridPosCountLoc, num_bodies);

    // arrays in the grid shaders
    vector3 planetGridPos[num_bodies];
    float grid_r_s[num_bodies];
    float grid_radius[num_bodies];

    glUseProgram(shaders);

    // get the uniform locations
    // the model view and projection of the bodies
    GLuint modelLoc = glGetUniformLocation(shaders, "model");
    if(modelLoc == -1) {
        printf("failed to get model from shader\n");
    }

    GLuint projLoc = glGetUniformLocation(shaders, "projection");
    GLuint viewLoc = glGetUniformLocation(shaders, "view");

    /// Lighting-related uniforms
    GLuint ambientStrengthLoc = glGetUniformLocation(shaders, "ambientStrength");
    if(ambientStrengthLoc == -1) {
        printf("failed to get ambientStrength uniform\n");
    }

    GLuint diffuseStrengthLoc = glGetUniformLocation(shaders, "diffuseStrength");
    GLuint lightModelLoc = glGetUniformLocation(shaders, "lightModel");
    if(lightModelLoc == -1) {
        printf("failed to get lightModel uniform\n");
    }
    GLuint objColorLoc = glGetUniformLocation(shaders, "objectColor");
    if(objColorLoc == -1) {
        printf("failed to get objectColor uniform\n");
    }
    // Set the pos of the diffuse light
    GLuint lightPos = glGetUniformLocation(shaders, "lightPos");
    if(lightPos == -1) {
        printf("Failed to get uniform lightPos\n");
    }

    GLuint numLightSourcesLoc = glGetUniformLocation(shaders, "numLightSources");
    if(numLightSourcesLoc == -1) {
        printf("Failed to get uniform lightPos\n");
    }

    // get the grid shader uniform locations
    GLuint schwarzchildRadiusLoc = glGetUniformLocation(g->shaders, "r_s");

    // This gets added to in the init bodies loop below
    int numLightSources = 0;

    // This is sorta-temp code while I figure out how I want to do the
    // translations long-term
    vector3 init_bodies_pos[num_bodies];
    for(int i = 0; i < num_bodies; i++) {

        init_bodies_pos[i] = scene->bodies_array[i]->pos;
        grid_r_s[i] = normalize(scharzchild_radius(scene->bodies_array[i]->mass), 0,
                                SPACE_MAX / 2);
        grid_radius[i] = normalize(scene->bodies_array[i]->radius, 0, SPACE_MAX);

        printf("Schwarzchild Radius of %d = %f, Normalized = %.8lf\n", i + 1,
               scharzchild_radius(scene->bodies_array[i]->mass), grid_r_s[i]);
        printf("Normalized Radius = %f\n", grid_radius[i]);

        // Checking to see if the body is a star, and adding to numLightSources if
        // it is
        if(scene->bodies_array[i]->type == STAR) {
            numLightSources++;
        }
    }

    // light related arrays
    matrix4 lightModel[numLightSources];
    vector3 lightLocations[numLightSources];

    double fps = 0.0;

    glUniform1i(numLightSourcesLoc, numLightSources);

    // TODO! There's too many here
    const double FIXED_DT = (1.0 / 240.0); // this is what we render at
    const double DT = 1;                   // this is our h value in the integration function
    const double MAX_FRAME_TIME = 0.25;
    double accumulator = 0.0;
    double lastTime = glfwGetTime(); // Time of the last debug message
    double lastDebugTime = glfwGetTime();
    float lastFrame = 0.0f; // Time of the last frame
    float deltaTime = 0.0f; // Time between current frame and last frame
    int nbFrames = 0;
    int physicsFrames = 0;
    int run = 0;

    printf("DT = %lf\nEach Physics Frame = %lf seconds in simulation\n", DT, DT);
    printf("1 Second Real-Time = %lf seconds in Sim Time\n", 240 * TIMESKIP);

    while(!glfwWindowShouldClose(window)) {

        // these are just pointers to the scene objects so that my var definitions
        // arent super long
        Camera* cam = scene->cam;

        nbFrames++;
        run++;

        // Frame timer
        double currentTime = glfwGetTime(); // newTime
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        double frameTime = deltaTime;
        if(frameTime > MAX_FRAME_TIME) {
            frameTime = MAX_FRAME_TIME;
        }

        // TODO! Look into why this doesnt work. It was causing a bug where the sim would just stop??
        // accumulator += frameTime * TIMESKIP; // Sim time is renderTime * TIMESKIP. Ex: if timeskip is 5, physics renders at 5x realtime

        accumulator += frameTime;

        enum REFERENCE_FRAME frame = scene->config->ref_frame;

        // step_physics_3d(scene->bodies_array, scene->num_bodies, frame, 0, TIMESKIP); // This is temp

        while(accumulator >= FIXED_DT) {

            for(int i = 0; i < TIMESKIP; i++) {
                step_physics_3d(scene->bodies_array, scene->num_bodies, frame, 0, DT);
                physicsFrames++;
                // sim_t += FIXED_DT;
            }
            accumulator -= FIXED_DT;
        }

        if(currentTime - lastDebugTime >= 1.0 &&
           debug) { // If last prinf() was more than 1 sec ago
            show_debug_message(run, nbFrames, scene->bodies_array, scene->num_bodies);
            if(cam->tracking) {
                vector3 body_pos = normalize_vec3(
                    scene->bodies_array[cam->tracked_body]->pos, SPACE_MIN, SPACE_MAX);
                printf("\ncam pos: (%.3f, %.3f, %.3f) | body pos: (%.3f, %.3f, %.3f) | "
                       "offset: (%.3f, %.3f, %.3f)\n",
                       cam->pos.x, cam->pos.y, cam->pos.z, body_pos.x, body_pos.y,
                       body_pos.z, cam->tracking_vector.r, cam->tracking_vector.az,
                       cam->tracking_vector.el);
            }
            printf("\nPhysics Frames = %d\n", physicsFrames);
            lastDebugTime += 1.0;
            nbFrames = 0;
        }

        if(currentTime - lastDebugTime >= 1.0) {

            scene->framerate = (double)nbFrames / 1.0;
            lastDebugTime += 1.0;
            nbFrames = 0;
        }

        body_3d** bodies_array = scene->bodies_array;

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
        cam->rotSpeed = 4.5f * deltaTime * cam->speedMultiplier;
        cam->front = cameraFront;

        get_input(window, scene);

        // new tracking, capture tracking vector from body to camera in space
        // OR
        // cycled to a different body, recapture the tracking vector
        if((!was_tracking && cam->tracking) ||
           (was_tracking && cam->tracking && cam->tracked_body != prev_tracked)) {
            vector3 normed_track = normalize_vec3(
                scene->bodies_array[cam->tracked_body]->pos, SPACE_MIN, SPACE_MAX);
            cam->tracking_vector =
                cartesian_to_spherical(subtract_vec3s(cam->pos, normed_track));
        }
        // disabled tracking, rsync yaw/pitch from actual look direction
        if(was_tracking && !cam->tracking) {
            vector3 look = vec3_unit_vector(
                scale_vec3(spherical_to_cartesian(cam->tracking_vector), -1.0));
            cam->pitch = asinf(look.y) * RAD_TO_DEG;
            cam->yaw = atan2f(look.z, look.x) * RAD_TO_DEG;
            cameraFront = look;
        }
        // while tracking is on
        if(cam->tracking) {
            body_3d* target = scene->bodies_array[cam->tracked_body];
            vector3 normalized_target =
                normalize_vec3(target->pos, SPACE_MIN, SPACE_MAX);
            cam->pos = add_vec3s(normalized_target,
                                 spherical_to_cartesian(cam->tracking_vector));
            cameraFront =
                vec3_unit_vector(subtract_vec3s(normalized_target, cam->pos));
        }

        // These are the steps to calculte the vectors needed for a lookAt matrix
        vector3 cameraTarget = add_vec3s(cameraFront, cam->pos);
        vector3 cameraDirection =
            vec3_unit_vector(subtract_vec3s(cam->pos, cameraTarget));
        vector3 cameraRight = vec3_unit_vector(cross_product(up, cameraDirection));
        vector3 cameraUp = cross_product(cameraDirection, cameraRight);

        // I want to abstract this away, but it's probably easier to work with in this format?
        matrix4 view = {
            {cameraRight.x, cameraUp.x, cameraDirection.x, 0},
            {cameraRight.y, cameraUp.y, cameraDirection.y, 0},
            {cameraRight.z, cameraUp.z, cameraDirection.z, 0},
            {(-1) * dot_vec3s(cameraRight, cam->pos),
             (-1) * dot_vec3s(cameraUp, cam->pos),
             (-1) * dot_vec3s(cameraDirection, cam->pos), 1}};

        cam->right = cameraRight;
        cam->up = cameraUp;

        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaders);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, (const GLfloat*)projection);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (const GLfloat*)view);

        // this updates the view model as needed for the orbit shaders
        // orbit.vert has location 2 static set to the view matrix
        glUseProgram(orbit_shader);
        glUniformMatrix4fv(2, 1, GL_FALSE, (const GLfloat*)view);

        for(int i = 0; i < scene->num_bodies; i++) {

            body_3d* b = scene->bodies_array[i];

            // Normalized Positions of the bodies
            vector3 n_pos = normalize_vec3(b->pos, SPACE_MIN, SPACE_MAX);

            // vector3 relative_pos = subtract_vec3s(b->pos, cam->pos);

            // orbits updating
            // This variable is probably poorly named, but it essentially puts a line
            // in the orbit path once every N frames where N is the ORBIT_SAMPLING var
            if(scene->config->draw_orbits) {

                int ORBIT_SAMPLING = 50; // TODO! Look into removing this. With decoupled physics and rendering, I think its unneeded?

                if(run % ORBIT_SAMPLING == 0) {
                    updateOrbits(b->orbit, n_pos);
                }

                drawOrbit(b->orbit, orbit_shader);
            }

            if(scene->config->draw_grid) {

                // Get body pos, rounded to the nearest 0.01 ( the res of the grid )
                // vector3 n_pos_grid = { roundf(b_pos.x * 100.0f)/100.0f, 0.0f,
                // roundf(b_pos.z * 100.0f)/100.0f };
                vector3 n_pos_grid = {roundf(n_pos.x * 100.0f) / 100.0f, 0.0f,
                                      roundf(n_pos.z * 100.0f) / 100.0f};
                // Used in the grid vert shader to render Flamm's Parabloid
                planetGridPos[i] = n_pos_grid;
            }

            // These get re-made every time. Should they be? Is there a faster way?
            matrix4 translation;
            matrix4_init_identity(translation);
            matrix4_position_translation(translation, n_pos);

            // X * Y * Z matrixes
            // Quats would probably be better here?
            matrix4 rot_matrix;
            matrix4_rotation_transformation(b->rotation, rot_matrix);

            // total model matrix (translation * rotation)
            matrix4 model;
            matrix4_by_matrix4(translation, rot_matrix, model);

            glUseProgram(shaders);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (const GLfloat*)model);

            // check if texturing is enabled for this object
            if(b->has_texture) {

                glUniform1i(glGetUniformLocation(shaders, "load_texture"),
                            b->has_texture);
                glBindTexture(GL_TEXTURE_2D, b->texture);
            }

            glBindVertexArray(b->vao);

            // TODO: Revisit how I'm doing this lighting
            // Check if a body is defined as a star
            // If yes, give is an ambient strength of max and add it as a light source
            // (iffy)
            if(b->type == STAR) {
                glUniform1f(diffuseStrengthLoc, 1.0f);
                matrix4_copy(model, lightModel[i]);

                // keep it always bright (like a star)
                glUniform1f(ambientStrengthLoc, 1.0f);
                lightLocations[i] = n_pos;

            } else {
                glUniform1f(ambientStrengthLoc, 0.08f);
            }

            // Setup obj color
            glUniform3f(objColorLoc, b->color.r, b->color.g, b->color.b);
            glUniform3fv(lightPos, numLightSources, (const GLfloat*)lightLocations);
            glUniformMatrix4fv(lightModelLoc, numLightSources, GL_FALSE,
                               (const GLfloat*)lightModel);

            int bytes_to_draw = b->resolution;

            // bytes to render. the 6 is the vertices in the quad for the sphere
            glDrawArrays(GL_TRIANGLES, 0, bytes_to_draw);
        }

        if(scene->config->draw_grid) {
            glUseProgram(g->shaders);
            glUniform1fv(10, num_bodies, (const GLfloat*)grid_radius);
            glUniform1fv(schwarzchildRadiusLoc, num_bodies, (const GLfloat*)grid_r_s);
            glUniform3fv(gridPosLoc, num_bodies, (const GLfloat*)planetGridPos);
            draw_grid(g, view, projection);
        }

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    // Free some stuff
    free(scene->cam);

    puts("\nSimulation Ending...");
}
