#pragma once

#include <QWidget>

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QOpenGLFramebufferObject>

#include "node.h"
#include "camera.h"
#include "skybox.h"
#include "material/texphong.h"
#include "material/postmaterial.h"
#include "navigator/position_navigator.h"
#include "navigator/modeltrackball.h"
#include "navigator/rotate_y.h"

#include <memory> // std::unique_ptr
#include <array>  //std::array
#include <map>    // std::map
#include <chrono> // clock, time calculations

/*
 * OpenGL-based scene. Required objects are created in the constructor,
 * and the scene is rendered using render().
 *
 * Also, the scene derives from QObject so it can use Qt's
 * signal and slot mechanism to connect to the app's UI.
 *
 * Do not call render() directly, use update() instead.
 *
 */

class Scene : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit Scene(QWidget* parent, QOpenGLContext *context);

    QMatrix4x4& worldTransform() { return nodes_["World"]->transformation; }

signals:

    // signal the scene emits when some Framebuffer Object (FBO) was updated
    void displayBufferContents(unsigned int id, QString label, const QImage& img);

public slots:

    // change background color
    void setBackgroundColor(QVector3D rgb);

    // methods to change common material parameters
    void toggleAnimation(bool flag);
<<<<<<< HEAD
    void setLightIntensity(size_t i, float v);
    void setAmbientScale(float v);
    void setDiffuseScale(float v);
    void setSpecularScale(float v);
    void setShininess(float v);
    void toggleEnvMap(bool v);
    void setMirrorScale(float v);
    void setRefractScale(float v);
    void setRefractRatio(float v);

    void toggleSkyBox(bool v);
    void setSkylightScale(float v);

    // methods affecting post processing
    void setPostFilterKernelSize(int n);
    void useSimpleBlur();
    void useTwoPassGauss();
    void toggleJittering(bool value);
    void toggleSplitDisplay(bool value);
    void toggleFBODisplay(bool value);

    // change the node to be rendered in the scene
    void setSceneNode(QString node);
=======
    void moveGround(QVector2D movement);

    void refreshTexture();
    void setLightIntensity(size_t, float v) {
        groundMaterial_->lights[0].intensity = v; update();
    }
    void setBlendExponent(float v) {
        planetMaterial_->planet.night_blend_exp= v*10.0; update();
    }
    void setNightScale(float v) {
        planetMaterial_->planet.night_scale = v*5.0; update();
    }
    void toggleBumpMapping(bool flag) {
        planetMaterial_->bump.use = flag; update();
    }
    void setBumpMapScale(float v) {
        groundMaterial_->bump.scale = v*3;
        vectorsMaterial_->bump.scale = v*3; update();
    }
    void toggleDisplacementMapping(bool flag) {
        planetMaterial_->displacement.use = flag; update();
    }
    void setDisplacementMapScale(float v) {
        groundMaterial_->displacement.scale = v/100 * 20;
        vectorsMaterial_->displacement.scale = v/100 * 20; update();
    }
    void toggleWireframe(bool flag)  {
        showWireframe = flag; update();
    }
    void visualizeVectors(int which) {
        // this mapping from some numbers to 0, 1, 2, 3 was determined heuristically :-)
        // qDebug() << "which: " << -2-which;
        vectorsMaterial_->vectorToShow = -2-which; update();
    }
    void setVectorScale(float v) {
        vectorsMaterial_->scale = v/10.0; update();
    }
>>>>>>> e17cfa1db82653f3a745c7c5e9e922b04f381038

    // key/mouse events from UI system, pass on to navigators or such
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    /*
     *  perform OpenGL rendering of the entire scene.
     *  Don't call this yourself (!) - use update() instead.
     *
     */
    void draw();

    // anyone can trigger a redraw of the widget through this method
    void update();

    // adjust camera / viewport / ... if drawing surface changes
    void updateViewport(size_t width, size_t height);

    // helper for creating a cube tex
    std::shared_ptr<QOpenGLTexture> makeCubeMap(std::string path_to_images, std::array<std::string, 6> sides);
    //{return std::make_shared<QOpenGLTexture>(QImage(":/textures/stone.jpg").mirrored());}

protected:

    // draw the actual scene
    void draw_scene_();

    // parent widget
    QWidget* parent_;

    // periodically update the scene for animations
    QTimer timer_;

    // track time for animations
    std::chrono::high_resolution_clock clock_;
    std::chrono::time_point<std::chrono::high_resolution_clock> firstDrawTime_;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastDrawTime_;

    // rotation
    double angle = 0.0;
    bool rotationOn = true;

    // bg color
    QVector3D bgcolor_ = QVector3D(0.4f,0.4f,0.4f);

<<<<<<< HEAD
    // draw from FBO for post processing, use full viewport
    void post_draw_full_(QOpenGLFramebufferObject& fbo, Node& node);
    // draw from FBO, render left half of node1 + right half of node2
    void post_draw_split_(QOpenGLFramebufferObject &fbo1, Node &node1, QOpenGLFramebufferObject &fbo2, Node &node2);

    // multi-pass rendering
    std::shared_ptr<QOpenGLFramebufferObject> fbo1_, fbo2_;
    std::map<QString, std::shared_ptr<PostMaterial>> post_materials_;
    bool split_display_ = true;
    bool show_FBOs_ = false;
=======
    // different materials to be demonstrated
    std::shared_ptr<PhongMaterial> material_;
    std::shared_ptr<PlanetMaterial> planetMaterial_;
    std::shared_ptr<GroundMaterial> groundMaterial_;
    std::shared_ptr<SkyBoxMaterial> skyBoxMaterial_;
    std::shared_ptr<WireframeMaterial> wireframeMaterial_;
    std::shared_ptr<VectorsMaterial> vectorsMaterial_;
>>>>>>> e17cfa1db82653f3a745c7c5e9e922b04f381038

    // different materials to be demonstrated
    std::map<QString, std::shared_ptr<TexturedPhongMaterial>> materials_;

    // mesh(es) to be used / shared
    std::map<QString, std::shared_ptr<Mesh>> meshes_;

    // nodes to be used
    std::map<QString, std::shared_ptr<Node>> nodes_;

    // skybox
    std::shared_ptr<SkyBox> skybox_;
    bool drawSkyBox_ = false;

    // light nodes for any number of lights
    std::vector<std::shared_ptr<Node>> lightNodes_;

    // navigation
    std::unique_ptr<ModelTrackball> navigator_;
    std::unique_ptr<PositionNavigator> lightNavigator_;
<<<<<<< HEAD
    std::unique_ptr<RotateY> cameraNavigator_;
=======
    std::unique_ptr<PlaneNavigator> planeNavigator_;

    QVector3D planespeed = QVector3D(0.0002f,0.0,0.0);
    bool plane_started = false;
    QVector2D rotation_angle = QVector2D(0.0,0.0);
    float elevation_angle_ = 15;
    float rotation_speed_ = 5.0;
>>>>>>> e17cfa1db82653f3a745c7c5e9e922b04f381038

    // helper for creating programs from shader files
    std::shared_ptr<QOpenGLShaderProgram> createProgram(const std::string& vertex,
                                                        const std::string& fragment,
                                                        const std::string& geom = "");

    // helper for creating a node scaled to size 1
    std::shared_ptr<Node> createNode(std::shared_ptr<Mesh> mesh, bool scale_to_1 = true);

    // helpers to construct the objects and to build the hierarchical scene
    void makeNodes();
    void makeScene();

};

