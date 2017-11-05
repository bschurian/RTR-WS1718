/*
 * fragment shader for procedural seismic-wave-like ripple
 *
 */

#version 150

uniform float time;
struct Wave {
    float depth;
    float speed;
};
uniform Wave wave;

// output - transformed to eye coordinates (EC)
in vec4 position_EC;
in vec3 normal_EC;
in vec2 texCoords_frag;

// output: fragment/pixel color
out vec4 outColor;

uniform mat4 viewMatrix;

void main() {

    // where the wave is
    float t = cos(time*wave.speed) * 0.5 + 0.5;

    float heigth = texCoords_frag.y;
    float outC =  abs(heigth - t);

    float wave = 1-pow(outC, wave.depth);

    outColor = vec4(vec3(wave), 1);
}
