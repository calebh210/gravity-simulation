#version 430 core
layout(location = 0) in vec3 vp;

uniform mat4 model; 
uniform mat4 view;
uniform mat4 projection;
uniform int gridPosCount;
uniform vec3 gridPos[32];
uniform float radius[32];
uniform float r_s[32];

//Since Flamm's is normally so small, incrase the magnitude sometimes if you want to see more
uniform float magnitude;

// This returns an offset of Y to use for Flamm's
// This number is subtracted from the y coordinate on the grid to create the curvature
// look into smoothstep: https://docs.gl/sl4/smoothstep
float flamms_parabaloid(float r, float r_s) {

    float m = magnitude;

    float z = 2.0 * sqrt(max(0.0,r_s * (r - r_s))) * m;

    return z;
}

void main() {

    vec3 v = vp; 

    for(int i=0; i < gridPosCount; i++){

        vec2 d = v.xz - gridPos[i].xz;
        float r = length(d); // distance to body

        if(r < radius[i]){
            r = radius[i];
        }

        if (r <= r_s[i]){

            // I'm choosing to represent singularity with -1.0f.
            v.y = -1.0f;

        } else {

            float offset = flamms_parabaloid(r, r_s[i]);
            v.y += offset;

        }

    }

    gl_Position = projection * view * model * vec4(v, 1.0);
} 