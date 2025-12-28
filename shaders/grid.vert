#version 460 core
layout(location = 0) in vec3 vp;

uniform mat4 model; 
uniform mat4 view;
uniform mat4 projection;
uniform int gridPosCount;
uniform vec3 gridPos[32];
uniform float warp[32];
uniform float radius[32];
uniform float r_s[32];

// this returns a normalized mappings of flamm's which i'm using for my grids
// look into smoothstep: https://docs.gl/sl4/smoothstep
float flamms_parabaloid(float r, float r_s) {
    float minVal = -1.0;
    float maxVal = 1.0;

    if (r <= r_s) {
        return -minVal;
    }

    float z = -2.0 * sqrt(max(0.0,r_s * (r - r_s)));

    float normalized_z = (z - minVal) / (maxVal - minVal);
    return normalized_z;
}


// this works ish?
void main() {

    vec3 v = vp; 

    for(int i=0; i < gridPosCount; i++){

        float dx = v.x - gridPos[i].x;
        float dz = v.z - gridPos[i].z;
        float r = sqrt(dx*dx + dz*dz); // distance to body

        if(r < radius[i]){
            r = radius[i];
        }

        v.y -= flamms_parabaloid(r, r_s[i]);
    }

    gl_Position = projection * view * model * vec4(v, 1.0);
} 