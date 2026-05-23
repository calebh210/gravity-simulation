#include "graphics/legend.h"
#include <glad.h>
#include <GLFW/glfw3.h>
#include "utils/shaders_parser.h"
#include <stdio.h>

// https://learnopengl.com/In-Practice/Text-Rendering
// Takes the path of a font
// So you can chose your own!
FT_Setup* ft_setup(char* font){

    FT_Setup* setup = malloc(sizeof(FT_Setup));

    // Array containing the 128 ascii chars
    Character* character_array = malloc(sizeof(Character) * 128);  

    // disabled 4 byte alignment enforcemment
    // might cause UD later in program?
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        printf("Error: Could not init FreeType Library\n");
        exit(1);
    }

    FT_Face face;
    if (FT_New_Face(ft, font, 0, &face))
    {
        printf("Error: Failed to load font\n");  
        exit(1);
    }

    FT_Set_Pixel_Sizes(face, 0, 48);  

    for(unsigned char c = 0; c < 128; c++){

        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            printf("Error: Failed to load glpyh\n");
            exit(1);
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        ivector2 size = {face->glyph->bitmap.width, face->glyph->bitmap.rows};
        ivector2 bearing = {face->glyph->bitmap_left, face->glyph->bitmap_top};

        Character ch = {
            texture, 
            size,
            bearing,
            face->glyph->advance.x
        }; 

        character_array[c] = ch;

    }

    glGenVertexArrays(1, &setup->VAO);
    glGenBuffers(1, &setup->VBO);
    glBindVertexArray(setup->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, setup->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); 

    const char* vert_path = "shaders/legend.vert";
    char* vertex_shader = parse_shader_file(vert_path);

    const char* frag_path = "shaders/legend.frag";
    char* fragment_shader = parse_shader_file(frag_path);

    GLuint vs = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vs, 1, (const GLchar**)&vertex_shader, NULL );
    glCompileShader( vs );

    GLuint fs = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fs, 1, (const GLchar**)&fragment_shader, NULL );
    glCompileShader( fs );

    GLuint legend_shaders = glCreateProgram();
    glAttachShader( legend_shaders, fs );
    glAttachShader( legend_shaders, vs );
    glLinkProgram( legend_shaders );
    glUseProgram(legend_shaders);

    float t = 512;
    float r = 512;
    float b = -t;
    float l = -r;
    float n = 0.1;
    float f = 1000;

    float projection[] = {
        2.0f / (r-l), 0.0f, 0.0f, -(r + l) / ( r - l),
        0.0f, 2.0f / ( t -b ), 0.0f, -(t + b) / (t - b),
        0.0f, 0.0f, -2.0f / (f - n), -(f + n) / (f - n),
        0.0f, 0.0f, 0.0f, 1.0f
    };


    GLuint projLoc = glGetUniformLocation(legend_shaders, "projection");

    if(projLoc == -1){
        printf("Failed to apply projection shader to legend \n");
    }

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);


    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    setup->character_array = character_array;
    setup->shaders = legend_shaders;

    return setup;

}

void render_text(FT_Setup* ft, char text[], int text_len, vector2 position, float scale, vector3 color){ 


    glUseProgram(ft->shaders);

    glUniform3f(glGetUniformLocation(ft->shaders, "textColor"), color.x, color.y, color.z);

    GLuint VAO = ft->VAO;
    GLuint VBO = ft->VBO;

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    float x = position.x;
    float y = position.y;

    for(int i = 0; i < text_len; i++ ){

        Character ch = ft->character_array[text[i]];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

}


void draw_legend(FT_Setup* ft, Camera* cam, body_3d* bodies_array[], double nbFrames){

    char* text;
    float scale = 0.4f;

    if(cam->tracking){
        asprintf(&text, "Tracking Object: %s", bodies_array[cam->tracked_body]->name);
    } else {
        asprintf(&text, "Tracking Object: None");
    }

    int text_len = strlen(text);
    render_text(ft, text, text_len, (vector2){260.0, 400.0}, scale, (vector3){0.5f, 0.3f, 0.8f} );

    char* fps;
    asprintf(&fps, "FPS: %0.1f", (double)nbFrames / 1.0);
    text_len = strlen(fps);

    render_text(ft, fps, text_len, (vector2){260.0, 350.0}, scale, (vector3){0.5f, 0.3f, 0.8f} );

    char* time;
    asprintf(&time, "Runtime: %0.3f", glfwGetTime());
    text_len = strlen(time);

    render_text(ft, time, text_len, (vector2){260.0, 300.0}, scale, (vector3){0.5f, 0.3f, 0.8f} );

}