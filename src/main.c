#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <yaml.h>
#include "utils/structures.h"
#include "physics/gravity.h"
#include "math/math_funcs.h"
#include "physics/cr3bp.h"

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

// This func parses the config file init.yaml which contains the initial conditions of the sim 
// Currently it works-ish, but isn't very robust, and def needs work
// Also, unsure if it should live here?
void parse_config_file(two_d_body* bodies_array[], int NUM_BODIES){
    //https://www.wpsoftware.net/andrew/pages/libyaml.html
    FILE *fh = fopen("init.yaml", "r");
    yaml_parser_t parser;
    yaml_event_t  event;

    if(!yaml_parser_initialize(&parser)){
        puts("Failed to initialize YAML parser");
    }
    if(fh == NULL){
        puts("Failed to open YAML file");
    }

    yaml_parser_set_input_file(&parser, fh);

    int NUM_BODIES_YAML = 0; // need to check if this is the same or not from the -n option
    bool mass_next = false;
    bool pos_next = false;
    bool vel_next = false;
    bool radius_next = false;

    do {
        yaml_parser_parse(&parser, &event);

        switch(event.type)
        {
        case YAML_NO_EVENT: puts("No event!"); break;
        // case YAML_STREAM_START_EVENT: puts("STREAM START"); break;
        // case YAML_STREAM_END_EVENT:   puts("STREAM END");   break;
        // case YAML_DOCUMENT_START_EVENT: puts("<b>Start Document</b>"); break;
        // case YAML_DOCUMENT_END_EVENT:   puts("<b>End Document</b>");   break;
        // case YAML_SEQUENCE_START_EVENT: puts("<b>Start Sequence</b>"); break;
        // case YAML_SEQUENCE_END_EVENT:   puts("<b>End Sequence</b>");   break;
        // case YAML_MAPPING_START_EVENT:  puts("<b>Start Mapping</b>");  break;
        case YAML_MAPPING_END_EVENT:    puts("<b>End Mapping</b>");  NUM_BODIES_YAML++; break;

        // This is where the key/value pairs are
        case YAML_SCALAR_EVENT: 

            if(NUM_BODIES_YAML > NUM_BODIES - 1) continue; // temp line while I think about how I want to do this

                if(strcmp((const char*)event.data.scalar.value, "Name") == 0){
                    
                    two_d_body *body = ( two_d_body*) malloc(sizeof( two_d_body));
                    if(body == NULL){
                        printf("Failed to allocate memory for body...\n");
                        exit(1);
                    }
                    printf("Mapping this body into %d of array \n", NUM_BODIES_YAML);
                    bodies_array[NUM_BODIES_YAML] = body;
                    break;

                }

                if(strcmp((const char*)event.data.scalar.value, "Mass") == 0){
                    mass_next = true;
                    break;
                }

                if(strcmp((const char*)event.data.scalar.value, "Position") == 0){
                    pos_next = true;
                    break;
                }

                if(strcmp((const char*)event.data.scalar.value, "Velocity") == 0){
                    vel_next = true;
                    break;
                }

                if(strcmp((const char*)event.data.scalar.value, "Radius") == 0){
                    radius_next = true;
                    break;
                }

                if(mass_next){
                    char *stopstring;                                                   
                    bodies_array[NUM_BODIES_YAML]->mass = strtod((const char*)event.data.scalar.value, &stopstring);   
                    mass_next = false;
                    printf("Mass = %s\n", event.data.scalar.value);
                    break;

                }

                if(pos_next){
                    char *token;
                    char *stopstring;                                                   

                    // get the X
                    token = strtok((char*)event.data.scalar.value, ",");
                    bodies_array[NUM_BODIES_YAML]->pos.x = strtod(token, &stopstring);  

                    printf("%s\n", token);

                    // Get the Y
                    token = strtok(NULL, ","); 
                    bodies_array[NUM_BODIES_YAML]->pos.y = strtod(token, &stopstring);   

                    // add something here to detect if its set to 3d, and if yes then read a third pos

                    pos_next = false;
                    break;

                }

                if(vel_next){

                    char *token;
                    char *stopstring;                                                   

                    // get the X
                    token = strtok((char*)event.data.scalar.value, ",");
                    bodies_array[NUM_BODIES_YAML]->velocity.x = strtod(token, &stopstring);  

                    printf("%s\n", token);

                    // Get the Y
                    token = strtok(NULL, ","); 
                    bodies_array[NUM_BODIES_YAML]->velocity.y = strtod(token, &stopstring); 

                    vel_next = false;
                    break;
                }

                if(radius_next){
                    char *stopstring;                                                   

                    // get the radius
                    bodies_array[NUM_BODIES_YAML]->radius = strtod((const char*)event.data.scalar.value, &stopstring);  

                    radius_next = false;
                    break;

                }



            break;

        default:
        }
        if(event.type != YAML_STREAM_END_EVENT)
            yaml_event_delete(&event);

    } while(event.type != YAML_STREAM_END_EVENT);
        yaml_event_delete(&event);

    printf("NUM OF BODIES IN YAML: %d\n", NUM_BODIES_YAML); //subtract one since the file emits a token
    yaml_parser_delete(&parser);
    fclose(fh);
}

int main(int argc, char **argv){
    char *REF_FRAME = NULL; //messy?
    int REF_FRAME_CODE = 0;
    float TIME_DELTA = 10.0f; //time diff between frames
    bool DEBUG = false; // If TRUE, print debug statements once a second
    int opt;
    int NUM_BODIES = 2; // defaulting this to two seems correct?

    while((opt = getopt(argc, argv, "dm:ht:n:")) != -1) 
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
            case 't': 
                TIME_DELTA = atof(optarg);
                if(TIME_DELTA == 0.0){
                    printf("Time Step set to 0.0. No time will pass. Closing simulation...\n");
                    exit(0);
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
    two_d_body* bodies_array[NUM_BODIES];

    parse_config_file(bodies_array, NUM_BODIES);

    render(bodies_array, REF_FRAME_CODE, TIME_DELTA, NUM_BODIES, DEBUG);

}