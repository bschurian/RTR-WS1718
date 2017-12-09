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

// output: color
out vec4 outColor;

// point light
struct PointLight {
    vec3 intensity;
    vec4 position_WC;
};
uniform PointLight light;

// Phong coefficients and exponent
struct PhongMaterial {

    // basic Phong
    vec3 k_ambient;
    vec3 k_diffuse;
    vec3 k_specular;
    float shininess;

};
uniform PhongMaterial phong;

struct SurfaceMaterial {

    // additional textures
    sampler2D grassTexture;
    sampler2D gravelTexture;
    sampler2D sandTexture;

};
uniform SurfaceMaterial surface;

struct BumpMaterial {
    float scale;
    sampler2D tex;
};
uniform BumpMaterial bump;

struct DisplacementMaterial {
    float scale;
    sampler2D tex;
};
uniform DisplacementMaterial displacement;

// more uniforms
uniform mat4 modelViewMatrix;
uniform mat4  projectionMatrix;
uniform float time;
uniform vec3  ambientLightIntensity;

/*
 *  Calculate surface color based on Phong illumination model.
 */

vec3 surfaceshader(vec3 n, vec3 v, vec3 l, vec2 uv) {

    uv *= 3;

    // texture lookups
    vec3 grassCol = texture(surface.grassTexture, uv).rgb;
    vec3 gravelCol = texture(surface.gravelTexture, uv).rgb;
    vec3 sandCol = texture(surface.sandTexture, uv).rgb;
    vec3 snowCol = vec3(1,1,1);

    // cosine of angle between light and surface normal.
    float ndotl = dot(n,l);

    // visualizing texture coords + day/night border
    float debugFactor = 1.0;
    const float dawnThreshold = cos(radians(89.0));

    // ambient part
    vec3 ambient = phong.k_ambient *
                   ambientLightIntensity * debugFactor;

    // surface back-facing to light?
    if(ndotl<=0.0)
        return ambient;
    else
        ndotl = max(ndotl, 0.0);

    // diffuse contribution
    vec3 diffuseCoeff = gravelCol;
//     : phong.k_diffuse

    //heigth of a mountain
    float height = (inverse(modelViewMatrix) * position_EC).y;

    if(height > 0.15){
        diffuseCoeff = snowCol;
    }

    // clouds at day?
//        diffuseCoeff = (1.0-cloudDensity)*diffuseCoeff + cloudDensity*vec3(1.5,1.5,1.5);
    // final diffuse term for daytime
    vec3 diffuse =  diffuseCoeff * light.intensity * ndotl;


    // ambient part contains lights; modify depending on time of day
//    ambient *= pow(1.0-ndotl,surface.night_blend_exp);

     // reflected light direction = perfect reflection direction
    vec3 r = reflect(-l,n);

    // cosine of angle between reflection dir and viewing dir
    float rdotv = max( dot(r,v), 0.0);

    // return sum of all contributions
    return ambient + diffuse;

}


vec3 decodeNormal(vec3 normal) {
    return normalize( (normal * vec3(2, 2, 1) - vec3(1, 1, 0)) * vec3(bump.scale, bump.scale, 1 ) );
}

void main() {

    // default normal in tangent space is (0,0,1).
    vec3 bumpValue = texture(bump.tex, texcoord_frag).xyz;

    // get bump direction (in tangent space) from bump texture
    vec3 N = decodeNormal(bumpValue);
    vec3 V = normalize(viewDir_TS);
    vec3 L = normalize(lightDir_TS);

    // calculate color using diffuse illumination
    vec3 color = surfaceshader(N, V, L, texcoord_frag);
    
    // set fragment color
    outColor = vec4(color, 1.0);
}
