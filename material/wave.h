#pragma once

#include "material/animatedmaterial.h"

#include <QOpenGLTexture>


class WaveMaterial : public AnimatedMaterial {
public:

    // constructor requires existing shader program
    WaveMaterial(std::shared_ptr<QOpenGLShaderProgram> prog) : AnimatedMaterial(prog)
    {}

    // bind underlying shader program and set required uniforms
    void apply(unsigned int light_pass = 0) override;

    struct Wave {
        float depth = 0.1f;
        float speed = 1;
    } wave;

};


