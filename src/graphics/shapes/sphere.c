#include "graphics/shapes/sphere.h"
#include <stdlib.h>

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