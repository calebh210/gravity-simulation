#ifndef PARSER_H
#define PARSER_H

#include "utils/structures.h"

typedef struct Settings{
    bool draw_grid;
    bool draw_orbits;
    int ref_frame_code;
    float time_delta;
    bool debug;
    int num_bodies;
    char* font;
} Settings;

Settings* parse_config_file(char* filename, body_t* bodies_array[], bool is_3d, int NUM_BODIES);

#endif
