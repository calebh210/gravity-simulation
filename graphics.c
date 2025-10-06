#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846f

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

void drawTriangle(){

glBegin(GL_TRIANGLES);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);
glEnd();

}

int render(float pos[4]) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    GLFWwindow* window = glfwCreateWindow(1000, 800, "OpenGL Window", NULL, NULL);
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
    glViewport(0, 0, 1000, 800);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);  // 2D orthographic projection
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Clear the screen
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Set circle color
        glColor3f(1.0f, 0.5f, 0.2f); 

        drawCircle(pos[0], pos[1], 0.01f, 100);

        glColor3f(0.2f, 0.7f, 1.0f);  
        drawCircle(pos[2], pos[3], 0.01f, 100);

        glColor3f(1.0f, 0.5f, 0.2f); 
        drawTriangle();


        glfwSwapBuffers(window);

        glfwPollEvents();
    }


    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

