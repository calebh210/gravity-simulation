#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include "structures.h"
#include "gravity.h"
#include "math_funcs.h"

//Take the string argument for render_mode. Validate that it is correct
// Return an int code to make it easier to work with
int validate_rendering_mode(char *REF_FRAME){
    if(strcmp(REF_FRAME, "inertial") == 0){
        return 100;
    } else if(strcmp(REF_FRAME, "cog") == 0){
        return 101;
    } else if(strcmp(REF_FRAME, "relative") == 0){
        return 102;
    }else{
        printf("Render Mode not recognized. Exiting...\n");
        exit(1);
    }
}

int main(int argc, char **argv){
    
    char *REF_FRAME = NULL; //messy?
    int REF_FRAME_CODE = 0;
    bool DEBUG = false; // If TRUE, print debug statements once a second
    int opt;
    while((opt = getopt(argc, argv, "dm:h")) != -1) 
    { 
        switch(opt) 
        {
            case 'd':
                DEBUG = true;
                printf("Debugging Mode: Enabled\n");
                break;
            case 'm': 
                REF_FRAME = optarg;
                REF_FRAME_CODE = validate_rendering_mode(REF_FRAME); //validate
                printf("Rendering Mode: %s\n", REF_FRAME); 
                break; 
            case 'h':
                printf("Help Menu Placeholder!\n"); 
                exit(0); // shouldnt continue if help menu called
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





    two_d_body *body1 = ( two_d_body*) malloc(sizeof( two_d_body)*2);

    two_d_body *body2 = ( two_d_body*) malloc(sizeof( two_d_body)*2);

    body1->mass = mass_sun;
    body2->mass =  mass_sun / 2;

    //E3 to convert from KM to M
    // ORANGE IN SIM
    body1->pos.x = 0;
    body1->pos.y = 0;
    body1->velocity.x = 5E3;
    body1->velocity.y = 3;
    body1->radius = 695700E3;

    //BLUE IN SIM
    body2->pos.x = AU * 0.5;
    body2->pos.y = -AU * 0.5;
    body2->velocity.x = -2E3;
    body2->velocity.y = 20E3;
    body2->radius = 695700E3;

    if(scharzchild_radius(body1->mass) > body1->radius){
        printf("Body is a black hole");
        fflush(stdout);
        sleep(5);
    }


    //printf("Scharzchild Radius %lfm", scharzchild_radius(mass_earth));

    render(body1, body2, REF_FRAME_CODE, DEBUG);

    // body2->pos.x = 8000E3;
    // body2->pos.y = 6000E3;
    // relative_equation_of_motion(body1, body2);

}