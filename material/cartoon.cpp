#include "material/cartoon.h"
#include <assert.h>

void CartoonMaterial::apply(unsigned int light_pass)
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
    prog_->setUniformValue("phong.k_ambient",  phong.k_ambient);
    prog_->setUniformValue("phong.k_diffuse",  phong.k_diffuse);
    //prog_->setUniformValue("phong.k_diffuse",  QVector3D(0.20f,0.20f,0.10f));
    prog_->setUniformValue("phong.k_specular", phong.k_specular);
    prog_->setUniformValue("phong.shininess",  phong.shininess);

}



