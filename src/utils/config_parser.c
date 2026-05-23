#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>

#include "utils/config_parser.h"

// This func parses the config file init.yaml which contains the initial conditions of the sim 
// Currently it works-ish, but isn't very robust, and def needs work
// Using a body_t to handle 2d and 3d bodies, but it's kinda spaghetti
// Will need to re-work this later


Settings* parse_config_file(char* filename, body_t* bodies_array[], bool is_3d, int NUM_BODIES){
    //https://www.wpsoftware.net/andrew/pages/libyaml.html
    FILE *fh = fopen(filename, "r");
    yaml_parser_t parser;
    yaml_event_t  event;


    Settings *config_settings = malloc(sizeof(Settings));

    if(!yaml_parser_initialize(&parser)){
        puts("Failed to initialize YAML parser. Exiting...");
        exit(1);
    }
    
    if(fh == NULL){
        puts("Failed to open YAML file. Are you sure you have \"init.yaml\" in this directory?\n Exiting...");
        exit(1);
    }

    yaml_parser_set_input_file(&parser, fh);

    int NUM_BODIES_YAML = -1; // This is set to -1 so that it lines up with the indexes of the array
    bool name_next = false;
    bool mass_next = false;
    bool pos_next = false;
    bool vel_next = false;
    bool radius_next = false;
    bool color_next = false;
    bool type_next = false;
    bool grid_status_next = false;
    bool orbit_next = false;

    do {
        yaml_parser_parse(&parser, &event);

        switch(event.type)
        {
        case YAML_NO_EVENT: puts("No Data in YAML!"); break;

        // This is where the key/value pairs are
        case YAML_SCALAR_EVENT: 

            if(NUM_BODIES == NUM_BODIES_YAML) continue; // temp line while I think about how I want to do this

                if(strcmp((const char*)event.data.scalar.value, "Name") == 0){

                    name_next = true;

                    NUM_BODIES_YAML++;
                    body_t *body = ( body_t* ) malloc(sizeof( body_t ));
                    
                    if(is_3d){
                        body_3d *body3d = ( body_3d*) malloc(sizeof( body_3d));
                        if(body == NULL){
                            printf("Failed to allocate memory for body...\n");
                            exit(1);
                        }
                        body->t.as_3d = body3d;

                        bodies_array[NUM_BODIES_YAML] = body;
                        break;
                    }else{
                        body_2d *body2d = ( body_2d*) malloc(sizeof( body_2d));
                        if(body == NULL){
                            printf("Failed to allocate memory for body...\n");
                            exit(1);
                        }
                        body->t.as_2d = body2d;

                        bodies_array[NUM_BODIES_YAML] = body;
                        break;
                    }

                
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

                if(strcmp((const char*)event.data.scalar.value, "Color") == 0){
                    color_next = true;
                    break;
                }

                if(strcmp((const char*)event.data.scalar.value, "Type") == 0){
                    type_next = true;
                    break;
                }

                if(strcmp((const char*)event.data.scalar.value, "Grid") == 0){
                    grid_status_next = true;
                    break;
                }

                if(strcmp((const char*)event.data.scalar.value, "Orbits") == 0){
                    orbit_next = true;
                    break;
                }

                if(name_next){

                    char* n = (const char*)event.data.scalar.value;

                    char* name = malloc(sizeof(char) * 32);

                    // Arbitrary max size of 32 for n
                    // Check this before copying into buffer
                    if (strlen(n) > 32){
                        printf("ERROR: Name \"%s\" is too long. Max size is 32 chars", n);
                        exit(1);
                    }

                    strncpy(name, n, 32);

                    if (is_3d){ 
                        bodies_array[NUM_BODIES_YAML]->t.as_3d->name = name;
                    }

                    name_next = false;
                    break;

                }

                if(mass_next){
                    char *stopstring;        
                    if(is_3d){
                        bodies_array[NUM_BODIES_YAML]->t.as_3d->mass = strtod((const char*)event.data.scalar.value, &stopstring);   
                    } else {
                        bodies_array[NUM_BODIES_YAML]->t.as_2d->mass = strtod((const char*)event.data.scalar.value, &stopstring);   
                    }                                          
                    mass_next = false;
                    break;

                }

                if(pos_next){
                    char *token;
                    char *stopstring;   
                    
                    if(is_3d){
                        // get the X
                        token = strtok((char*)event.data.scalar.value, ",");
                        if(token == NULL){
                            printf("Object missing position.x component\n");
                            goto parsing_error;
                        }
                        bodies_array[NUM_BODIES_YAML]->t.as_3d->pos.x = strtod(token, &stopstring);  

                        // Get the Y
                        token = strtok(NULL, ","); 
                        if(token == NULL){
                            printf("Object missing position.y component\n");
                            goto parsing_error;
                        }
                        bodies_array[NUM_BODIES_YAML]->t.as_3d->pos.y = strtod(token, &stopstring);   

                        // add something here to detect if its set to 3d, and if yes then read a third pos
                        token = strtok(NULL, ","); 
                        if(token == NULL){
                            printf("Object missing position.z component\n");
                            goto parsing_error;
                        }
                        bodies_array[NUM_BODIES_YAML]->t.as_3d->pos.z = strtod(token, &stopstring);  

                        pos_next = false;
                        break;

                    } else {
                        token = strtok((char*)event.data.scalar.value, ",");
                        if(token == NULL){
                            printf("Object missing position.x component\n");
                            goto parsing_error;
                        }
                        bodies_array[NUM_BODIES_YAML]->t.as_2d->pos.x = strtod(token, &stopstring);  

                        // Get the Y
                        token = strtok(NULL, ","); 
                        if(token == NULL){
                            printf("Object missing position.y component\n");
                            goto parsing_error;
                        }
                        bodies_array[NUM_BODIES_YAML]->t.as_2d->pos.y = strtod(token, &stopstring);   
                        pos_next = false;
                        break;
                    }

                }

                if(vel_next){

                    char *token;
                    char *stopstring;                                                   

                    if(is_3d){
                        // get the X
                        token = strtok((char*)event.data.scalar.value, ",");
                        if(token == NULL){
                            printf("Object missing velocity.x component\n");
                            goto parsing_error;
                        }
                        bodies_array[NUM_BODIES_YAML]->t.as_3d->velocity.x = strtod(token, &stopstring);  

                        // Get the Y
                        token = strtok(NULL, ","); 
                        if(token == NULL){
                            printf("Object missing velocity.y component\n");
                            goto parsing_error;
                        }
                        bodies_array[NUM_BODIES_YAML]->t.as_3d->velocity.y = strtod(token, &stopstring); 

                        // Get the Z
                        token = strtok(NULL, ","); 
                        if(token == NULL){
                            printf("3D Object missing velocity.z component\n");
                            goto parsing_error;
                        }
                        bodies_array[NUM_BODIES_YAML]->t.as_3d->velocity.z = strtod(token, &stopstring);  

                        vel_next = false;
                        break;

                    } else {
                        // get the X
                        token = strtok((char*)event.data.scalar.value, ",");
                        if(token == NULL){
                            printf("Object missing velocity.x component\n");
                            goto parsing_error;
                        }
                        bodies_array[NUM_BODIES_YAML]->t.as_2d->velocity.x = strtod(token, &stopstring);  

                        // Get the Y
                        token = strtok(NULL, ","); 
                        if(token == NULL){
                            printf("Object missing velocity.y component\n");
                            goto parsing_error;
                        }
                        bodies_array[NUM_BODIES_YAML]->t.as_2d->velocity.y = strtod(token, &stopstring); 

                        vel_next = false;
                        break;
                    }
                }

                if(radius_next){
                    char *stopstring;                                                   

                    // get the radius
                    if(is_3d){
                        bodies_array[NUM_BODIES_YAML]->t.as_3d->radius = strtod((const char*)event.data.scalar.value, &stopstring);  
                    } else {
                        bodies_array[NUM_BODIES_YAML]->t.as_2d->radius = strtod((const char*)event.data.scalar.value, &stopstring);  
                    }

                    radius_next = false;
                    break;

                }

                if(color_next){
                    char *stopstring;                                                   
                    unsigned int hex = (unsigned int)strtol((const char*)event.data.scalar.value, &stopstring, 16);

                    if(is_3d){
                        // convert hex to rgb and add it to the body struct
                        bodies_array[NUM_BODIES_YAML]->t.as_3d->color.r = ((hex >> 16) & 0xFF) / 255.0;  
                        bodies_array[NUM_BODIES_YAML]->t.as_3d->color.g = ((hex >> 8) & 0xFF) / 255.0;   
                        bodies_array[NUM_BODIES_YAML]->t.as_3d->color.b = ((hex) & 0xFF) / 255.0;        
                    } else {
                        // convert hex to rgb and add it to the body struct
                        bodies_array[NUM_BODIES_YAML]->t.as_2d->color.r = ((hex >> 16) & 0xFF) / 255.0;  
                        bodies_array[NUM_BODIES_YAML]->t.as_2d->color.g = ((hex >> 8) & 0xFF) / 255.0;   
                        bodies_array[NUM_BODIES_YAML]->t.as_2d->color.b = ((hex) & 0xFF) / 255.0;  
                    }

                    color_next = false;

                }

                if(type_next){

                    if(is_3d){
                        // needs to be implemented still
                        const char* type = (const char*)event.data.scalar.value;
                        if(strcmp(type, "Star") == 0 || strcmp(type, "star") == 0){
                            bodies_array[NUM_BODIES_YAML]->t.as_3d->type = STAR;
                        }
                    }
                    type_next = false;
                }

                if(grid_status_next){

                    const char* type = (const char*)event.data.scalar.value;

                    if(strcmp(type, "false") == 0 || strcmp(type, "False") == 0){
                        config_settings->draw_grid = false;
                    } else {
                        config_settings->draw_grid = true;
                    }

                    grid_status_next = false;
                }

                if(orbit_next){
                    
                    const char* type = (const char*)event.data.scalar.value;

                    if(strcmp(type, "false") == 0 || strcmp(type, "False") == 0){
                        config_settings->draw_orbits = false;
                    } else {
                        config_settings->draw_orbits = true;
                    }

                    orbit_next = false;
                }

            break;

        
        }
        if(event.type != YAML_STREAM_END_EVENT)
            yaml_event_delete(&event);

    } while(event.type != YAML_STREAM_END_EVENT);
        yaml_event_delete(&event);


    if(NUM_BODIES_YAML + 1 < NUM_BODIES){
        goto bodies_mismatch_error;
    }

    
    yaml_parser_delete(&parser);
    fclose(fh);
    return config_settings;

    parsing_error:
        printf("Exiting...\n");
        exit(1);

    bodies_mismatch_error:
        printf("NUM_BODIES defined with -n was more than the number of bodies defined in init.yaml. Exiting with error...\n");
        exit(1);

}
