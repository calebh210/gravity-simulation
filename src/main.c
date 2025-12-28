#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <getopt.h>
#include "utils/config_parser.h"
#include "utils/structures.h"
#include "physics/gravity.h"
#include "math/math_funcs.h"
#include "physics/cr3bp.h"
#include "graphics/render3d.h"

//Take the string argument for render_mode. Validate that it is correct
// Return an int code to make it easier to work with
int validate_rendering_mode(char *REF_FRAME){
    if(strcmp(REF_FRAME, "inertial") == 0){
        return 100;
    } else if(strcmp(REF_FRAME, "cog") == 0){
        return 101;
    } else if(strcmp(REF_FRAME, "relative") == 0){
        return 102;
    } else if(strcmp(REF_FRAME, "cr3bp") == 0){
        return 103;
    } else if(strcmp(REF_FRAME, "n-body") == 0){
        return 200;
    }else{
        printf("Render Mode not recognized. Exiting...\n");
        exit(1);
    }
}


int main(int argc, char **argv){
    char *REF_FRAME = NULL; //messy?
    int REF_FRAME_CODE = 0;
    float TIME_DELTA = 10.0f; //time diff between frames
    bool DEBUG = false; // If TRUE, print debug statements once a second
    int opt;
    int NUM_BODIES = 2; // defaulting this to two seems correct?
    bool is_3d = false;

    while((opt = getopt(argc, argv, "dm:ht:n:3")) != -1) 
    { 
        switch(opt) 
        {

            case 'd':
                DEBUG = true;
                printf("Debugging Mode: Enabled\n");
                break;
            case '3':
                is_3d = true;
                break;
            case 'm': 
                REF_FRAME = optarg;
                REF_FRAME_CODE = validate_rendering_mode(REF_FRAME); //validate
                printf("Rendering Mode: %s\n", REF_FRAME); 
                break; 
            case 't': 
                TIME_DELTA = atof(optarg);
                if(TIME_DELTA == 0.0){
                    printf("Time Step set to 0.0. No time will pass\n");
                } 
                printf("Time Step Value is: %f\n", TIME_DELTA); 
                break; 
            case 'h':
                printf("Help Menu Placeholder!\n"); 
                exit(0); // shouldnt continue if help menu called
                break;
            case 'n': //define the num of bodies
                NUM_BODIES = atoi(optarg);
                break;
            case ':': 
                printf("option needs a value\n"); 
                break; 
            case '?': 
                printf("unknown option: %c\n", optopt);
                exit(0);
                break; 
        } 
    } 

    // Ensure that an argument is present
    if (argc < 2) {
       printf("Atleast one option required. Exiting...\n");
       exit(0);
    }

    //Ensure Num Bodies is not 0
    if(NUM_BODIES == 0){
        printf("NUM_BODIES is set to 0. Please add atleast one.\n");
    }

    // Create the NUM_BODIES array
    //body_2d* bodies_array[NUM_BODIES];
    body_t* bodies_array_config[NUM_BODIES];

    // Parse the config file (init.yaml)
    // Maybe I should make the option to pick this filename
    parse_config_file(bodies_array_config, is_3d, NUM_BODIES);


    // Convert the generics into the proper type for rendering!
    if(is_3d){
        body_3d* bodies_array[NUM_BODIES];
        for(int i = 0; i < NUM_BODIES; i++){
            bodies_array[i] = bodies_array_config[i]->t.as_3d;
        }
        render3d(bodies_array, REF_FRAME_CODE, TIME_DELTA, NUM_BODIES, DEBUG);

    }else{
        body_2d* bodies_array[NUM_BODIES];
        for(int i = 0; i < NUM_BODIES; i++){
            bodies_array[i] = bodies_array_config[i]->t.as_2d;
        }
        render(bodies_array, REF_FRAME_CODE, TIME_DELTA, NUM_BODIES, DEBUG);

    }


}