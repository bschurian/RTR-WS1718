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

    //modelAnimation
    void setEnemyStance();
    void moveEnemy();
    void togglePlayerVisibility(bool v);

    // methods to change common material parameters
    void toggleAnimation(bool flag);
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

    // key/mouse events from UI system, pass on to navigators or such
    void keyPressEvent(QKeyEvent *event);
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

    // game states
    int life = 5;
    //1-4 blocking stance
    int enemy_state = 0;
    bool blocking = true;
    double enemy_movement = 0;
    bool moveRight = true;

    //model display
    bool playerVisible = true;

    // rotation
    double angle = 0.0;
    bool rotationOn = true;

    // bg color
    QVector3D bgcolor_ = QVector3D(0.4f,0.4f,0.4f);

    // draw from FBO for post processing, use full viewport
    void post_draw_full_(QOpenGLFramebufferObject& fbo, QOpenGLFramebufferObject& fbo2, Node& node);
    // draw from FBO, render left half of node1 + right half of node2
    void post_draw_split_(QOpenGLFramebufferObject &fbo1, Node &node1, QOpenGLFramebufferObject &fbo2, QOpenGLFramebufferObject& fbo3, Node &node2);

    // multi-pass rendering
    std::shared_ptr<QOpenGLFramebufferObject> new_frame, fbo1_, fbo2_;
    std::map<QString, std::shared_ptr<PostMaterial>> post_materials_;
    bool split_display_ = true;
    bool show_FBOs_ = false;
    bool last_FBO_rendered_was_one = true;

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
    std::unique_ptr<RotateY> cameraNavigator_;

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

