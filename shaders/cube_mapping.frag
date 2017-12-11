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
in vec3 texcoord_frag;

uniform samplerCube sky;

// output: color
out vec4 outColor;

// more uniforms
uniform mat4 modelViewMatrix;
uniform mat4 viewMatrix;
uniform mat4  projectionMatrix;
uniform float time;
uniform vec3  ambientLightIntensity;

void main() {

    vec3 viewdirEC  = (vec4(0,0,0,1) - position_EC).xyz;

    vec3 reflEC = reflect(-viewdirEC, normal_EC);
    vec3 reflWC = (inverse(viewMatrix) * vec4(reflEC, 0.0)).xyz;
    vec3 c_mirror = texture(sky, reflWC).rgb;
    //c_mirror = texture(sky, texcoord_frag).rgb;

    vec3 gray = vec3(1, 0, 0);
    
    // set fragment color
    outColor = vec4(c_mirror, 1.0);
}
