#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "graphics/grid.h"
#include "math/matrix/matrix4.h"
#include "utils/shaders_parser.h"

void init_grid(Grid *g){

    GLuint vao, vbo;

    glGenVertexArrays( 1, &vao );
    g->vao = vao;
    glBindVertexArray( g->vao );

    glGenBuffers( 1, &vbo );
    g->vbo = vbo;


    // I admit, the generation code isn't the cleanest, but it works ¯\_(ツ)_/¯
    // I need to look into an index buffer, a lot of this is re-used

    // The "step" is how far apart to draw each row/col
    // x and z define the bounds to draw
    // y is hardcoded to 0 as the reference point
    // CHANGING THESE REQUIRES A MANUAL CHANGE TO glDrawArrays() in draw_grid!
    float step = 0.05f;

    float x_init = -5.0f;
    float z_init = -5.0f;

    int rows = (int)ceilf((fabsf(x_init) + fabsf(z_init)) / step);

    int vertices = (2 * rows * rows) * 2;

    vector3* points = malloc(vertices * sizeof(vector3));

    int index = 0;

    for(int i=0; i < rows; i++){ 
        float x = x_init + (i * step);
        for(int j=0; j < rows; j++){
            float z = z_init + (j * step);
            points[index++] = (vector3){ x, 0.0f, z};
            z = z + step;
            points[index++] = (vector3){ x, 0.0f, z};
        }
    }

    for(int i=0; i < rows; i++){ 
        float z = z_init + (i * step);
        for(int j=0; j < rows; j++){
            float x = x_init + (j * step);
            points[index++] = (vector3){ x, 0.0f, z};
            x = x + step;
            points[index++] = (vector3){ x, 0.0f, z};
        }
    }

    glBindBuffer( GL_ARRAY_BUFFER, g->vbo );
    glBufferData( GL_ARRAY_BUFFER, vertices * sizeof(vector3), points, GL_STATIC_DRAW );

    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(vector3), (void*)0);
    glBindVertexArray(0);

    free(points); // loaded in the vbo, doesn't need to be on heap anymore

    // Shader init
    const char* vert_path = "shaders/grid.vert";
    char* vertex_shader = parse_shader_file(vert_path);

    const char* frag_path = "shaders/grid.frag";
    char* fragment_shader = parse_shader_file(frag_path);

    // vert shader setup / check
    GLuint vs = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vs, 1, (const GLchar**)&vertex_shader, NULL );
    glCompileShader( vs );
    free(vertex_shader);

    GLint success;

    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    { 
        printf("Failed to compile vertex shader.\n");
        return;
    }

    // frag shader setup / check
    GLuint fs = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fs, 1, (const GLchar**)&fragment_shader, NULL );
    glCompileShader( fs );
    free(fragment_shader);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    { 
        printf("Failed to compile fragment shader.\n");
        return;
    }
    
    // linking grid shader to the program
    GLuint grid_shaders = glCreateProgram();
    glAttachShader( grid_shaders, fs );
    glAttachShader( grid_shaders, vs );
    glLinkProgram( grid_shaders );

    glGetProgramiv(grid_shaders, GL_LINK_STATUS, &success);
    if (success != GL_TRUE)
    {
        printf("Failed to link grid shader program.\n");
        glDeleteProgram(grid_shaders);
        return;
    }

    //store the shaders in the grid object
    g->shaders = grid_shaders;
}


void draw_grid(Grid *g,matrix4 view, matrix4 projection){

    GLuint grid_shaders = g->shaders;

    glUseProgram( grid_shaders );

    GLuint projLoc = glGetUniformLocation(grid_shaders, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, (const GLfloat *)projection);
    if(projLoc == -1){
        printf("Failed to get uniform projection in grid shader\n");
    }

    GLuint viewLoc = glGetUniformLocation(grid_shaders, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (const GLfloat *)view);
        if(viewLoc == -1){
        printf("Failed to get uniform view in grid shader\n");
    }

    matrix4 grid_model;
    matrix4_init_identity(grid_model);

    GLuint modelLoc = glGetUniformLocation(grid_shaders, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (const GLfloat *)grid_model);
        if(modelLoc == -1){
        printf("Failed to get uniform model in grid shader\n");
    }

    // Magnitude is used to amplify Flamm's paraboloid, since for most objects (like the sun)
    // the curvature is so small it cannot be seen
    GLuint magnitudeLoc = glGetUniformLocation(g->shaders, "magnitude");
    if(magnitudeLoc == -1){
        printf("Failed to get uniform magnitude\n");
    }
    glUniform1f(magnitudeLoc, 1.0f);

    glBindVertexArray( g->vao );
    
    // THE DRAW BYTES ARE HARDCODED
    glDrawArrays(GL_LINES, 0, 160000);

}