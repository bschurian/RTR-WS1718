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


out vec4 position_EC;
out vec3 normal_EC;

//skybox
uniform samplerCube sky;

// tex coords - just copied
out vec3 texcoord_frag;

void main(void) {

    position_EC = modelViewMatrix * vec4(position_MC,1);

    // apply displacement mapping?
    vec4 pos = vec4(position_MC,1);

    // vertex/fragment position in clip coordinates
    gl_Position  = modelViewProjectionMatrix * pos;

    texcoord_frag = pos.xyz;

    // normal direction in eye coordinates
    normal_EC  = normalMatrix * normal_MC;

}


