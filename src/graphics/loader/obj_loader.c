#include <stdio.h>
#include "graphics/loader/obj_loader.h"

//https://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/

void load_obj(const char *filename, vector3_da *out_vertices, vector2_da *out_uvs, vector3_da *out_normals){

    FILE *fp = fopen(filename, "r");

    // configure the dynamic arrays (similar to c++ vectors)
    // macros are used to make them sort-of generic?

    int_da vertexIndices, uvIndices, normalIndices;
    int_da_init(&vertexIndices);
    int_da_init(&uvIndices);
    int_da_init(&normalIndices);


    vector3_da temp_vertices;
    vector3_da_init(&temp_vertices);

    vector2_da temp_uvs;
    vector2_da_init(&temp_uvs);

    vector3_da temp_normals;
    vector3_da_init(&temp_normals);


    if( fp == NULL ){
        printf("Failed to open %s\n", filename);
        return;
    }   

    while(true){

        // assume size is smaller than 256
        char line[256];

        int res = fscanf(fp, "%s", line);
        if (res == EOF){
            break; 
        }

        if ( strcmp( line, "v" ) == 0 ){
            vector3 vertex;
            fscanf(fp, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            vector3_da_push(&temp_vertices, vertex);
        }  else if ( strcmp( line, "vt" ) == 0 ){
            vector2 uv;
            fscanf(fp, "%f %f\n", &uv.x, &uv.y );
            vector2_da_push(&temp_uvs, uv);

        } else if ( strcmp( line, "vn" ) == 0 ){
            vector3 normal;
            fscanf(fp, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            vector3_da_push(&temp_normals, normal);

    
        } else if ( strcmp( line, "f" ) == 0 ){

            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            
            // this is a format check
            int matches = fscanf(fp, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            
            if (matches != 9){
                printf("File can't be read. Try exporting with other options\n");
                return;
            }

            int_da_push(&vertexIndices, vertexIndex[0]);
            int_da_push(&vertexIndices, vertexIndex[1]);
            int_da_push(&vertexIndices, vertexIndex[2]);

            int_da_push(&uvIndices, uvIndex[0]);
            int_da_push(&uvIndices, uvIndex[1]);
            int_da_push(&uvIndices, uvIndex[2]);

            int_da_push(&normalIndices, normalIndex[0]);
            int_da_push(&normalIndices, normalIndex[0]);
            int_da_push(&normalIndices, normalIndex[0]);
 
        }
    }
    
    // load vertices into vertex vector
    for( unsigned int i=0; i<vertexIndices.size; i++ ){

        unsigned int vertexIndex = vertexIndices.buf[i];
        vector3 vertex = temp_vertices.buf[ vertexIndex-1 ];
        vector3_da_push(out_vertices, vertex);
    }

    for( unsigned int i=0; i<normalIndices.size; i++ ){

        unsigned int normalIndex = normalIndices.buf[i];
        vector3 vertex = temp_normals.buf[ normalIndex-1 ];
        vector3_da_push(out_normals, vertex);
    }
}