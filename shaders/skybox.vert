/*
 *
 * vertex shader for skybox
 *
 */

#version 150

// transformation matrices
uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;

// in: position and normal vector in model coordinates (_MC)
in vec3 position_MC;
in vec3 normal_MC;
in vec2 texcoord;


// output - transformed to eye coordinates (EC)
out vec4 position_EC;
out vec3 normal_EC;

// tex coords - just copied
out vec2 texcoord_frag;


void main(void) {

    // pass the texture coordinates on to the fragment shader
    texcoord_frag = texcoord;

    // transform vertex position and normal into eye coordinates
    // for lighting calculations
    position_EC   = modelViewMatrix * vec4(position_MC,1.0);
    normal_EC     = normalMatrix*normal_MC;

    // set the fragment position in clip coordinates
    gl_Position  = projectionMatrix * position_EC;

}


