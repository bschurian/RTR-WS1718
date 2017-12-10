/*
 * fragment shader for phong + textures + bumps
 *
 */

#version 150

// output - transformed to eye coordinates (EC)
in vec4 position_EC;
in vec3 normal_EC;

// output - transformed to tangent space (TS)
in vec3 viewDir_TS;
in vec3 lightDir_TS;

// tex coords - just copied
in vec2 texcoord_frag;

uniform sampler2D skybox;

// output: color
out vec4 outColor;

// more uniforms
uniform mat4 modelViewMatrix;
uniform mat4  projectionMatrix;
uniform float time;
uniform vec3  ambientLightIntensity;

void main() {

    // default normal in tangent space is (0,0,1).
    vec3 skyCol = texture(skybox, texcoord_frag+translation).xyz;
    
    // set fragment color
    outColor = vec4(skyCol, 1.0);
}
