#include "graphics/common.h"
#include "utils/shaders_parser.h"
#include "graphics/camera.h"
#include "graphics/render_rt.h"
#include "math/vector/vector4.h"
#include "utils/constants.h"
#include "math/math_funcs.h"
#include "physics/gravity3d.h"

// void upload_camera_ubo(Camera cam){

//     struct CameraUBO{
//         vector4 cameraPos;
//         vector4 cameraDirection;
//         vector4 cameraRight;
//         vector4 cameraUp;
//         vector4 tanHalfFov;
//     } cameraUBOdata;

//     vector3 up = {0.0f, 1.0f, 0.0f};

//     // vector3 cameraFront = vec3_unit_vector(cam.direction);

//     //These are the steps to calculte the vectors needed for a lookAt matrix
//     // vector3 cameraTarget = add_vec3s(cameraFront, cam.pos);

//     cam->pitch = 0.0f;  
//     cam->yaw = -90.0f;

//     float yaw = cam->yaw * DEG_TO_RAD;
//     float pitch = cam->pitch * DEG_TO_RAD;

//     // This is used to handle rotation of the camera
//     vector3 direction;
//     direction.x = cos(yaw) * cos(pitch);
//     direction.y = sin(pitch);
//     direction.z = sin(yaw) * cos(pitch);
    
//     vector3 cameraDirection = vec3_unit_vector(cam.direction);
//     vector3 cameraRight = vec3_unit_vector(cross_product(cameraDirection, up));
//     vector3 cameraUp = cross_product(cameraDirection, cameraRight);

//     vector4 tanHalfFov = { tanf((60.0f * 0.5f) * M_PI / 180.0f), 0, 0, 0 };

//     cameraUBOdata.cameraPos = (vector4){cam.pos.x, cam.pos.y, cam.pos.z, 0};
//     // printf("camera pos = %f, %f %f\n", cam.pos.x, cam.pos.y, cam.pos.z);
//     cameraUBOdata.cameraDirection = (vector4){cameraDirection.x, cameraDirection.y, cameraDirection.z, 0};
//     cameraUBOdata.cameraRight = (vector4){cameraRight.x, cameraRight.y, cameraRight.z, 0};
//     cameraUBOdata.cameraUp = (vector4){cameraUp.x, cameraUp.y, cameraUp.z, 0};
//     cameraUBOdata.tanHalfFov = tanHalfFov; // why not just put this in comp shader?

//     glBindBuffer(GL_UNIFORM_BUFFER, cam.ubo);
//     glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(cameraUBOdata), &cameraUBOdata);

// }

