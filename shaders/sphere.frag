#version 460 core

out vec4 FragColor;
in vec3 Normal;  
in vec3 FragPos;  
uniform float ambientStrength;
uniform float diffuseStrength;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform mat4 lightModel;

void main()
{
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);

    vec3 lp = vec3(lightModel * vec4(lightPos, 1.0));
    vec3 lightDir = normalize(lp - FragPos);  

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * diffuseStrength;

    vec3 result = (ambient + diffuse) * objectColor;

    FragColor = vec4(result, 1.0);

}