/*
 * fragment shader for phong + textures + bumps
 *
 */

#version 150

// output - transformed to eye coordinates (EC)
in vec4 position_EC;
in vec3 normal_EC;
in vec2 texCoords_frag;

// output: fragment/pixel color
out vec4 outColor;

struct DotsMaterial {

    int shades;
    vec3 k_ambient;
    vec3 k_diffuse;
    vec3 k_specular;
    float shininess;
    float frequency;#
    float radius;
    vec3 dotcolor;
};

uniform DotsMaterial dots;
uniform vec3 ambientLightIntensity;

struct PointLight {
    vec3 intensity;
    vec4 position_WC;
    int  pass;
};
uniform PointLight light;

uniform mat4 viewMatrix;


/*
 *  Calculate surface color based on Phong illumination model.
 *  from http://www.konlabs.com/articles_data/cel_shading/
 */

vec3 mycartoon(vec3 n, vec3 v, vec3 l) {

    // cosine of angle between light and surface normal.
    float ndotl = dot(n,l);

    // ambient / emissive part
    vec3 ambient = vec3(0,0,0);
    if(light.pass == 0) // only add ambient in first light pass
        ambient = dots.k_ambient * ambientLightIntensity;

    // surface back-facing to light?
    if(ndotl<=0.0)
        return 1;
    else
        ndotl = max(ndotl, 0.0);

    //number of shades
    float shadeIntensity = ceil(ndotl * dots.shades -0.5)/(dots.shades);

    if(shadeIntensity < 0){
        return 1;
    }

    // diffuse term
    vec3 diffuse =  dots.k_diffuse * light.intensity * shadeIntensity;

    // reflected light direction = perfect reflection direction
    vec3 r = reflect(-l,n);

    // cosine of angle between reflection dir and viewing dir
    float rdotv = max( dot(r,v), 0.0);

    float specularIntensity = ceil(rdotv*dots.shades-0.5)/dots.shades;

    // specular contribution + gloss map
    vec3 specular = dots.k_specular * light.intensity * pow(specularIntensity, dots.shininess);
//return pow(sin(t), cel.shininess);
    // return sum of all contributions
    return ambient + diffuse + specular;

}
/**
//vec3 dots(vec3 colormain,vec3 colordot,float freq,vec3 ){
vec3 dots(){

    vec3 black = vec3(0.0,0.0,0.0);
    vec3 white = vec3(1.0,1.0,1.0);
    //später radius und freq(1/freq) einsetzen
    if(mod(texCoords_frag.x,0.05)>=0.25)
        vec3 dotcolor = white;
    else
        vec3 dotcolor = black;

    return dotcolor;
}
**/
void main() {
    // calculate all required vectors in camera/eye coordinates
    vec4 lightpos_EC = viewMatrix * light.position_WC;
    vec3 lightdir_EC = (lightpos_EC   - position_EC).xyz;
    vec3 viewdir_EC  = (vec4(0,0,0,1) - position_EC).xyz;

    // calculate color using phong, all vectors in eye coordinates
    vec3 final_color = mycartoon(normalize(normal_EC),
                               normalize(viewdir_EC),
                               normalize(lightdir_EC));

    vec3 black = vec3(0.0,0.0,0.0);
    vec3 white = vec3(1.0,1.0,1.0);
    /**
    vec3 dotcolor = dots(10.0,white,black,0.5);

    vec3 mixedcolor = final_color + dotcolor;
    **/
    // set output
    outColor = vec4(final_color, 1.0);
}

