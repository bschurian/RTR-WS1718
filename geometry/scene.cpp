#include "scene.h"

#include <iostream> // std::cout etc.
#include <assert.h> // assert()
#include <random>   // random number generation

#include "geometry/cube.h"       // geom::Cube
#include "geometry/parametric.h" // geom::Sphere

#include <QtMath>
#include <QMessageBox>

using namespace std;

Scene::Scene(QWidget* parent, QOpenGLContext *context) :
    QOpenGLFunctions(context),
    parent_(parent),
    timer_(),
    firstDrawTime_(clock_.now()),
    lastDrawTime_(clock_.now())
{

    // check some OpenGL parameters
    {
        int minor, major;
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        cout << "OpenGL context version " << major << "." << minor << endl;

        int texunits_frag, texunits_vert;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texunits_frag);
        glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &texunits_vert);
        cout << "texture units: " << texunits_frag << " (frag), "
             << texunits_vert << " (vert)" << endl;

        int texsize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texsize);
        cout << "max texture size: " << texsize << "x" << texsize << endl;
    }

    // construct map of nodes
    makeNodes();

    // from the nodes, construct a hierarchical scene (adding more nodes)
    makeScene();

    // select initial model and shader
    setSceneNode("Sphere");
    setShader("Phong");

    // initialize navigation controllers
    cameraNavigator_ = std::make_unique<RotateY>(nodes_["Camera"], nullptr, nullptr);
    cameraNavigator_->setDistance(3.0);
    lightNavigator_ = std::make_unique<PositionNavigator>(nodes_["Light0"], nodes_["World"], nodes_["Camera"]);

    // make sure we redraw when the timer hits
    connect(&timer_, SIGNAL(timeout()), this, SLOT(update()) );

}

void Scene::makeNodes()
{
    // load shader source files and compile them into OpenGL program objects
    auto planet_prog = createProgram(":/shaders/planet_with_bumps.vert", ":/shaders/planet_with_bumps.frag");
    planetMaterial_ = std::make_shared<PlanetMaterial>(planet_prog);
    planetMaterial_->phong.shininess = 10;

    // program (with additional geometry shader) to visualize wireframe
    auto wire_prog = createProgram(":/shaders/wireframe.vert",
                                   ":/shaders/wireframe.frag",
                                   ":/shaders/wireframe.geom");
    wireframeMaterial_ = std::make_shared<WireframeMaterial>(wire_prog);

    // program (with additional geometry shader) to visualize normal/tangent vectors
    auto vectors_prog = createProgram(":/shaders/vectors.vert",
                                      ":/shaders/vectors.frag",
                                      ":/shaders/vectors.geom");
    vectorsMaterial_ = std::make_shared<VectorsMaterial>(vectors_prog);
    vectorsMaterial_->vectorToShow  = 0;

    // load textures
    auto day    = std::make_shared<QOpenGLTexture>(QImage(":/textures/earth_day.jpg").mirrored());
    auto night  = std::make_shared<QOpenGLTexture>(QImage(":/textures/earth_at_night_2048.jpg").mirrored());
    auto gloss  = std::make_shared<QOpenGLTexture>(QImage(":/textures/earth_bathymetry_2048.jpg").mirrored());
    auto clouds = std::make_shared<QOpenGLTexture>(QImage(":/textures/earth_clouds_2048.jpg").mirrored());
    auto disp   = std::make_shared<QOpenGLTexture>(QImage(":/textures/earth_topography_2048.jpg").mirrored());
    auto bumps  = std::make_shared<QOpenGLTexture>(QImage(":/textures/earth_topography_2048_NRM.png").mirrored());

    // tex parameters
    clouds->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
    clouds->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);

    // assign textures to material
    planetMaterial_->planet.dayTexture = day;
    planetMaterial_->planet.nightTexture = night;
    planetMaterial_->planet.glossTexture = gloss;
    planetMaterial_->planet.cloudsTexture = clouds;
    planetMaterial_->bump.tex = bumps;
    planetMaterial_->displacement.tex = disp;

    // load meshes from .obj files and assign shader programs to them
    auto std = planetMaterial_;
    meshes_["Duck"]    = std::make_shared<Mesh>(":/models/duck/duck.obj", std);
    meshes_["Teapot"]  = std::make_shared<Mesh>(":/models/teapot/teapot.obj", std);
    meshes_["Dwarf"]   = std::make_shared<Mesh>(":/models/dwarf/Dwarf_2_Low.obj", std);

    // add meshes of some procedural geometry objects (not loaded from OBJ files)
    meshes_["Cube"]   = std::make_shared<Mesh>(make_shared<geom::Cube>(), std);
    meshes_["Sphere"] = std::make_shared<Mesh>(make_shared<geom::Planet>(80,80), std);
    meshes_["Torus"]  = std::make_shared<Mesh>(make_shared<geom::Torus>(4, 2, 120,40), std);
    meshes_["Rect"]   = std::make_shared<Mesh>(make_shared<geom::Rect>(20,20), std);

    // pack each mesh into a scene node, along with a transform that scales
    // it to standard size [1,1,1]
    nodes_["Sphere"]  = createNode(meshes_["Sphere"], true);
    nodes_["Torus"]   = createNode(meshes_["Torus"], true);
    nodes_["Rect"]    = createNode(meshes_["Rect"], true);
    nodes_["Cube"]    = createNode(meshes_["Cube"], true);
    nodes_["Duck"]    = createNode(meshes_["Duck"], true);
    nodes_["Teapot"]  = createNode(meshes_["Teapot"], true);
    nodes_["Dwarf"]   = createNode(meshes_["Dwarf"], true);

    // rotate some models
    nodes_["Sphere"]->transformation.rotate(-90, QVector3D(1,0,0));
    nodes_["Torus"]->transformation.rotate(-60, QVector3D(1,0,0));
    nodes_["Rect"]->transformation.rotate(30, QVector3D(1,0,0));

}

