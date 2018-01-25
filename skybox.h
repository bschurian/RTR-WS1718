#pragma once

#include "node.h"
#include "material/skyboxmaterial.h"

class SkyBox
{
    std::shared_ptr<SkyBoxMaterial> material_;
    std::shared_ptr<Node> world_, camera_;
    std::shared_ptr<Mesh> cube_;


public:
    SkyBox(std::shared_ptr<SkyBoxMaterial> material, std::shared_ptr<Node> world, std::shared_ptr<Node> camera);
    std::shared_ptr<SkyBoxMaterial> material() const { return material_; }

    // draws a box using the skybox material, but re-centers the camera to (0,0,0)
    void draw(const Camera &cam);

};

