#pragma once

#include "material/material.h"

#include <QOpenGLTexture>
#include <memory>


class CubeMappingMaterial : public Material {
public:

    // constructor requires existing shader program
    CubeMappingMaterial(std::shared_ptr<QOpenGLShaderProgram> prog)
        : Material(prog) {}

    // planet-specific properties
    std::shared_ptr<QOpenGLTexture> skyTexture;
    QVector2D translation = QVector2D(0.0, 0.0) ;

    // bind underlying shader program and set required uniforms
    void apply(unsigned int light_pass = 0) {};

};

