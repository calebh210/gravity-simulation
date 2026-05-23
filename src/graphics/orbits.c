#include "graphics/orbits.h"
#include <stdlib.h>
#include <stdio.h>
#include <glad.h>
#include <GLFW/glfw3.h>

#include "utils/shaders_parser.h"

// All the functions to draw the orbit lines
// These are common between the 2D and 3D scenes

// Init the orbit lists
Orbit* initOrbit(){

    GLuint vao, vbo;
    Orbit* orbit = malloc(sizeof(orbit));
    
    orbit->points = init_list();
    orbit->vao = vao;
    orbit->vbo = vbo;
    // Loads the orbit vertex and frag shader
    // orbit->shaders = init_orbit_shaders();

    //this can overflow
    int ORBIT_LIMIT = 10000;
    
    glGenBuffers( 1, &orbit->vbo );
    glGenVertexArrays( 1, &orbit->vao );

    glBindVertexArray(orbit->vao);
    glBindBuffer(GL_ARRAY_BUFFER, orbit->vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vector3) * ORBIT_LIMIT,
                 NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vector3), (void*)0);
    glEnableVertexAttribArray(0);

    return orbit;
}

// Re-draw the orbits
void updateOrbits(Orbit* orbit, vector3 coords){

    //this can overflow
    int ORBIT_LIMIT = 10000;

    point *new_point = ( point * )malloc(sizeof(point));
    new_point->pos = coords;
    new_point->next = NULL;

    add_to_list(orbit->points, new_point);

    //Check the buffer isn't being overflown
    // Stop drawing for now, eventually overwrite old ones
    if(orbit->points->count >= ORBIT_LIMIT){
        return;
    }

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, orbit->vbo);
    // Add data to the VBO
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vector3) * (orbit->points->count - 1), sizeof(vector3), &new_point->pos);

}

//trying to draw an orbit path
void drawOrbit(Orbit* orbit, GLuint shader){
    glUseProgram(shader);
    glBindVertexArray(orbit->vao);
    glBindBuffer(GL_ARRAY_BUFFER, orbit->vbo);
    glDrawArrays(GL_POINTS, 0, orbit->points->count);
}

GLuint init_orbit_shaders(){
    // The orbit shaders
    const char* orbit_frag = "shaders/orbit.frag";
    char* orbit_frag_shader = parse_shader_file(orbit_frag);

    const char* orbit_vertex = "shaders/orbit.vert";
    char* orbit_vert_shader = parse_shader_file(orbit_vertex);

    GLuint orbit_vs = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( orbit_vs, 1, (const GLchar**)&orbit_vert_shader, NULL );
    glCompileShader( orbit_vs );

    GLuint orbit_fs = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( orbit_fs, 1, (const GLchar**)&orbit_frag_shader, NULL );
    glCompileShader( orbit_fs );

    GLuint orbit_shader = glCreateProgram();
    glAttachShader( orbit_shader, orbit_fs );
    glAttachShader( orbit_shader, orbit_vs );
    glLinkProgram( orbit_shader );


    if(orbit_shader == -1){
        printf("Failed to compile orbit shaders. Exiting...\n");
        return -1;
    }

    return orbit_shader;

}