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

    struct PointLight {
        QVector4D position_WC = QVector4D(0,1,5,1);
        QVector3D color = QVector3D(1,1,1);
        float intensity = 0.5;
    };
    std::vector<PointLight> lights;

};


