/*
 * Simple Blur Shader
 * (C)opyright Hartmut Schirmacher, http://schirmacher.beuth-hochschule.de
 *
 */

#version 150

// texture to be blurred
uniform sampler2D post_tex;
uniform sampler2D post_tex2;
uniform int image_width;
uniform int image_height;

// texture coords
in vec2 texcoord_frag;

// output: color
out vec4 outColor;

// simple pseudo random number
float rand(vec2 xy)
{
    return fract(sin(dot(xy, vec2(12.9898,78.233))) * 43758.5453);
}
vec2 rand2(vec2 xy)
{
    return vec2(rand(xy.xy), rand(xy.yx));
}

void main(void)
{
    vec2 image_size = vec2(image_width, image_height);

    vec3 color = vec3(0,0,0);
    color += (texture(post_tex, texcoord_frag).rgb*0.5);
    color += (texture(post_tex2, texcoord_frag).rgb*0.5);

    // out to frame buffer
    outColor = vec4(color, 1);
    // outColor = vec4(texcoord_frag.s, texcoord_frag.t, 0, 1);

}
