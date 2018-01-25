#pragma once

#include "material/material.h"

class PostMaterial : public Material {
public:

    // constructor requires existing shader program
    PostMaterial(std::shared_ptr<QOpenGLShaderProgram> prog,
                 int texunit = 10) : Material(prog), tex_unit(texunit) {}

    // the texture to be post processed
    GLint post_texture_id;

    // the image size ("resolution") of the texture, needs to be set from outside
    QSize image_size;

    // kernel size of the filter to be applied
    QSize kernel_size = QSize(5,5);

    // use jittering of sample points?
    bool use_jitter = false;

    // texture unit to be used
    int tex_unit;

    // bind underlying shader program and set required uniforms
    void apply(unsigned int light_pass = 0) override;

};


