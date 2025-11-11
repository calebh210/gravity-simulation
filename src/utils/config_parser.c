#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>

#include "utils/config_parser.h"

// This func parses the config file init.yaml which contains the initial conditions of the sim 
// Currently it works-ish, but isn't very robust, and def needs work
void parse_config_file(two_d_body* bodies_array[], int NUM_BODIES){
    //https://www.wpsoftware.net/andrew/pages/libyaml.html
    FILE *fh = fopen("init.yaml", "r");
    yaml_parser_t parser;
    yaml_event_t  event;

    if(!yaml_parser_initialize(&parser)){
        puts("Failed to initialize YAML parser. Exiting...");
        exit(1);
    }
    
    if(fh == NULL){
        puts("Failed to open YAML file. Are you sure you have \"init.yaml\" in this directory?\n Exiting...");
        exit(1);
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
        case YAML_MAPPING_END_EVENT: NUM_BODIES_YAML++; break;

        // This is where the key/value pairs are
        case YAML_SCALAR_EVENT: 

            if(NUM_BODIES_YAML > NUM_BODIES - 1) continue; // temp line while I think about how I want to do this

                if(strcmp((const char*)event.data.scalar.value, "Name") == 0){
                    
                    two_d_body *body = ( two_d_body*) malloc(sizeof( two_d_body));
                    if(body == NULL){
                        printf("Failed to allocate memory for body...\n");
                        exit(1);
                    }
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
                    break;

                }

                if(pos_next){
                    char *token;
                    char *stopstring;                                                   

                    // get the X
                    token = strtok((char*)event.data.scalar.value, ",");
                    bodies_array[NUM_BODIES_YAML]->pos.x = strtod(token, &stopstring);  

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

    yaml_parser_delete(&parser);
    fclose(fh);
}