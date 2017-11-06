/*
 *
 * vertex shader for simple phong
 *
 */

#version 150

// transformation matrices
uniform mat4 modelViewProjectionMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 viewMatrix;
uniform mat4 inverseViewMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

uniform float time;
struct Wave {
    float depth;
    float speed;
    float heigth;
};
uniform Wave wave;

//uniform float time;

// in: position and normal vector in model coordinates (_MC)
in vec3 position_MC;
in vec3 normal_MC;
in vec2 texcoord;

out vec4 position_EC;
out vec3 normal_EC;
out vec2 texCoords_frag;
uniform sampler3D  Noise;

void main(void) {

    // where the wave is
    float waveIntensity = cos(time*wave.speed) * 0.5 + 0.5;

    float yTex = texcoord.y;
    float rawWaveIntensityAtY =  abs(yTex - waveIntensity);

    float userModifiedWaveIntensityAtY = 1-pow(rawWaveIntensityAtY, wave.depth);

    // this makes the area of effect of the heigt distortion smaller
    // for color this is not used
    float waveHeight = pow(userModifiedWaveIntensityAtY, 3) * wave.heigth;

    // vertex/fragment position in eye coordinates
    position_EC = modelViewMatrix * vec4(position_MC,1);
    position_EC += vec4(normal_MC,0) * waveHeight;

    // position in clip coordinates
    gl_Position  = projectionMatrix * position_EC;

    // normal direction in eye coordinates
    normal_EC  = normalMatrix * normal_MC*(-1);

    texCoords_frag = texcoord;
}


