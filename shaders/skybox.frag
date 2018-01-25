/*
 * fragment shader for skybox
 *
 */

#version 150

// input - transformed to eye coordinates (EC)
in vec4 position_EC;
in vec3 normal_EC;

// tex coords - just copied
in vec2 texcoord_frag;

// output: color
out vec4 outColor;

struct SkyBox {
    float intensity_scale;
};
uniform SkyBox skybox;

uniform samplerCube cubeMap;

// matrices
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;


void main() {

    // normalize normal after projection
    vec3 normalEC = normalize(normal_EC);

    // do we use a perspective or an orthogonal projection matrix?
    bool usePerspective = projectionMatrix[2][3] != 0.0;

    // vector from eye to vertex
    vec3 toVertexEC = usePerspective? normalize(position_EC.xyz) : vec3(0,0,-1);
    vec3 toVertexWC = (inverse(viewMatrix) * vec4(toVertexEC,0)).xyz;

    // simply look up color in environment map, along viewing ray
    vec3 sky = texture(cubeMap, toVertexWC).rgb * skybox.intensity_scale;
    outColor = vec4(sky, 1.0);

}
