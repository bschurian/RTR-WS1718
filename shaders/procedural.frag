/*
 * fragment shader for procedural seismic-wave-like ripple
 *
 */

#version 150

uniform float time;

// output - transformed to eye coordinates (EC)
in vec4 position_EC;
in vec3 normal_EC;

// output: fragment/pixel color
out vec4 outColor;

uniform mat4 viewMatrix;

void main() {

    // calculate all required vectors in camera/eye coordinates
//    vec3 viewdir_EC  = (vec4(0,0,0,1) - position_EC).xyz;

    // calculate color using phong, all vectors in eye coordinates
//    vec3 final_color = myphong(normalize(normal_EC));

    // set output
//    outColor = vec4(final_color, 1.0);
    outColor = vec4(0.1);
//    outColor = position_EC;
//    sdas;
}
