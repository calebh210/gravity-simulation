#include <math.h>
#include <stdlib.h>
#include "graphics/shapes/circle.h"
#include "utils/constants.h"

// Function to draw a circle at (cx, cy) with radius
[[gnu::pure]] double* drawCircle(vector2 c, float r, int num_segments) {

    // Center of circle
    double cx = c.x;
    double cy = c.y;

    int num_vertices = (num_segments + 1) * 3;

    int idx = 0;

    double* vertices = malloc(num_vertices * sizeof(double));

    for(int i = 0; i <= num_segments; i++) {
        float angle = 2.0f * PI * i / num_segments;
        double x = cx + cosf(angle) * r;
        double y = cy + sinf(angle) * r;

        vertices[idx] = x;
        vertices[idx + 1] = y;
        vertices[idx + 2] = 0.0;

        idx += 3;
    }

    return vertices;
}