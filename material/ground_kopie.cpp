#include "material/ground_kopie.h"


void SkyBoxMaterial::apply(unsigned int light_pass)
{
    prog_->bind();

    // qDebug() << "applying Planet material";

    // first set all the uniforms for Phong general material
    PhongMaterial::apply(light_pass);

    // Planet textures
    prog_->setUniformValue("sky", 0);
    sky->bind( 0);

}
