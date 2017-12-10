#include "material/ground.h"


void GroundMaterial::apply(unsigned int light_pass)
{
    prog_->bind();

    // qDebug() << "applying Planet material";

    // first set all the uniforms for Phong general material
    PhongMaterial::apply(light_pass);

    // Planet textures
    prog_->setUniformValue("surface.grassTexture", 0);
    surfaces.grassTexture->bind(0);
    prog_->setUniformValue("surface.gravelTexture", 1);
    surfaces.gravelTexture->bind(1);
    prog_->setUniformValue("surface.sandTexture", 2);
    surfaces.sandTexture->bind(2);

    // bump & displacement mapping
    prog_->setUniformValue("bump.scale", bump.scale);
    prog_->setUniformValue("bump.tex", 3); bump.tex->bind(3);
    prog_->setUniformValue("displacement.scale", displacement.scale);
    prog_->setUniformValue("displacement.tex", 4); displacement.tex->bind(4);

    //movement of the texture
    prog_->setUniformValue("translation", translation);

}



