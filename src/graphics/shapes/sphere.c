#include <stdlib.h>
#include "graphics/shapes/sphere.h"
#include "utils/constants.h"

// I should probably make this cleaner at some point
// TODO: Implement an Icosphere
// https://songho.ca/opengl/gl_sphere.html

void drawSphere(float r, int sector_count, int stack_count, vector3_da* out_vertices, vector3_da* out_normals, vector2_da* out_uvs) {

    float sectorStep = 2 * PI / sector_count;
    float stackStep = PI / stack_count;

    for(int i = 0; i < stack_count; i++) {

        float stackAngle = i * stackStep;
        float stackAngleNext = (i + 1) * stackStep;

        // float xy = r * cosf(stackAngle);

        for(int j = 0; j < sector_count; j++) {

            float sectorAngle = j * sectorStep;
            float sectorAngleNext = (j + 1) * sectorStep;

            float x1 = (sinf(stackAngle) * cosf(sectorAngle) * r);
            float y1 = (sinf(stackAngle) * sinf(sectorAngle) * r);
            float z1 = (cosf(stackAngle) * r);

            float x2 = (sinf(stackAngleNext) * cosf(sectorAngle) * r);
            float y2 = (sinf(stackAngleNext) * sinf(sectorAngle) * r);
            float z2 = (cosf(stackAngleNext) * r);

            float x3 = (sinf(stackAngle) * cosf(sectorAngleNext) * r);
            float y3 = (sinf(stackAngle) * sinf(sectorAngleNext) * r);
            float z3 = (cosf(stackAngle) * r);

            float x4 = (sinf(stackAngleNext) * cosf(sectorAngleNext) * r);
            float y4 = (sinf(stackAngleNext) * sinf(sectorAngleNext) * r);
            float z4 = (cosf(stackAngleNext) * r);

            vector3 p1 = {x1, y1, z1};
            vector3 p2 = {x2, y2, z2};
            vector3 p3 = {x3, y3, z3};
            vector3 p4 = {x4, y4, z4};

            // for an openGL sphere the center is always locally 0
            vector3 center = {0, 0, 0};

            // normals for the mesh
            vector3 n1 = vec3_unit_vector(subtract_vec3s(p1, center));
            vector3 n2 = vec3_unit_vector(subtract_vec3s(p2, center));
            vector3 n3 = vec3_unit_vector(subtract_vec3s(p3, center));
            vector3 n4 = vec3_unit_vector(subtract_vec3s(p4, center));

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
