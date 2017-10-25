#pragma once

#include "material/material.h"

#include <QOpenGLTexture>


class AnimatedMaterial : public Material {
public:

    // constructor requires existing shader program
    AnimatedMaterial(std::shared_ptr<QOpenGLShaderProgram> prog) : Material(prog)
    {}

    // bind underlying shader program and set required uniforms
    void apply(unsigned int light_pass = 0) override;

    // animation time
    float time = 0.0;
};