int render_rt(Scene* scene) {

    GLFWwindow* window = init_render();

    // TODO! un-hardcode this later
    const char* fpath = "shaders/rt/rt.vert";
    char* vertex_shader = parse_shader_file(fpath);

    const char* frag_path = "shaders/rt/rt.frag";
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

    glClearColor(0.0f, 0.2f, 0.2f, 1.0f);
    GLuint vao, vbo;


    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,  // top left
        -1.0f, -1.0f,  0.0f, 0.0f,  // bottom left
        1.0f, -1.0f,  1.0f, 0.0f,  // bottom right

        -1.0f,  1.0f,  0.0f, 1.0f,  // top left
        1.0f, -1.0f,  1.0f, 0.0f,  // bottom right
        1.0f,  1.0f,  1.0f, 1.0f   // top right

    };

    glGenBuffers( 1, &vbo );
    glGenVertexArrays( 1, &vao );

    glBindVertexArray( vao );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);


    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // dimensions of the image
    int tex_w = 512, tex_h = 512;
    GLuint tex_output;
    glGenTextures(1, &tex_output);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_output);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT,
    NULL);
    glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    int work_grp_cnt[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

    printf("max global (total) work group counts x:%i y:%i z:%i\n",
    work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

    int work_grp_size[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
    
    GLuint work_grp_inv;
    printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
    work_grp_size[0], work_grp_size[1], work_grp_size[2]);

    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
    printf("max local work group invocations %i\n", work_grp_inv);

    char* ray_shader_string = parse_shader_file("shaders/rt/rt.comp");
    GLuint ray_shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(ray_shader, 1, (const GLchar**)&ray_shader_string, NULL);
    glCompileShader(ray_shader);
    // check for compilation errors as per normal here

    glGetShaderiv(ray_shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    { 
        printf("Failed to compile ray shader.\n");
        return -1; 
    }

    GLuint ray_program = glCreateProgram();
    glAttachShader(ray_program, ray_shader);
    glLinkProgram(ray_program); 

    unsigned int uniformBlock = glGetUniformBlockIndex(ray_program, "Bodies");
    glUniformBlockBinding(ray_program,    uniformBlock, 1);

    unsigned int uboBodies;
    glGenBuffers(1, &uboBodies);


    unsigned int cameraBlock = glGetUniformBlockIndex(ray_program, "Camera");
    glUniformBlockBinding(ray_program,    cameraBlock, 2);

    GLuint cameraUBO = 0;
    glGenBuffers(2, &cameraUBO);


    // Hard capped at 16 per scene currently
    vector4 bodiesColor[16];
    vector4 bodiesPos[16];
    // Even though this should just be a float, std 140 padding is weird and requires a 16byte stride for scalars
    vector4 bodiesRadii[16];

    for(int i =0; i < scene->num_bodies; i++){

        body_3d *test_body = scene->bodies_array[i];

        printf("Sim coords for body = %f, %f, %f\n", test_body->pos.x,test_body->pos.y,test_body->pos.z);

        vector3 test_body_pos = normalize_vec3(test_body->pos, SPACE_MIN, SPACE_MAX);

        bodiesPos[i] = (vector4){test_body_pos.x,test_body_pos.y,test_body_pos.z,0};

        printf("Normalized coords for body = %f, %f, %f\n", test_body_pos.x,test_body_pos.y,test_body_pos.z);

        bodiesColor[i] = (vector4){test_body->color.r,test_body->color.g,test_body->color.b,1.0};

        bodiesRadii[i] = (vector4){1.0, 0, 0, 0};

    }

    // TODO: Make all this bottom shit work

    size_t bodiesColorSize = sizeof(bodiesColor);
    size_t bodiesPosSize = sizeof(bodiesPos);
    size_t bodiesRadiiSize = sizeof(bodiesRadii);

    size_t TOTAL_SIZE = bodiesColorSize + bodiesPosSize + bodiesRadiiSize;

    printf("Total Size of UBO = %d", TOTAL_SIZE);

    glBindBuffer(GL_UNIFORM_BUFFER, uboBodies);
    glBufferData(GL_UNIFORM_BUFFER, TOTAL_SIZE, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 1);
    
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, uboBodies, 0, TOTAL_SIZE);

    glBindBuffer(GL_UNIFORM_BUFFER, uboBodies);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, bodiesColorSize, &bodiesColor);
    glBindBuffer(GL_UNIFORM_BUFFER, 1);

    glBindBuffer(GL_UNIFORM_BUFFER, uboBodies);
    glBufferSubData(GL_UNIFORM_BUFFER, bodiesColorSize, bodiesPosSize, &bodiesPos);
    glBindBuffer(GL_UNIFORM_BUFFER, 1);

    glBindBuffer(GL_UNIFORM_BUFFER, uboBodies);
    glBufferSubData(GL_UNIFORM_BUFFER, bodiesColorSize + bodiesPosSize, bodiesRadiiSize, &bodiesRadii);
    glBindBuffer(GL_UNIFORM_BUFFER, 1);

    Camera cam;
    cam.pos = (vector3){0.1f, 0.1f, 10.0f};
    cam.pitch = 0.0f;  
    cam.yaw = -90.0f;

    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferData(GL_UNIFORM_BUFFER, 80, NULL, GL_DYNAMIC_DRAW); 
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, cameraUBO); // binding = 2 matches shader


    glfwSwapInterval(0);

    while( !glfwWindowShouldClose(window) ){


        float sim_dt = 100.0f;
        // TODO!! Clean this up. For debug only
        rk4_nbody_3d(0, sim_dt, scene->bodies_array, scene->num_bodies);

            // Hard capped at 16 per scene currently
    vector4 bodiesColor[16];
    vector4 bodiesPos[16];
    // Even though this should just be a float, std 140 padding is weird and requires a 16byte stride for scalars
    vector4 bodiesRadii[16];

    for(int i =0; i < scene->num_bodies; i++){

        body_3d *test_body = scene->bodies_array[i];

        printf("Sim coords for body = %f, %f, %f\n", test_body->pos.x,test_body->pos.y,test_body->pos.z);

        vector3 test_body_pos = normalize_vec3(test_body->pos, SPACE_MIN, SPACE_MAX);

        bodiesPos[i] = (vector4){test_body_pos.x,test_body_pos.y,test_body_pos.z,0};

        printf("Normalized coords for body = %f, %f, %f\n", test_body_pos.x,test_body_pos.y,test_body_pos.z);

        bodiesColor[i] = (vector4){test_body->color.r,test_body->color.g,test_body->color.b,1.0};

        bodiesRadii[i] = (vector4){1.0, 0, 0, 0};

    }

    // TODO: Make all this bottom shit work

    size_t bodiesColorSize = sizeof(bodiesColor);
    size_t bodiesPosSize = sizeof(bodiesPos);
    size_t bodiesRadiiSize = sizeof(bodiesRadii);

    size_t TOTAL_SIZE = bodiesColorSize + bodiesPosSize + bodiesRadiiSize;

    printf("Total Size of UBO = %d", TOTAL_SIZE);

    glBindBuffer(GL_UNIFORM_BUFFER, uboBodies);
    glBufferData(GL_UNIFORM_BUFFER, TOTAL_SIZE, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 1);
    
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, uboBodies, 0, TOTAL_SIZE);

    glBindBuffer(GL_UNIFORM_BUFFER, uboBodies);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, bodiesColorSize, &bodiesColor);
    glBindBuffer(GL_UNIFORM_BUFFER, 1);

    glBindBuffer(GL_UNIFORM_BUFFER, uboBodies);
    glBufferSubData(GL_UNIFORM_BUFFER, bodiesColorSize, bodiesPosSize, &bodiesPos);
    glBindBuffer(GL_UNIFORM_BUFFER, 1);

    glBindBuffer(GL_UNIFORM_BUFFER, uboBodies);
    glBufferSubData(GL_UNIFORM_BUFFER, bodiesColorSize + bodiesPosSize, bodiesRadiiSize, &bodiesRadii);
    glBindBuffer(GL_UNIFORM_BUFFER, 1);


        { // launch compute shaders!
            glUseProgram(ray_program);
            glDispatchCompute((GLuint)tex_w, (GLuint)tex_h, 1);
        }

        //make sure writing to image has finished before read
        glMemoryBarrier(
            GL_SHADER_IMAGE_ACCESS_BARRIER_BIT |
            GL_TEXTURE_FETCH_BARRIER_BIT
        );

        glBindBuffer(GL_UNIFORM_BUFFER, uboBodies);

    struct CameraUBO{
        vector4 cameraPos;
        vector4 cameraDirection;
        vector4 cameraRight;
        vector4 cameraUp;
        vector4 tanHalfFov;
    } cameraUBOdata;

        vector3 up = {0.0f, 1.0f, 0.0f};

        // vector3 cameraFront = vec3_unit_vector(cam.direction);

        //These are the steps to calculte the vectors needed for a lookAt matrix
        // vector3 cameraTarget = add_vec3s(cameraFront, cam.pos);

        float yaw = cam.yaw * DEG_TO_RAD;
        float pitch = cam.pitch * DEG_TO_RAD;

        // This is used to handle rotation of the camera
        vector3 direction;
        direction.x = cos(yaw) * cos(pitch);
        direction.y = sin(pitch);
        direction.z = sin(yaw) * cos(pitch);


        // direction is the CAMERA direction, 
        vector3 cameraDirection = vec3_unit_vector(direction);
        vector3 cameraRight = vec3_unit_vector(cross_product(cameraDirection, up));
        vector3 cameraUp = cross_product(cameraDirection, cameraRight);

        vector4 tanHalfFov = { tanf((60.0f * 0.5f) * M_PI / 180.0f), 0, 0, 0 };

        cameraUBOdata.cameraPos = (vector4){cam.pos.x, cam.pos.y, cam.pos.z, 0};
        // printf("camera pos = %f, %f %f\n", cam.pos.x, cam.pos.y, cam.pos.z);
        cameraUBOdata.cameraDirection = (vector4){cameraDirection.x, cameraDirection.y, cameraDirection.z, 0};
        cameraUBOdata.cameraRight = (vector4){cameraRight.x, cameraRight.y, cameraRight.z, 0};
        cameraUBOdata.cameraUp = (vector4){cameraUp.x, cameraUp.y, cameraUp.z, 0};
        cameraUBOdata.tanHalfFov = tanHalfFov; // why not just put this in comp shader?

        glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(cameraUBOdata), &cameraUBOdata);

               { // normal drawing pass
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(shader_program);
            glBindVertexArray(vao);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex_output);
            glDisable(GL_DEPTH_TEST);  // draw as background
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glEnable(GL_DEPTH_TEST);
        }
            // Wipe the drawing surface clear.
            // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glfwSwapBuffers( window );
            glfwPollEvents();


    }


}