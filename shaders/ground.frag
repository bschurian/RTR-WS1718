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
    sampler2D stoneTexture;

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

struct Fog {
    float start;
    float end;
};
uniform Fog fog ;

uniform vec2 translation;

// more uniforms
uniform mat4 modelViewMatrix;
uniform mat4  projectionMatrix;
uniform float time;
uniform vec3  ambientLightIntensity;

/*
 *  Calculate surface color based on Phong illumination model.
 */

vec3 surfaceshader(vec3 n, vec3 v, vec3 l, vec2 uv) {


    //return vec3(n.z);

    uv += translation;
    uv *= 30;

    // texture lookups
    vec3 grassCol = texture(surface.grassTexture, uv*10).rgb;
    vec3 gravelCol = texture(surface.gravelTexture, vec2(uv.x, uv.y*(3888/2592))*3).rgb;
    vec3 sandCol = texture(surface.sandTexture, uv).rgb;
    vec3 snowCol = vec3(1,1,1);
    vec3 stoneCol = texture(surface.stoneTexture, vec2(uv.x, uv.y*(2400/1600))*1.5).rgb;

    // cosine of angle between light and surface normal.
    float ndotl = dot(n,l);

    // visualizing texture coords + day/night border
    float debugFactor = 1.0;
    const float dawnThreshold = cos(radians(89.0));

    // ambient part
    vec3 ambient = phong.k_ambient *
                   ambientLightIntensity * debugFactor;

    // surface back-facing to light?
    if(ndotl<=0.0){
        return ambient;
    }
    else{
        ndotl = max(ndotl, 0.0);
    }

    // diffuse contribution
    vec3 diffuseCoeff = sandCol;
//     : phong.k_diffuse

    //heigth of this fragment
    float height = (inverse(modelViewMatrix) * position_EC).y;

    //steepness
    float steepness = n.z;

    if(height > 0.15 && steepness > 0.6){
        diffuseCoeff = snowCol;
    }else{
        if(steepness < 0.5){
            if(height*pow(steepness,2) > 0.008){
                diffuseCoeff = stoneCol;
            }else{
                diffuseCoeff = gravelCol;
            }
        }else{
            diffuseCoeff = grassCol;
        }
//        diffuseCoeff = vec3(min(1.0, pow(steepness,8)*10));
        diffuseCoeff = vec3(
                    min(1,
                            pow(
                                pow((0.15-height)*(1/0.15), 2)*(1-pow((0.5-steepness)*2, 4)*3)
                            , 1) * 3
                        )
                    );
//        diffuseCoeff = vec3(pow(1-abs(0.5-steepness)*2, 10));
//        return diffuseCoeff;
        diffuseCoeff = mix(mix(stoneCol, gravelCol,min(1,
                                                       pow(
                                                           pow((0.15-height)*(1/0.15), 2)*(1-pow((0.5-steepness)*2, 4)*3)
                                                       , 1) * 3
                                                   )), grassCol, min(1.0, pow(steepness,8)*10));
    }

    // final diffuse term for daytime
    vec3 diffuse =  diffuseCoeff * light.intensity * ndotl;

    // return sum of all contributions
    return ambient + diffuse;

}


vec3 decodeNormal(vec3 normal) {
    return normalize( (normal * vec3(2, 2, 1) - vec3(1, 1, 0)) * vec3(bump.scale, bump.scale, 1 ) );
}

void main() {

    // default normal in tangent space is (0,0,1).
    vec3 bumpValue = texture(bump.tex, texcoord_frag+translation).xyz;

    // get bump direction (in tangent space) from bump texture
    vec3 N = decodeNormal(bumpValue);
    vec3 V = normalize(viewDir_TS);
    vec3 L = normalize(lightDir_TS);

    // calculate color using diffuse illumination
    vec3 color;
    //fog
    vec3 gray = vec3(0.7);
    float fogFactor = (fog.end - position_EC.z)/(fog.end-fog.start);
    //if(fogFactor <= 0.0){
      //  color = vec3(0.7); //gray
    //}else{
        color = //mix(surfaceshader(N, V, L, texcoord_frag), gray, fogFactor);
                surfaceshader(N, V, L, texcoord_frag);
    //}
    
    // set fragment color
    outColor = vec4(color, 1.0);
}
