/*
 *  vectors: geom shader examples by Hartmut Schirmacher
 */

#version 150

uniform mat4 modelViewMatrix;
uniform mat4 modelMatrix;

uniform int vectorToShow;

in vec3 position_MC;
in vec3 normal_MC;
in vec3 tangent_MC;
in vec3 bitangent_MC;
in vec2 texcoord;


out VertexData {
    vec3 vector;
} VertexOut;

struct DisplacementMaterial {
    float scale;
    sampler2D tex;
};
uniform DisplacementMaterial displacement;

struct BumpMaterial {
    float scale;
    sampler2D tex;
};
uniform BumpMaterial bump;

// tex coords - just copied
out vec2 texcoord_frag;

// displacement mapping
vec4 displace(vec4 pos) {

    // read displacement value from displacement map
    float disp = texture(displacement.tex, texcoord).r;

    // apply inverse of model-to-world scaling factor to displacement factor
    disp *= 1.0 / modelMatrix[0][0];

    // user-controlled scaling of the displacement effect
    disp *= displacement.scale;

    // apply displacement
    pos += vec4(normal_MC,0)*disp;

    return pos;
}

vec3 decodeNormal(vec3 normal) {
    return normalize( (normal * vec3(2, 2, 1) - vec3(1, 1, 0)) * vec3(bump.scale, bump.scale, 1 ) );
}

void main(void) {

    // apply displacement mapping?
    vec4 pos = vec4(position_MC,1);
    pos = displace(pos);
    // which vector to show?
    vec3 vector = vec3(0,0,0);
    switch(vectorToShow) {
    case 1:
        vector = normal_MC;
        break;
    case 2:
        vector = tangent_MC;
        break;
    case 3:
        vector = bitangent_MC;
        break;
    };

    vec3 bumpValue = texture(bump.tex, texcoord_frag).xyz;

    // transform position and vector to be shown
    VertexOut.vector = decodeNormal(bumpValue);//(modelViewMatrix * vec4(vector,0)).xyz;
    gl_Position      = modelViewMatrix * pos;
}

