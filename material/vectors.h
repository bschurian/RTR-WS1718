#pragma once

#include "material/material.h"
#include <QOpenGLTexture>
#include <memory>


// material to visualize normal, tangent, bitangent
class VectorsMaterial : public Material {
public:

    // constructor requires existing shader program
    VectorsMaterial(std::shared_ptr<QOpenGLShaderProgram> prog) : Material(prog) {}


    int vectorToShow = 1; // 1=normal, 2=tangent, 3=bitangent
    QVector3D vectorColor = QVector3D(1,1,0);
    float scale = true;

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

    // bind underlying shader program and set required uniforms
    void apply(unsigned int = 0) override;

};

