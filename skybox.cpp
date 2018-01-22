#include "skybox.h"
#include "geometry/cube.h"

SkyBox::SkyBox(std::shared_ptr<SkyBoxMaterial> material,
               std::shared_ptr<Node> world,
               std::shared_ptr<Node> camera)
    : material_(material), world_(world), camera_(camera)
{
     cube_ = std::make_shared<Mesh>(std::make_shared<geom::Cube>(), material_);
}

void SkyBox::draw(const Camera &cam)
{
    // the camera's view matrix contains rotation and translation.
    // the sky is assumed to be inifitely far away, so translation
    // has no effect. So we eliminate it from the matrix.
    auto vm = cam.viewMatrix();
    auto pm = cam.projectionMatrix();
    vm.setColumn(3,QVector4D(0,0,0,1)); // eliminate translation
    Camera centeredCamera(vm,pm);
    centeredCamera.setShaderTransformationMatrices(*material_, QMatrix4x4());

    // disable depth writing (and testing, does not matter in this case)
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    // draw using centered (non-translated) camera
    cube_->draw(0);

}
