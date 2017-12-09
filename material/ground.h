#pragma once

#include "material/material.h"
#include "material/phong.h"

#include <QOpenGLTexture>
#include <memory>


class GroundMaterial : public PhongMaterial {
public:

    // constructor requires existing shader program
    GroundMaterial(std::shared_ptr<QOpenGLShaderProgram> prog)
        : PhongMaterial(prog) {}

    // planet-specific properties
    struct Surfaces {
        std::shared_ptr<QOpenGLTexture> grassTexture;
        std::shared_ptr<QOpenGLTexture> gravelTexture;
        std::shared_ptr<QOpenGLTexture> sandTexture;
    } surfaces;

    // bump mapping
    struct Bump {
        float scale = 1.0;
        std::shared_ptr<QOpenGLTexture> tex;
    } bump;

    // displacement mapping
    struct Displacement {
        float scale = 1.0;
        std::shared_ptr<QOpenGLTexture> tex;
    } displacement;

    QVector2D translation = QVector2D(0.0, 0.0) ;

    // bind underlying shader program and set required uniforms
    void apply(unsigned int light_pass = 0) override;

};

