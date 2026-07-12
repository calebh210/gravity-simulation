#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "utils/config_parser.h"
#include "utils/help.h"
#include "math/math_funcs.h"
#include "physics/cr3bp.h"
#include "graphics/render3d.h"
#include "graphics/render.h"
#include "graphics/body.h"
#include "graphics/scene.h"

// Take the string argument for render_mode. Validate that it is correct
//  Return an int code to make it easier to work with
enum REFERENCE_FRAME validate_rendering_mode(char* REF_FRAME) {

    enum REFERENCE_FRAME frame;

    if(strcmp(REF_FRAME, "inertial") == 0) {
        frame = INERTIAL;
    } else if(strcmp(REF_FRAME, "cog") == 0) {
        frame = CENTER_OF_GRAVITY;
    } else if(strcmp(REF_FRAME, "relative") == 0) {
        frame = RELATIVE;
    } else if(strcmp(REF_FRAME, "cr3bp") == 0) {
        frame = CR3BP;
    } else if(strcmp(REF_FRAME, "n-body") == 0) {
        frame = N_BODY;
    } else {
        printf("Render Mode not recognized. Exiting...\n");
        exit(1);
    }

    return frame;
}

int main(int argc, char** argv) {
    char* FRAME = NULL; // messy?
    enum REFERENCE_FRAME REF_FRAME;
    float TIME_DELTA = 10.0f; // time diff between frames
    bool DEBUG = false;       // If TRUE, print debug statements once a second
    int opt;
    int NUM_BODIES = 2; // defaulting this to two seems correct?
    bool is_3d = false;
    char* config_file = "init.yaml"; // default config file name
    char* font = "fonts/Inter.ttf";

    while((opt = getopt(argc, argv, "dm:ht:n:3f:w:")) != -1) {
        switch(opt) {
        case 'd':
            DEBUG = true;
            printf("Debugging Mode: Enabled\n");
            break;
        case '3':
            is_3d = true;
            break;
        case 'm':
            FRAME = optarg;
            REF_FRAME = validate_rendering_mode(FRAME); // validate
            printf("Rendering Mode: %s\n", FRAME);
            break;
        case 't':
            TIME_DELTA = atof(optarg);
            if(TIME_DELTA == 0.0) {
                printf("Time Step set to 0.0. No time will pass\n");
            }
            printf("Time Step Value is: %f\n", TIME_DELTA);
            break;
        case 'h':
            print_help_menu();
            exit(0); // shouldnt continue if help menu called
            break;
        case 'n': // define the num of bodies
            NUM_BODIES = atoi(optarg);
            break;
        case ':':
            printf("option needs a value\n");
            break;
        case '?':
            printf("Unknown Option: %c\n", optopt);
            printf("Print the help menu with -h\n");
            exit(0);
            break;
        case 'f':
            printf("Config file set to %s\n", optarg);
            config_file = optarg;
            break;
        case 'w':
            printf("Font set to %s\n", optarg);
            font = optarg;
            break;
        default:
            print_help_menu();
            exit(1);
            break;
        }
    }

    // Ensure that an argument is present
    if(argc < 2) {
        printf("Atleast one option required. View the help menu with -h. Exiting...\n");
        exit(1);
    }

    // Ensure Num Bodies is not 0
    if(NUM_BODIES == 0) {
        printf("NUM_BODIES is set to 0. Please add atleast one.\n");
        exit(1);
    }

    if(NUM_BODIES != 2 && (REF_FRAME == CENTER_OF_GRAVITY || REF_FRAME == RELATIVE)) {
        printf("Simulation can only run in this mode with two (2) bodies\n");
        exit(1);
    }

    if(NUM_BODIES != 3 && REF_FRAME == CR3BP) {
        printf("Simulation can only run in this mode with three (3) bodies\n");
        exit(1);
    }

    // Create the NUM_BODIES array
    // body_2d* bodies_array[NUM_BODIES];
    body_t* bodies_array_config[NUM_BODIES];

    // Parse the config file (init.yaml)
    // Maybe I should make the option to pick this filename
    Settings* config_settings = parse_config_file(config_file, bodies_array_config, is_3d, NUM_BODIES);
    config_settings->ref_frame = REF_FRAME;
    config_settings->time_delta = TIME_DELTA;
    config_settings->debug = DEBUG;
    // config_settings->num_bodies = NUM_BODIES;
    config_settings->font = font;

    Scene* scene = malloc(sizeof(Scene));
    scene->num_bodies = NUM_BODIES;
    scene->config = config_settings;
    scene->cam = NULL;

    // Convert the generics into the proper type for rendering!
    if(is_3d) {
        body_3d** bodies_array = malloc(sizeof(body_3d) * NUM_BODIES);
        for(int i = 0; i < NUM_BODIES; i++) {

            bodies_array[i] = bodies_array_config[i]->t.as_3d;
        }

        scene->bodies_array = bodies_array;
        render3d(scene);

    } else {
        body_2d* bodies_array[NUM_BODIES];
        for(int i = 0; i < NUM_BODIES; i++) {
            bodies_array[i] = bodies_array_config[i]->t.as_2d;
        }
        render(bodies_array, REF_FRAME, TIME_DELTA, NUM_BODIES, DEBUG);
    }
}