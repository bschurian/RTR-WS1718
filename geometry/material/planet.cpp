#include "material/planet.h"


void PlanetMaterial::apply(unsigned int light_pass)
{
    prog_->bind();

    // qDebug() << "applying Planet material";

    // first set all the uniforms for Phong general material
    PhongMaterial::apply(light_pass);

    // Planet textures
    prog_->setUniformValue("planet.useDayTexture", planet.useDayTexture);
    prog_->setUniformValue("planet.useNightTexture", planet.useNightTexture);
    prog_->setUniformValue("planet.useGlossTexture", planet.useGlossTexture);
    prog_->setUniformValue("planet.useCloudsTexture", planet.useCloudsTexture);
    if(planet.useDayTexture) {
        prog_->setUniformValue("planet.dayTexture", 0);
        planet.dayTexture->bind(0);
    }
    if(planet.useNightTexture) {
        prog_->setUniformValue("planet.nightTexture", 1);
        planet.nightTexture->bind(1);
    }
    if(planet.useGlossTexture) {
        prog_->setUniformValue("planet.glossTexture", 2);
        planet.glossTexture->bind(2);
    }
    if(planet.useCloudsTexture) {
        prog_->setUniformValue("planet.cloudsTexture", 3);
        planet.cloudsTexture->bind(3);
    }
    prog_->setUniformValue("planet.night_scale", planet.night_scale);
    prog_->setUniformValue("planet.night_blend_exp", planet.night_blend_exp);

    // Planet debugging & animation
    prog_->setUniformValue("planet.debug", planet.debug);
    prog_->setUniformValue("planet.debug_texcoords", planet.debug_texcoords);
    prog_->setUniformValue("planet.debugWaterLand", planet.debugWaterLand);
    prog_->setUniformValue("planet.animateClouds", planet.animateClouds);

    // bump & displacement mapping
    prog_->setUniformValue("bump.use", bump.use);
    if(bump.use) {
        prog_->setUniformValue("bump.scale", bump.scale);
        prog_->setUniformValue("bump.tex", 4); bump.tex->bind(4);
        prog_->setUniformValue("bump.debug", bump.debug);
    }
    prog_->setUniformValue("displacement.use", displacement.use);
    if(displacement.use) {
        prog_->setUniformValue("displacement.scale", displacement.scale);
        prog_->setUniformValue("displacement.tex", 5); displacement.tex->bind(5);
    }

}



