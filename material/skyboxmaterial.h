#pragma once

#include "material.h"
#include <QOpenGLTexture.h>

class SkyBoxMaterial : public Material {
public:

    // constructor requires existing shader program
    SkyBoxMaterial(std::shared_ptr<QOpenGLShaderProgram> prog, int texunit = 0)
        : Material(prog), tex_unit(texunit) {}

    // the cube map
    std::shared_ptr<QOpenGLTexture> texture;

    // intensity scaling factor
    float intensity_scale = 1.0;

    // texture unit to be used
    int tex_unit;

    // bind underlying shader program and set required uniforms
    void apply(unsigned int light_pass = 0) override;

};

