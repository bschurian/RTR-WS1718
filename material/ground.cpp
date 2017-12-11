#include "material/ground.h"


void GroundMaterial::apply(unsigned int light_pass)
{
    prog_->bind();

    // qDebug() << "applying Planet material";

    // first set all the uniforms for Phong general material
    PhongMaterial::apply(light_pass);

    // Ground textures
    prog_->setUniformValue("surface.grassTexture", 0);
    surfaces.grassTexture->bind(0);
    prog_->setUniformValue("surface.gravelTexture", 1);
    surfaces.gravelTexture->bind(1);
    prog_->setUniformValue("surface.sandTexture", 2);
    surfaces.sandTexture->bind(2);
    prog_->setUniformValue("surface.stoneTexture", 2);
    surfaces.stoneTexture->bind(2);

    // Fog params
    prog_->setUniformValue("fog.start", fog.start);
    prog_->setUniformValue("fog.end", fog.end);

    // bump & displacement mapping
    prog_->setUniformValue("bump.scale", bump.scale);
    prog_->setUniformValue("bump.tex", 4); bump.tex->bind(4);
    prog_->setUniformValue("displacement.scale", displacement.scale);
    prog_->setUniformValue("displacement.tex", 5); displacement.tex->bind(5);

    //movement of the texture
    prog_->setUniformValue("translation", translation);

}



