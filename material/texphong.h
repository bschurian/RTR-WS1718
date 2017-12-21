#pragma once

#include "material/phong.h"


class TexturedPhongMaterial : public PhongMaterial {
public:

    // constructor requires existing shader program
    TexturedPhongMaterial(std::shared_ptr<QOpenGLShaderProgram> prog, int texunit = 0)
        : PhongMaterial(prog)
    { tex.tex_unit = texunit; }

    // texturing-specific properties
    struct Textures {
        bool useDiffuseTexture = false;
        bool useEmissiveTexture = false;
        bool useGlossTexture = false;
        bool useEnvironmentTexture = false;
        std::shared_ptr<QOpenGLTexture> diffuseTexture;
        std::shared_ptr<QOpenGLTexture> emissiveTexture;
        std::shared_ptr<QOpenGLTexture> glossTexture;
        std::shared_ptr<QOpenGLTexture> environmentTexture;
        float emissive_scale = 1.0;
        int tex_unit;     // first texture unit to be used
    } tex;

    // bump mapping
    struct Bump {
        bool use = false;
        float scale = 1.0;
        float debug = false;
        std::shared_ptr<QOpenGLTexture> tex;
    } bump;

    // displacement mapping
    struct Displacement {
        bool use = false;
        float scale = 1.0;
        std::shared_ptr<QOpenGLTexture> tex;
    } displacement;

    // environment mapping
    struct EnvMap {
        QVector3D k_mirror   = QVector3D(0.5f,0.5f,0.5f);
        QVector3D k_refract  = QVector3D(0,0,0);
        float refract_ratio = 1.5;
    } envmap;

    // bind underlying shader program and set required uniforms
    virtual void apply(unsigned int light_pass = 0) override;

};

