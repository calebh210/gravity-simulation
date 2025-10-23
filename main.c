#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include "structures.h"
#include "gravity.h"

//Take the string argument for render_mode. Validate that it is correct
void validate_rendering_mode(char *render_mode){
    if(strcmp(render_mode, "inertial") == 0){
        return;
    } else if(strcmp(render_mode, "relative") == 0){
        return;
    } else {
        printf("Render Mode not recognized. Exiting...\n");
        exit(1);
    }
}

int main(int argc, char **argv){
    
    char *render_mode; //messy?
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
                render_mode = optarg;
                validate_rendering_mode(render_mode); //validate
                printf("Rendering Mode: %s\n", render_mode); 
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
    body2->mass =  mass_sun;

    //E3 to convert from KM to M
    // ORANGE IN SIM
    body1->pos.x = -AU;
    body1->pos.y = 0;
    body1->velocity.x = 8E3;
    body1->velocity.y = 5E2;
    body1->radius = 695700E3;

    //BLUE IN SIM
    body2->pos.x = AU;
    body2->pos.y = -AU;
    body2->velocity.x = -2E3;
    body2->velocity.y = 15E3;
    body2->radius = 695700E3;

    if(scharzchild_radius(body1->mass) > body1->radius){
        printf("Body is a black hole");
        fflush(stdout);
        sleep(5);
    }


    //printf("Scharzchild Radius %lfm", scharzchild_radius(mass_earth));

    render(body1, body2, DEBUG);

    // body2->pos.x = 8000E3;
    // body2->pos.y = 6000E3;
    // relative_equation_of_motion(body1, body2);

}