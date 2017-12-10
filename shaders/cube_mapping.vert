/*
 *
 * vertex shader for phong + textures + bumps 
 *
 */

#version 150

// transformation matrices
uniform mat4 modelViewProjectionMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

// in: position and normal vector in model coordinates (_MC)
in vec3 position_MC;
in vec3 normal_MC;
in vec3 tangent_MC;
in vec3 bitangent_MC;
in vec2 texcoord;

//skybox
uniform sampler2D skybox;

// tex coords - just copied
out vec2 texcoord_frag;

void main(void) {

    // apply displacement mapping?
    vec4 pos = vec4(position_MC,1);

    // vertex/fragment position in clip coordinates
    gl_Position  = modelViewProjectionMatrix * pos;

}


