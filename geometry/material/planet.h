#pragma once

#include "material/material.h"
#include "material/phong.h"

#include <QOpenGLTexture>
#include <memory>


class PlanetMaterial : public PhongMaterial {
public:

    // constructor requires existing shader program
    PlanetMaterial(std::shared_ptr<QOpenGLShaderProgram> prog)
        : PhongMaterial(prog) {}

    // planet-specific properties
    struct Planet {
        bool useDayTexture = false;
        bool useNightTexture = false;
        bool useGlossTexture = false;
        bool useCloudsTexture = false;
        std::shared_ptr<QOpenGLTexture> dayTexture;
        std::shared_ptr<QOpenGLTexture> nightTexture;
        std::shared_ptr<QOpenGLTexture> glossTexture;
        std::shared_ptr<QOpenGLTexture> cloudsTexture;
        float night_scale = 1.0;
        float night_blend_exp = 3.0;
        bool debug = false;
        bool debug_texcoords = false;
        bool debugWaterLand = false;
        bool animateClouds = false;
    } planet;

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

    // bind underlying shader program and set required uniforms
    void apply(unsigned int light_pass = 0) override;

};

