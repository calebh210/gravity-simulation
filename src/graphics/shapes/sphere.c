#include <stdlib.h>
#include "graphics/shapes/sphere.h"

void drawSphere(vector3 s, float r, int NUM_SEGMENTS, vector3_da *out_vertices, vector3_da *out_normals) {

    // Center of sphere
    float cx = s.x;
    float cy = s.y;
    float cz = s.z;

    int n = NUM_SEGMENTS;
    
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
            vector3_da_push(out_vertices, p1);
            vector3_da_push(out_normals, n1);

            vector3_da_push(out_vertices, p2);
            vector3_da_push(out_normals, n1);

            vector3_da_push(out_vertices, p3);
            vector3_da_push(out_normals, n1);

            // triangle 2
            vector3_da_push(out_vertices, p3);
            vector3_da_push(out_normals, n1);

            vector3_da_push(out_vertices, p4);
            vector3_da_push(out_normals, n1);

            vector3_da_push(out_vertices, p2);
            vector3_da_push(out_normals, n1);

        }

    }

}