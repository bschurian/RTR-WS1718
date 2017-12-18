#pragma once

#include "material/material.h"
#include "material/phong.h"

#include <QOpenGLTexture>
#include <memory>


class SkyBoxMaterial : public PhongMaterial {
public:

    // constructor requires existing shader program
    SkyBoxMaterial(std::shared_ptr<QOpenGLShaderProgram> prog)
        : PhongMaterial(prog) {}

    //six sides
    std::shared_ptr<QOpenGLTexture> sky;

    // bind underlying shader program and set required uniforms
    void apply(unsigned int light_pass = 0) override;

};

