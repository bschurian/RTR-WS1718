#pragma once

#include "material/animatedmaterial.h"

#include <QOpenGLTexture>


class CartoonMaterial : public AnimatedMaterial {
public:

    // constructor requires existing shader program
    CartoonMaterial(std::shared_ptr<QOpenGLShaderProgram> prog) : AnimatedMaterial(prog)
    {
        lights.push_back(PointLight()); // make sure there is at least one light
    }

    // bind underlying shader program and set required uniforms
    void apply(unsigned int light_pass = 0) override;

    // animation time
    float time = 0.0;

    // ambient light
    QVector3D ambientLightIntensity = QVector3D(0.3f,0.3f,0.3f);

    // properties of the Cel aspects of the material
    struct Cel {
        QVector3D k_ambient  = QVector3D(0.10f,0.10f,0.50f); // red-ish
        QVector3D k_diffuse  = QVector3D(0.10f,0.50f,0.10f); // green-ish
        QVector3D k_specular = QVector3D(0.80f,0.80f,0.80f); // white-ish
        int shades  = 1;
        float     shininess  = 80; // middle-ish
    } cel;

};


