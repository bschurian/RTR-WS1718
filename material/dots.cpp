#include "material/dots.h"
#include <assert.h>

void DotsMaterial::apply(unsigned int light_pass)
{
    // qDebug() << "applying TexturedPhongMaterial";

    prog_->bind();

    // globals
    prog_->setUniformValue("time", time);
    prog_->setUniformValue("ambientLightIntensity", ambientLightIntensity);

    // point light
    assert(light_pass>=0 && light_pass<lights.size());
    prog_->setUniformValue("light.position_WC", lights[light_pass].position_WC);
    prog_->setUniformValue("light.intensity", lights[light_pass].color * lights[light_pass].intensity);
    prog_->setUniformValue("light.pass", light_pass);

    // Phong
    prog_->setUniformValue("dots.k_ambient",  dots.k_ambient);
    prog_->setUniformValue("dots.k_diffuse",  dots.k_diffuse);
    prog_->setUniformValue("dots.k_specular", dots.k_specular);
    prog_->setUniformValue("dots.shininess",  dots.shininess);

    // Cartoon
    prog_->setUniformValue("dots.shades", dots.shades);

    //Dots
    prog_->setUniformValue("dots.frequency", dots.frequency);
    prog_->setUniformValue("dots.radius", dots.radius);
    prog_->setUniformValue("dots.dotcolor", dots.dotcolor);

}


