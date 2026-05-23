#include "graphics/controls.h"
#include "graphics/render3d.h"
#include "utils/constants.h"

// TODO add helper to make a new input button

// TODO add helper to make a new input button

// All the if statements for keypresses in GLFW
// Updates the camera struct as needed
void get_input(GLFWwindow* window, Camera* cam){
    
    vector3 cameraPosDefault = {0, 0.4f,1.5f};

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    // move camera forward / back
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        if (!cam->tracking) {
            cam->pos = add_vec3s(cam->pos, scale_vec3(cam->front, cam->speed));
        } else {
            if (cam->tracking_vector.r < 0.01f) {
                cam->tracking_vector.r = 0.01f;
            } else {
                cam->tracking_vector.r -= cam->speed;
            }
        }
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        if (!cam->tracking) {
            cam->pos = add_vec3s(cam->pos, scale_vec3(cam->front, (-1) * cam->speed));
        } else {
            cam->tracking_vector.r += cam->speed;
        }
    }
    // move camera left / right
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        // Splitting this into two lines to make it a bit cleaner
        if (!cam->tracking) {
            vector3 temp = vec3_unit_vector(cross_product(cam->front, cam->up)); 
            cam->pos = add_vec3s(cam->pos, scale_vec3(temp, (-1) * cam->speed));
        } else {
            cam->tracking_vector.az += cam->rotSpeed * DEG_TO_RAD;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        if (!cam->tracking) {
            vector3 temp = vec3_unit_vector(cross_product(cam->front, cam->up)); 
            cam->pos = add_vec3s(cam->pos, scale_vec3(temp, cam->speed));    
        } else {
            cam->tracking_vector.az -= cam->rotSpeed * DEG_TO_RAD;
        }
    }
    // move cam up
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
        if (!cam->tracking) {
            cam->pos = add_vec3s(cam->pos, scale_vec3(cam->up, (-1) * cam->speed));
        }
    }
    // move cam down
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        if (!cam->tracking) {
            cam->pos = add_vec3s(cam->pos, scale_vec3(cam->up, cam->speed));
        }
    }

    // x rotation
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS){
        if (!cam->tracking) {
            cam->pitch -= cam->rotSpeed;
        } else {
            if (cam->tracking_vector.el < 0.01f) {
                cam->tracking_vector.el = 0.01f;
            } else {
                cam->tracking_vector.el -= cam->rotSpeed * DEG_TO_RAD;
            }
        }
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS){
        if (!cam->tracking) {
            cam->pitch += cam->rotSpeed;
        } else {
            if (cam->tracking_vector.el > PI - 0.01f) {
                cam->tracking_vector.el = PI - 0.01f;
            } else {
                cam->tracking_vector.el += cam->rotSpeed * DEG_TO_RAD;
            }
        }
    }

    // y rotation
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS){
        cam->yaw -= cam->rotSpeed; 
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS){
        cam->yaw += cam->rotSpeed; 
    }

    // track body
    static bool t_was_pressed = false;
    bool t_pressed = glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS;
    if (t_pressed && !t_was_pressed){
        cam->tracking = !cam->tracking;
    }
    t_was_pressed = t_pressed;

    // track prev body
    static bool left_was_pressed = false;
    bool left_pressed = glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS;
    if (left_pressed && !left_was_pressed && cam->tracking){
        cam->tracked_body = (cam->tracked_body - 1 + cam->num_bodies) % cam->num_bodies;
    }
    left_was_pressed = left_pressed;

    // track next body
    static bool right_was_pressed = false;
    bool right_pressed = glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS;
    if (right_pressed && !right_was_pressed && cam->tracking){
        cam->tracked_body = (cam->tracked_body + 1) % cam->num_bodies;
    }
    right_was_pressed = right_pressed;

    // this adjusts the camera speed
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){

        //Prevent the print statement from popping up hundreds of time per press
        if(cam->speedMultiplier != 1){
            cam->speedMultiplier = 1;
            puts("\nCamera speed set to 1x");
        }
    }
    // 2x speed
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS){

        //Prevent the print statement from popping up hundreds of time per press
        if(cam->speedMultiplier != 2){
            cam->speedMultiplier = 2;
            puts("\nCamera speed set to 2x");
        }
    }
    // 5x speed
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS){
        //Prevent the print statement from popping up hundreds of time per press
        if(cam->speedMultiplier != 5){
            cam->speedMultiplier = 5;
            puts("\nCamera speed set to 5x");
        }
    }

    // reset camera to init
    if (glfwGetKey(window, GLFW_KEY_R)){
        cam->pos = cameraPosDefault;
        // Also need to reset the rotation
        cam->yaw = -90.0f;
    }

    // Don't let the user's flip the camera over, it breaks things
    if(cam->pitch > 89.0f)
        cam->pitch =  89.0f;
    if(cam->pitch < -89.0f)
        cam->pitch = -89.0f;

        
}
