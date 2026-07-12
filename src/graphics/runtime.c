#include <string.h>
#include "graphics/orbits.h"
#include "graphics/render3d.h"
#include "graphics/grid.h"
#include "graphics/runtime.h"
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

// add a new body to the sim during runtime
void add_new_body(Scene* scene) {

    scene->num_bodies++;

    body_3d** new_bodies_array = realloc(scene->bodies_array, sizeof(body_3d) * scene->num_bodies);

    scene->bodies_array = new_bodies_array;

    // need to setup new body here
    body_3d* b = malloc(sizeof(body_3d));

    glGenBuffers(1, &b->vbo);
    glGenVertexArrays(1, &b->vao);

    float n;

    printf("Enter X Pos for new obj: \n");

    scanf("%f", &n);

    b->pos.x = n;
    b->pos.y = 0;
    b->pos.z = 0;

    b->velocity.x = 0;
    b->velocity.y = 0;
    b->velocity.z = 0;

    b->name = "test";

    b->color.r = 0.5f;
    b->color.g = 0.5f;
    b->color.b = 0.5f;

    double mass;
    printf("Enter Mass for new obj: \n");
    scanf("%lf", &mass);

    b->radius = 100E8;
    b->mass = mass;

    b->has_texture = false;
    b->has_model = false;

    b->texture = 0;

    vector3_da vertices;
    vector2_da uvs;
    vector3_da normals;

    vector3_da_init(&vertices);
    vector2_da_init(&uvs);
    vector3_da_init(&normals);

    int sector_count = 36; // How many segments in da spheres
    int stack_count = 18;
    b->resolution = (sector_count * stack_count) * 6; // * 6 for the num of vertices in each quad

    drawSphere(normalize(b->radius, 0, SPACE_MAX), sector_count, stack_count, &vertices, &normals, &uvs);

    glBindBuffer(GL_ARRAY_BUFFER, b->vbo);

    size_t total_size = (vertices.size + normals.size) * sizeof(vector3) + (uvs.size * sizeof(vector2));

    glBufferData(GL_ARRAY_BUFFER, total_size, NULL, GL_STATIC_DRAW);

    // Add the vertices / normals to the buffer
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size * sizeof(vector3), &vertices.buf[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vertices.size * sizeof(vector3), normals.size * sizeof(vector3), &normals.buf[0]);
    glBufferSubData(GL_ARRAY_BUFFER, (vertices.size + normals.size) * sizeof(vector3), uvs.size * sizeof(vector2), &uvs.buf[0]);

    glBindVertexArray(b->vao);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(vertices.size * sizeof(vector3)));

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)((vertices.size + normals.size) * sizeof(vector3)));
    glEnableVertexAttribArray(2);

    scene->bodies_array[(scene->num_bodies - 1)] = b;
}