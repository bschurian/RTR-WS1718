#include "material/texphong.h"
#include <assert.h>


void TexturedPhongMaterial::apply(unsigned int light_pass)
{

    // first do all that regular Phong does
    PhongMaterial::apply(light_pass);

    // then take care of the textures
    prog_->setUniformValue("tex.useDiffuseTexture", tex.useDiffuseTexture);
    prog_->setUniformValue("tex.useEmissiveTexture", tex.useEmissiveTexture);
    prog_->setUniformValue("tex.useGlossTexture", tex.useGlossTexture);
    prog_->setUniformValue("tex.useEnvironmentTexture", tex.useEnvironmentTexture);

    int unit = tex.tex_unit;

    if(tex.useDiffuseTexture) {
        prog_->setUniformValue("diffuseTexture", unit);
        tex.diffuseTexture->bind(unit++);
    }
    if(tex.useEmissiveTexture) {
        prog_->setUniformValue("emissiveTexture", unit);
        tex.emissiveTexture->bind(unit++);
    }
    if(tex.useGlossTexture) {
        prog_->setUniformValue("glossTexture", unit);
        tex.glossTexture->bind(unit++);
    }
    if(tex.useEnvironmentTexture) {
        prog_->setUniformValue("environmentTexture", unit);
        tex.environmentTexture->bind(unit++);
    }
    prog_->setUniformValue("tex.emissive_scale", tex.emissive_scale);

    // bump & displacement mapping
    prog_->setUniformValue("bump.use", bump.use);
    if(bump.use) {
        prog_->setUniformValue("bump.scale", bump.scale);
        prog_->setUniformValue("bump.debug", bump.debug);
        prog_->setUniformValue("bumpTexture", unit); bump.tex->bind(unit++);
    }
    prog_->setUniformValue("displacement.use", displacement.use);
    if(displacement.use) {
        prog_->setUniformValue("displacement.scale", displacement.scale);
        prog_->setUniformValue("displacementTexture", unit); displacement.tex->bind(unit++);
    }

    // environment mapping
    prog_->setUniformValue("envmap.k_mirror",   envmap.k_mirror);
    prog_->setUniformValue("envmap.k_refract",  envmap.k_refract);
    prog_->setUniformValue("envmap.refract_ratio",  envmap.refract_ratio);

}