// once the nodes_ map is filled, construct a hierarchical scene from it
void Scene::makeScene()
{
    // world contains the scene plus the camera
    nodes_["World"] = createNode(nullptr, false);

    // scene means everything but the camera
    nodes_["Scene"] = createNode(nullptr, false);
    nodes_["World"]->children.push_back(nodes_["Scene"]);

    // add camera node
    nodes_["Camera"] = createNode(nullptr, false);
    nodes_["World"]->children.push_back(nodes_["Camera"]);

    // add a light relative to the scene or world or camera
    nodes_["Light0"] = createNode(nullptr, false);
    lightNodes_.push_back(nodes_["Light0"]);

    // light attached to camera, placed right above camera
    nodes_["Camera"]->children.push_back(nodes_["Light0"]);
    nodes_["Light0"]->transformation.translate(QVector3D(10, 0, 0));

}

void Scene::setShader(QString txt)
{
    if(txt == "None") {
        material_ = nullptr;
    }
    else if(txt == "Phong") {
        material_ = planetMaterial_;
        planetMaterial_->planet.debug_texcoords = false;
        planetMaterial_->planet.debugWaterLand = false;
        planetMaterial_->planet.useDayTexture = false;
        planetMaterial_->planet.useNightTexture = false;
        planetMaterial_->planet.useGlossTexture = false;
        planetMaterial_->planet.useCloudsTexture = false;
    }
    else if(txt == "Debug Tex Coords") {
        material_ = planetMaterial_;
        planetMaterial_->planet.debug_texcoords = true;
        planetMaterial_->planet.debugWaterLand = false;
        planetMaterial_->planet.useDayTexture = false;
        planetMaterial_->planet.useNightTexture = false;
        planetMaterial_->planet.useGlossTexture = false;
        planetMaterial_->planet.useCloudsTexture = false;
    }
    else if(txt == "Debug Day/Night") {
        material_ = planetMaterial_;
        planetMaterial_->planet.debug_texcoords = false;
        planetMaterial_->planet.debug = true;
        planetMaterial_->planet.debugWaterLand = false;
        planetMaterial_->planet.useDayTexture = false;
        planetMaterial_->planet.useNightTexture = false;
        planetMaterial_->planet.useGlossTexture = false;
        planetMaterial_->planet.useCloudsTexture = false;
    }
    else if(txt == "Day Texture") {
        material_ = planetMaterial_;
        planetMaterial_->planet.debug_texcoords = false;
        planetMaterial_->planet.debug = false;
        planetMaterial_->planet.debugWaterLand = false;
        planetMaterial_->planet.useDayTexture = true;
        planetMaterial_->planet.useNightTexture = false;
        planetMaterial_->planet.useGlossTexture = false;
        planetMaterial_->planet.useCloudsTexture = false;
    }
    else if(txt == "Night Texture") {
        material_ = planetMaterial_;
        planetMaterial_->planet.debug_texcoords = false;
        planetMaterial_->planet.debug = false;
        planetMaterial_->planet.debugWaterLand = false;
        planetMaterial_->planet.useDayTexture = false;
        planetMaterial_->planet.useNightTexture = true;
        planetMaterial_->planet.useGlossTexture = false;
        planetMaterial_->planet.useCloudsTexture = false;
    }
    else if(txt == "Day+Night Texture") {
        material_ = planetMaterial_;
        planetMaterial_->planet.debug_texcoords = false;
        planetMaterial_->planet.debug = false;
        planetMaterial_->planet.debugWaterLand = false;
        planetMaterial_->planet.useDayTexture = true;
        planetMaterial_->planet.useNightTexture = true;
        planetMaterial_->planet.useGlossTexture = false;
        planetMaterial_->planet.useCloudsTexture = false;
    }
    else if(txt == "Debug Gloss") {
        material_ = planetMaterial_;
        planetMaterial_->planet.debug_texcoords = false;
        planetMaterial_->planet.debug = false;
        planetMaterial_->planet.debugWaterLand = true;
        planetMaterial_->planet.useDayTexture = false;
        planetMaterial_->planet.useNightTexture = false;
        planetMaterial_->planet.useGlossTexture = true;
        planetMaterial_->planet.useCloudsTexture = false;
    }
    else if(txt == "Phong+Gloss") {
        material_ = planetMaterial_;
        planetMaterial_->planet.debug_texcoords = false;
        planetMaterial_->planet.debug = false;
        planetMaterial_->planet.debugWaterLand = false;
        planetMaterial_->planet.useDayTexture = false;
        planetMaterial_->planet.useNightTexture = false;
        planetMaterial_->planet.useGlossTexture = true;
        planetMaterial_->planet.useCloudsTexture = false;
    }
    else if(txt == "Day+Night+Gloss") {
        material_ = planetMaterial_;
        planetMaterial_->planet.debug_texcoords = false;
        planetMaterial_->planet.debug = false;
        planetMaterial_->planet.debugWaterLand = false;
        planetMaterial_->planet.useDayTexture = true;
        planetMaterial_->planet.useNightTexture = true;
        planetMaterial_->planet.useGlossTexture = true;
        planetMaterial_->planet.useCloudsTexture = false;
    }
    else if(txt == "+Clouds") {
        material_ = planetMaterial_;
        planetMaterial_->planet.debug_texcoords = false;
        planetMaterial_->planet.debug = false;
        planetMaterial_->planet.debugWaterLand = false;
        planetMaterial_->planet.useDayTexture = true;
        planetMaterial_->planet.useNightTexture = true;
        planetMaterial_->planet.useGlossTexture = true;
        planetMaterial_->planet.useCloudsTexture = true;
    }

    update();
}

