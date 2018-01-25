#include "skyboxmaterial.h"

void SkyBoxMaterial::apply(unsigned int)
{
    prog_->bind();
    prog_->setUniformValue("cubeMap", tex_unit);
    prog_->setUniformValue("skybox.intensity_scale", intensity_scale);
    texture->bind(tex_unit);
}
