#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/shaders_parser.h"

// parse the shader file locating in shaders/*
char* parse_shader_file(const char* fpath){

    FILE *fh = fopen(fpath, "rb");

    if (fh == NULL) {
        printf("vertex file not found");
        return NULL;
    }

    int fsize;

    fseek( fh, 0, SEEK_END );
    fsize = ftell( fh );
    rewind( fh );

    char* vertex_shader = malloc(fsize + 1);

    fread(vertex_shader, sizeof( char ), fsize, fh);

    vertex_shader[fsize] = '\0';

    fclose(fh);

    return vertex_shader;

}

