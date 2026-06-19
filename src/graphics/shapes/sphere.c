#include <stdlib.h>
#include "graphics/shapes/sphere.h"
#include "utils/constants.h"

// I should probably make this cleaner at some point
// TODO: Implement an Icosphere
// https://songho.ca/opengl/gl_sphere.html

void drawSphere(vector3 s, float r, int sector_count, int stack_count, vector3_da *out_vertices, vector3_da *out_normals, vector2_da *out_uvs) {

    // Center of sphere
    float cx = s.x;
    float cy = s.y;
    float cz = s.z;

    float sectorStep = 2 * PI / sector_count;
    float stackStep = PI / stack_count;
    
    for (int i = 0; i < stack_count  ; i++) {

        float stackAngle = i * stackStep;
        float stackAngleNext = (i + 1) * stackStep;

        // float xy = r * cosf(stackAngle);

        for(int j = 0; j < sector_count; j++){

            float sectorAngle = j * sectorStep;
            float sectorAngleNext = (j + 1) * sectorStep;

            float x1 = cx + (sinf(stackAngle) * cosf(sectorAngle) * r);
            float y1 = cy + (sinf(stackAngle) * sinf(sectorAngle) * r);
            float z1 = cz + (cosf(stackAngle) * r);

            float x2 = cx + (sinf(stackAngleNext) * cosf(sectorAngle) * r);
            float y2 = cy + (sinf(stackAngleNext) * sinf(sectorAngle) * r);
            float z2 = cz + (cosf(stackAngleNext) * r);

            float x3 = cx + (sinf(stackAngle) * cosf(sectorAngleNext) * r);
            float y3 = cy + (sinf(stackAngle) * sinf(sectorAngleNext) * r);
            float z3 = cz + (cosf(stackAngle) * r);

            float x4 = cx + (sinf(stackAngleNext) * cosf(sectorAngleNext) * r);
            float y4 = cy + (sinf(stackAngleNext) * sinf(sectorAngleNext) * r);
            float z4 = cz + (cosf(stackAngleNext) * r);


            vector3 p1 = {x1, y1, z1};
            vector3 p2 = {x2, y2, z2};
            vector3 p3 = {x3, y3, z3};
            vector3 p4 = {x4, y4, z4};


            //normals for the mesh
 
            vector3 n1 = vec3_unit_vector(subtract_vec3s(p1, s));
            vector3 n2 = vec3_unit_vector(subtract_vec3s(p2, s));
            vector3 n3 = vec3_unit_vector(subtract_vec3s(p3, s));
            vector3 n4 = vec3_unit_vector(subtract_vec3s(p4, s));
            
            // triangle 1
            vector3_da_push(out_vertices, p1);
            vector3_da_push(out_normals, n1);

            vector3_da_push(out_vertices, p2);
            vector3_da_push(out_normals, n2);

            vector3_da_push(out_vertices, p3);
            vector3_da_push(out_normals, n3);

            // triangle 2
            vector3_da_push(out_vertices, p3);
            vector3_da_push(out_normals, n3);

            vector3_da_push(out_vertices, p4);
            vector3_da_push(out_normals, n4);

            vector3_da_push(out_vertices, p2);
            vector3_da_push(out_normals, n2);


            float s0 = (float)j / sector_count;
            float s1 = (float)(j + 1) / sector_count;

            float t0 = (float)i / stack_count;
            float t1 = (float)(i + 1) / stack_count;

            vector2 uv1 = {s0, t0};
            vector2 uv2 = {s0, t1};
            vector2 uv3 = {s1, t0};
            vector2 uv4 = {s1, t1};

            vector2_da_push(out_uvs, uv1);
            vector2_da_push(out_uvs, uv2);
            vector2_da_push(out_uvs, uv3);
            vector2_da_push(out_uvs, uv3);
            vector2_da_push(out_uvs, uv4);
            vector2_da_push(out_uvs, uv2);

            

        }

    }

}