// this method is called implicitly by the Qt framework when a redraw is required.
void Scene::draw()
{
    // calculate animation time
    chrono::milliseconds millisec_since_first_draw;
    chrono::milliseconds millisec_since_last_draw;

    // calculate total elapsed time and time since last draw call
    auto current = clock_.now();
    millisec_since_first_draw = chrono::duration_cast<chrono::milliseconds>(current - firstDrawTime_);
    millisec_since_last_draw = chrono::duration_cast<chrono::milliseconds>(current - lastDrawTime_);
    lastDrawTime_ = current;

    // set time uniform in animated shader(s)
    float t = millisec_since_first_draw.count() / 1000.0f;
    planetMaterial_->time = t; // TODO not consistent with rtrApp

    draw_scene_();
}

void Scene::draw_scene_()
{

    // set camera based on node in scene graph
    float aspect = float(parent_->width())/float(parent_->height());
    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(30.0f,aspect,0.01f,1000.0f);

    auto camToWorld = nodes_["World"]->toParentTransform(nodes_["Camera"]);
    auto viewMatrix = camToWorld.inverted();
    Camera camera(viewMatrix, projectionMatrix);
    // qDebug() << viewMatrix;

    // clear background, set OpenGL state
    glClearColor(bgcolor_[0], bgcolor_[1], bgcolor_[2], 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // draw using currently selected material, if one is selected at all
    if(material_) {
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        replaceMaterialAndDrawScene(camera, material_);
    }

    // show wireframe in addition?
    if(showWireframe) {
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        replaceMaterialAndDrawScene(camera, wireframeMaterial_);
    }

    // show vectors in addition?
    if(vectorsMaterial_->vectorToShow != 0) {
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        replaceMaterialAndDrawScene(camera, vectorsMaterial_);
    }

}

void Scene::replaceMaterialAndDrawScene(const Camera& camera, shared_ptr<Material> material)
{
    // replace material in all meshes, if necessary
    if(material != meshes_.begin()->second->material()) {
        // qDebug() << "replacing material";
        for (auto& element : meshes_) {
            auto mesh = element.second;
            mesh->replaceMaterial(material);
        }
    }

    // draw one pass for each light
    // TODO: wireframe and vector materials always only require one pass
    for(unsigned int i=0; i<lightNodes_.size(); i++) {

        // qDebug() << "drawing light pass" << i;

        // determine current light position and set it in all materials
        QMatrix4x4 lightToWorld = nodes_["World"]->toParentTransform(lightNodes_[i]);
        planetMaterial_->lights[i].position_WC = lightToWorld * QVector3D(0,0,0);

        // draw light pass i
        nodes_["World"]->draw(camera, i);

        // settings for i>0 (add light contributions using alpha blending)
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE,GL_ONE);
        glDepthFunc(GL_EQUAL);
    }

}

