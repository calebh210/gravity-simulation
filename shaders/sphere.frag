#version 430 core

out vec4 FragColor;
in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoord;

uniform sampler2D customTexture;
// Make this a UBO?
uniform float ambientStrength;
uniform float diffuseStrength;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos[32];
uniform mat4 lightModel[32];
uniform int numLightSources;
uniform bool load_texture;

void main()
{
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);

    vec3 diffuse = vec3(0.0);

    for(int i=0; i < numLightSources; i++){

        vec3 lp = vec3(lightModel[i] * vec4(lightPos[i], 1.0));
        vec3 lightDir = normalize(lp - FragPos);  

        float diff = max(dot(norm, lightDir), 0.0);
        diffuse += diff * lightColor * diffuseStrength;

    }

    vec3 result = (ambient + diffuse) * objectColor;

    // if(load_texture){
    //     FragColor = texture(customTexture, TexCoord) * vec4(result, 1.0);
    // } else {
    //     FragColor = vec4(result, 1.0);
    // }

    FragColor = vec4(result, 1.0);

}