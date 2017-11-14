#pragma once

#include "material/material.h"
#include <memory>


class WireframeMaterial : public Material {
public:

    // constructor requires existing shader program
    WireframeMaterial(std::shared_ptr<QOpenGLShaderProgram> prog) : Material(prog) {}

    QVector3D wireframeColor = QVector3D(1,1,0);

    // bind underlying shader program and set required uniforms
    void apply(unsigned int = 0) override;

};