// helper to load shaders and create programs
shared_ptr<QOpenGLShaderProgram>
Scene::createProgram(const string& vertex, const string& fragment, const string& geom)
{
    auto p = make_shared<QOpenGLShaderProgram>();
    if(!p->addShaderFromSourceFile(QOpenGLShader::Vertex, vertex.c_str()))
        qFatal("could not add vertex shader");
    if(!p->addShaderFromSourceFile(QOpenGLShader::Fragment, fragment.c_str()))
        qFatal("could not add fragment shader");
    if(!geom.empty()) {
        if(!p->addShaderFromSourceFile(QOpenGLShader::Geometry, geom.c_str()))
            qFatal("could not add geometry shader");
    }
    if(!p->link())
        qFatal("could not link shader program");

    return p;
}

// helper to make a node from a mesh, and
// scale the mesh to standard size 1 of desired
shared_ptr<Node>
Scene::createNode(shared_ptr<Mesh> mesh,
                  bool scale_to_1)
{
    QMatrix4x4 transform;
    if(scale_to_1) {
        float r = mesh->geometry()->bbox().maxExtent();
        transform.scale(QVector3D(1.0/r,1.0/r,1.0/r));
    }

    return make_shared<Node>(mesh,transform);
}


void Scene::toggleAnimation(bool flag)
{
    if(flag) {
        timer_.start(1000.0 / 60.0); // update *roughly* every 60 ms
    } else {
        timer_.stop();
    }
    planetMaterial_->planet.animateClouds = flag;

}

void Scene::setSceneNode(QString node)
{
    auto n = nodes_[node];
    assert(n);

    nodes_["Scene"]->children.clear();
    nodes_["Scene"]->children.push_back(n);

    update();
}

// pass key/mouse events on to navigator objects
void Scene::keyPressEvent(QKeyEvent *event) {

    // dispatch: when Modifier is pressed, navigate light, else camera
    if(event->modifiers() & Qt::AltModifier)
        lightNavigator_->keyPressEvent(event);
    else
        cameraNavigator_->keyPressEvent(event);

    update();

}
// mouse press events all processed by trackball navigator
void Scene::mousePressEvent(QMouseEvent *)
{
}
void Scene::mouseMoveEvent(QMouseEvent *)
{
}
void Scene::mouseReleaseEvent(QMouseEvent *)
{
}
void Scene::wheelEvent(QWheelEvent *)
{
}

// trigger a redraw of the widget through this method
void Scene::update()
{
    parent_->update();
}

void Scene::updateViewport(size_t width, size_t height)
{
    glViewport(0,0,GLint(width),GLint(height));
}


