#include "scene.h"

#include <iostream> // std::cout etc.
#include <assert.h> // assert()
#include <random>   // random number generation

#include "geometry/cube.h" // geom::Cube
#include "geometry/parametric.h" // geom::Sphere, geom::Torus

#include "cubemap.h"

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

    // initialize navigation
    navigator_ = std::make_unique<ModelTrackball>(nodes_["Scene"], nodes_["World"], nodes_["Camera"]);
    lightNavigator_ = std::make_unique<PositionNavigator>(nodes_["Light0"], nodes_["World"], nodes_["Camera"]);
    cameraNavigator_ = std::make_unique<RotateY>(nodes_["Camera"], nodes_["World"], nodes_["Camera"]);

<<<<<<< HEAD
    // move camera away from scene
    cameraNavigator_->setDistance(3.0);
=======
    // initialize navigation controllers
    cameraNavigator_ = std::make_unique<RotateY>(nodes_["Camera"], nullptr, nullptr);
    lightNavigator_ = std::make_unique<PositionNavigator>(nodes_["Light0"], nodes_["World"], nodes_["Camera"]);
    planeNavigator_ = std::make_unique<PlaneNavigator>(nodes_["Rect"], nodes_["World"], nodes_["Camera"]);
    cameraNavigator_->setDistance(0.55);
>>>>>>> e17cfa1db82653f3a745c7c5e9e922b04f381038

    // make sure we redraw every frame
    timer_.start(0);
    connect(&timer_, SIGNAL(timeout()), this, SLOT(update()) );

}

void Scene::makeNodes()
{
<<<<<<< HEAD
    // load textures
    std::shared_ptr<QOpenGLTexture> cubetex = makeCubeMap(":/textures/bridge2048");
    std::shared_ptr<QOpenGLTexture> stdtex = std::make_shared<QOpenGLTexture>(QImage(":/textures/RTR-ist-super-4-3.png"));

    // make sky box material (tex unit 1)
    auto sky_prog = createProgram(":/shaders/skybox.vert", ":/shaders/skybox.frag");
    auto skymat = make_shared<SkyBoxMaterial>(sky_prog,1);

    // sky box object, can draw a skybox around a give camera, not part of the scene
    skymat->texture = cubetex;
    skybox_ = make_shared<SkyBox>(skymat, nullptr, nullptr);

    // load shader source files and compile them into OpenGL program objects
    auto phong_prog = createProgram(":/shaders/textured_phong.vert", ":/shaders/textured_phong.frag");

    // instance of textured Phong material, starting with tex unit 2
    materials_["red"] = std::make_shared<TexturedPhongMaterial>(phong_prog,2);
    materials_["red"]->phong.k_diffuse = QVector3D(0.8f,0.1f,0.1f);
    materials_["red"]->phong.k_ambient = materials_["red"]->phong.k_diffuse * 0.3f;
    materials_["red"]->phong.shininess = 80;
    materials_["red"]->tex.useEnvironmentTexture = true;
    materials_["red"]->tex.environmentTexture = cubetex;
    materials_["red"]->tex.useDiffuseTexture = true;
    materials_["red"]->tex.diffuseTexture = stdtex;

    // copy of the material, for changing values relative to original value
    materials_["red_original"] = std::make_shared<TexturedPhongMaterial>(*materials_["red"]);
    auto std = materials_["red"];

    // post processing materials, using separate tex units 10-12
    auto orig = createProgram(":/shaders/post.vert",
                              ":/shaders/original.frag");
    post_materials_["original"] = make_shared<PostMaterial>(orig, 10);

    auto blur = createProgram(":/shaders/post.vert",
                              ":/shaders/blur.frag");
    post_materials_["blur"] = make_shared<PostMaterial>(blur, 11);

    auto gaussA = createProgram(":/shaders/post.vert",
                                ":/shaders/gauss_9x9_passA.frag");
    auto gaussB = createProgram(":/shaders/post.vert",
                                ":/shaders/gauss_9x9_passB.frag");
    post_materials_["gauss_1"] = make_shared<PostMaterial>(gaussA,11);
    post_materials_["gauss_2"] = make_shared<PostMaterial>(gaussB,12);
=======
    // load shader source files and compile them into OpenGL program objects
    auto planet_prog = createProgram(":/shaders/planet_with_bumps.vert", ":/shaders/planet_with_bumps.frag");
    planetMaterial_ = std::make_shared<PlanetMaterial>(planet_prog);
    planetMaterial_->phong.shininess = 10;

    auto ground_prog = createProgram(":/shaders/ground.vert", ":/shaders/ground.frag");
    groundMaterial_ = std::make_shared<GroundMaterial>(ground_prog);
    groundMaterial_->phong.shininess = 10;

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
    auto groundDisp  = std::make_shared<QOpenGLTexture>(QImage(":/textures/noise.png").mirrored());
    auto groundBump  = std::make_shared<QOpenGLTexture>(QImage(":/textures/noiseNorm.png").mirrored());
    auto grass       = std::make_shared<QOpenGLTexture>(QImage(":/textures/grass.jpg").mirrored());
    auto gravel      = std::make_shared<QOpenGLTexture>(QImage(":/textures/geroell.jpg").mirrored());
    auto sand        = std::make_shared<QOpenGLTexture>(QImage(":/textures/sand.jpg").mirrored());
    auto stone       = std::make_shared<QOpenGLTexture>(QImage(":/textures/stone.jpg").mirrored());

    // tex parameters
    clouds->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
    clouds->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);
    groundDisp->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
    groundDisp->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);
    groundBump->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
    groundBump->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);

    // assign textures to material
    planetMaterial_->planet.dayTexture = day;
    planetMaterial_->planet.nightTexture = night;
    planetMaterial_->planet.glossTexture = gloss;
    planetMaterial_->planet.cloudsTexture = clouds;
    planetMaterial_->bump.tex = bumps;
    planetMaterial_->displacement.tex = disp;
>>>>>>> e17cfa1db82653f3a745c7c5e9e922b04f381038

    // assign textures to material v2
    groundMaterial_->phong.k_ambient = QVector3D(0.2f, 0.2f, 0.25f);
    groundMaterial_->surfaces.grassTexture = grass;
    groundMaterial_->surfaces.gravelTexture = gravel;
    groundMaterial_->surfaces.sandTexture = sand;
    groundMaterial_->surfaces.stoneTexture = stone;
    groundMaterial_->bump.tex = groundBump;
    groundMaterial_->displacement.tex = groundDisp;

    vectorsMaterial_->bump.tex = groundBump;
    vectorsMaterial_->displacement.tex = groundDisp;

    // load meshes from .obj files and assign shader programs to them
<<<<<<< HEAD
=======
    auto std = groundMaterial_;
>>>>>>> e17cfa1db82653f3a745c7c5e9e922b04f381038
    meshes_["Duck"]    = std::make_shared<Mesh>(":/models/duck/duck.obj", std);
    meshes_["Teapot"]  = std::make_shared<Mesh>(":/models/teapot/teapot.obj", std);

    // add meshes of some procedural geometry objects (not loaded from OBJ files)
    meshes_["Cube"]   = std::make_shared<Mesh>(make_shared<geom::Cube>(), std);
<<<<<<< HEAD
    meshes_["Sphere"] = std::make_shared<Mesh>(make_shared<geom::Sphere>(80,80), std);
    meshes_["Torus"]  = std::make_shared<Mesh>(make_shared<geom::Torus>(4, 2, 80,20), std);

    // full-screen rectangles for post processing
    meshes_["original"]  = std::make_shared<Mesh>(make_shared<geom::RectXY>(1, 1),
                                                  post_materials_["original"]);
    nodes_["original"]   = createNode(meshes_["original"], false);

    meshes_["blur"]      = std::make_shared<Mesh>(make_shared<geom::RectXY>(1, 1),
                                                  post_materials_["blur"]);
    nodes_["blur"]       = createNode(meshes_["blur"], false);

    meshes_["gauss_1"]   = std::make_shared<Mesh>(make_shared<geom::RectXY>(1, 1), post_materials_["gauss_1"]);
    nodes_ ["gauss_1"]   = createNode(meshes_["gauss_1"], false);
    meshes_["gauss_2"]   = std::make_shared<Mesh>(make_shared<geom::RectXY>(1, 1), post_materials_["gauss_2"]);
    nodes_ ["gauss_2"]   = createNode(meshes_["gauss_2"], false);

    // initial state of post processing phases
    nodes_["post_pass_1"] = nodes_["blur"];
    nodes_["post_pass_2"] = nullptr;
=======
    meshes_["Sphere"] = std::make_shared<Mesh>(make_shared<geom::Planet>(80,80), std);
    meshes_["Torus"]  = std::make_shared<Mesh>(make_shared<geom::Torus>(4, 2, 120,40), std);
    meshes_["Rect"]   = std::make_shared<Mesh>(make_shared<geom::Rect>(1000,1000), std);
>>>>>>> e17cfa1db82653f3a745c7c5e9e922b04f381038

    // pack each mesh into a scene node, along with a transform that scales
    // it to standard size [1,1,1]
    nodes_["Cube"]    = createNode(meshes_["Cube"], true);
    nodes_["Sphere"]  = createNode(meshes_["Sphere"], true);
    nodes_["Torus"]   = createNode(meshes_["Torus"], true);
    nodes_["Duck"]    = createNode(meshes_["Duck"], true);
    nodes_["Teapot"]  = createNode(meshes_["Teapot"], true);
<<<<<<< HEAD
=======
    nodes_["Dwarf"]   = createNode(meshes_["Dwarf"], true);

    // rotate some models
    nodes_["Sphere"]->transformation.rotate(-90, QVector3D(1,0,0));
    nodes_["Torus"]->transformation.rotate(-60, QVector3D(1,0,0));
//    nodes_["Rect"]->transformation.rotate(30, QVector3D(1,0,0));

>>>>>>> e17cfa1db82653f3a745c7c5e9e922b04f381038

}

// once the nodes_ map is filled, construct a hierarchical scene from it
void Scene::makeScene()
{
    // world contains the scene plus the camera
    nodes_["World"] = createNode(nullptr, false);

    // scene means everything but the camera
    nodes_["Scene"] = createNode(nullptr, false);
    nodes_["World"]->children.push_back(nodes_["Scene"]);

<<<<<<< HEAD
    // initial model to be shown in the scene
    nodes_["Scene"]->children.push_back(nodes_["Cube"]);
=======
    //create SkyBox
    // load shader source files and compile them into OpenGL program objects
    auto skybox_prog = createProgram(":/shaders/cube_mapping.vert", ":/shaders/cube_mapping.frag");
    skyBoxMaterial_ = std::make_shared<SkyBoxMaterial>(skybox_prog);
    auto skyFront       = std::make_shared<QOpenGLTexture>(QImage(":/textures/siege_bft.tga").mirrored());
    std::array<string, 6> file_names =
    {"real_dayrt.jpg","real_dayup.jpg","real_daybk.jpg",
      "real_daylf.jpg", "real_daydn.jpg", "real_dayft.jpg"};

    skyBoxMaterial_->sky = makeCubeMap(":/textures", file_names);

    nodes_["SkyBox"] = createNode(std::make_shared<Mesh>(make_shared<geom::Cube>(), skyBoxMaterial_), false);
    nodes_["SkyBox"]->transformation.scale(QVector3D(10, 10, 10));
    nodes_["World"]->children.push_back(nodes_["SkyBox"]);
>>>>>>> e17cfa1db82653f3a745c7c5e9e922b04f381038

    // add camera node
    nodes_["Camera"] = createNode(nullptr, false);
    nodes_["World"]->children.push_back(nodes_["Camera"]);

    // add a light relative to the world
    nodes_["Light0"] = createNode(nullptr, false);
    nodes_["World"]->children.push_back(nodes_["Light0"]);
    lightNodes_.push_back(nodes_["Light0"]);
<<<<<<< HEAD
    nodes_["Light0"]->transformation.translate(QVector3D(-0.55f, 0.68f, 4.34f)); // above camera
=======

    // light attached to camera, placed right above camera
    nodes_["World"]->children.push_back(nodes_["Light0"]);
    nodes_["Light0"]->transformation.translate(QVector3D(0, 10, 0));


}


std::shared_ptr<QOpenGLTexture>
Scene::makeCubeMap(string path_to_images, std::array<string, 6> sides)
{

    //load six images for the six sides of the cube
    std::vector<QImage> images;
    for(auto side : sides){
        QString filename = (path_to_images + "/" + side).c_str();
        images.push_back( QImage(filename).
                          convertToFormat(QImage::Format_RGBA8888));
    }
>>>>>>> e17cfa1db82653f3a745c7c5e9e922b04f381038

    //create and allocate cube map texture
    std::shared_ptr<QOpenGLTexture> tex_;
    tex_ =std::make_shared<QOpenGLTexture>(QOpenGLTexture::TargetCubeMap);
    tex_->create();
    tex_->setSize(images[0].width(), images[0].height(), images[0].depth());
    tex_->setFormat(QOpenGLTexture::RGBA8_UNorm);
    tex_->allocateStorage();

    //the file names in array sides must match this of
    std::array<QOpenGLTexture::CubeMapFace, 6> faces =
    {{QOpenGLTexture::CubeMapPositiveX, QOpenGLTexture::CubeMapPositiveY,
     QOpenGLTexture::CubeMapPositiveZ, QOpenGLTexture::CubeMapNegativeX,
     QOpenGLTexture::CubeMapNegativeY, QOpenGLTexture::CubeMapNegativeZ}};

    //set texture image data for each side
    for(auto i : {0,1,2,3,4,5}){
        tex_->setData( 0, 0, faces[i],
                       QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                       (const void*)images[i].constBits(), 0);
    }

    // texture parameters and mip-map generation
    tex_->setWrapMode(QOpenGLTexture::ClampToEdge);
    tex_->setMagnificationFilter(QOpenGLTexture::Linear);
    tex_->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    tex_->generateMipMaps();

    return tex_;
}

<<<<<<< HEAD
=======
void Scene::setShader(QString txt)
{
    if(txt == "None") {
        material_ = nullptr;
    }
    else if(txt == "Phong") {
        material_ = groundMaterial_;
        planetMaterial_->planet.debug_texcoords = false;
        planetMaterial_->planet.debugWaterLand = false;
        planetMaterial_->planet.useDayTexture = false;
        planetMaterial_->planet.useNightTexture = false;
        planetMaterial_->planet.useGlossTexture = false;
        planetMaterial_->planet.useCloudsTexture = false;
    }
    else if(txt == "Debug Tex Coords") {
        material_ = groundMaterial_;
        planetMaterial_->planet.debug_texcoords = true;
        planetMaterial_->planet.debugWaterLand = false;
        planetMaterial_->planet.useDayTexture = false;
        planetMaterial_->planet.useNightTexture = false;
        planetMaterial_->planet.useGlossTexture = false;
        planetMaterial_->planet.useCloudsTexture = false;
    }
    else if(txt == "Debug Day/Night") {
        material_ = groundMaterial_;
        planetMaterial_->planet.debug_texcoords = false;
        planetMaterial_->planet.debug = true;
        planetMaterial_->planet.debugWaterLand = false;
        planetMaterial_->planet.useDayTexture = false;
        planetMaterial_->planet.useNightTexture = false;
        planetMaterial_->planet.useGlossTexture = false;
        planetMaterial_->planet.useCloudsTexture = false;
    }
    else if(txt == "Day Texture") {
        material_ = groundMaterial_;
        planetMaterial_->planet.debug_texcoords = false;
        planetMaterial_->planet.debug = false;
        planetMaterial_->planet.debugWaterLand = false;
        planetMaterial_->planet.useDayTexture = true;
        planetMaterial_->planet.useNightTexture = false;
        planetMaterial_->planet.useGlossTexture = false;
        planetMaterial_->planet.useCloudsTexture = false;
    }
    else if(txt == "Night Texture") {
        material_ = groundMaterial_;
        planetMaterial_->planet.debug_texcoords = false;
        planetMaterial_->planet.debug = false;
        planetMaterial_->planet.debugWaterLand = false;
        planetMaterial_->planet.useDayTexture = false;
        planetMaterial_->planet.useNightTexture = true;
        planetMaterial_->planet.useGlossTexture = false;
        planetMaterial_->planet.useCloudsTexture = false;
    }
    else if(txt == "Day+Night Texture") {
        material_ = groundMaterial_;
        planetMaterial_->planet.debug_texcoords = false;
        planetMaterial_->planet.debug = false;
        planetMaterial_->planet.debugWaterLand = false;
        planetMaterial_->planet.useDayTexture = true;
        planetMaterial_->planet.useNightTexture = true;
        planetMaterial_->planet.useGlossTexture = false;
        planetMaterial_->planet.useCloudsTexture = false;
    }
    else if(txt == "Debug Gloss") {
        material_ = groundMaterial_;
        planetMaterial_->planet.debug_texcoords = false;
        planetMaterial_->planet.debug = false;
        planetMaterial_->planet.debugWaterLand = true;
        planetMaterial_->planet.useDayTexture = false;
        planetMaterial_->planet.useNightTexture = false;
        planetMaterial_->planet.useGlossTexture = true;
        planetMaterial_->planet.useCloudsTexture = false;
    }
    else if(txt == "Phong+Gloss") {
        material_ = groundMaterial_;
        planetMaterial_->planet.debug_texcoords = false;
        planetMaterial_->planet.debug = false;
        planetMaterial_->planet.debugWaterLand = false;
        planetMaterial_->planet.useDayTexture = false;
        planetMaterial_->planet.useNightTexture = false;
        planetMaterial_->planet.useGlossTexture = true;
        planetMaterial_->planet.useCloudsTexture = false;
    }
    else if(txt == "Day+Night+Gloss") {
        material_ = groundMaterial_;
        planetMaterial_->planet.debug_texcoords = false;
        planetMaterial_->planet.debug = false;
        planetMaterial_->planet.debugWaterLand = false;
        planetMaterial_->planet.useDayTexture = true;
        planetMaterial_->planet.useNightTexture = true;
        planetMaterial_->planet.useGlossTexture = true;
        planetMaterial_->planet.useCloudsTexture = false;
    }
    else if(txt == "+Clouds") {
        material_ = groundMaterial_;
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
>>>>>>> e17cfa1db82653f3a745c7c5e9e922b04f381038

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
    for(auto mat : materials_)
        mat.second->time = t;

    // create an FBO to render the scene into
    if(!fbo1_) {

        // for high-res Retina displays
        auto pixel_scale = parent_->devicePixelRatio();

        // what kind of FBO do we want?
        auto fbo_format = QOpenGLFramebufferObjectFormat();
        fbo_format.setAttachment(QOpenGLFramebufferObject::Depth);

        // create some FBOs for post processing
        fbo1_ = std::make_shared<QOpenGLFramebufferObject>(parent_->width()*pixel_scale,
                                                           parent_->height()*pixel_scale,
                                                           fbo_format);
        fbo2_ = std::make_shared<QOpenGLFramebufferObject>(parent_->width()*pixel_scale,
                                                           parent_->height()*pixel_scale,
                                                           fbo_format);
        qDebug() << "Creating FBOs with size" << fbo1_->size();
    }

<<<<<<< HEAD
    // draw the actual scene into fbo1
    fbo1_->bind();
=======
    //plane move texture
    if(plane_started)
        refreshTexture();

>>>>>>> e17cfa1db82653f3a745c7c5e9e922b04f381038
    draw_scene_();
    fbo1_->release();
    auto fbo_to_be_rendered = fbo1_;
    auto node_to_be_rendered = nodes_["post_pass_1"];

    // second pass?
    if(nodes_["post_pass_2"]) {
        fbo2_->bind();
        post_draw_full_(*fbo_to_be_rendered, *node_to_be_rendered);
        fbo2_->release();
        fbo_to_be_rendered = fbo2_;
        node_to_be_rendered = nodes_["post_pass_2"];
    }

    // final rendering pass, into visible framebuffer (object)
    if(split_display_) {
        post_draw_split_(*fbo1_, *nodes_["original"],
                         *fbo_to_be_rendered, *node_to_be_rendered);
    } else {
        post_draw_full_(*fbo_to_be_rendered, *node_to_be_rendered);
    }

    // extract FBI image and display in the UI, every 20 frames
    static size_t framecount=20-2; // initially will render twice
    if(show_FBOs_) {
        if(++framecount % 20 == 0) {
            emit displayBufferContents(0, "rendered scene", fbo1_->toImage());
            if(nodes_["post_pass_2"])
                emit displayBufferContents(1, "post pass 1", fbo2_->toImage());
        }
    }

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

    // clear buffer
    glClearColor(bgcolor_[0], bgcolor_[1], bgcolor_[2], 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // initial pass: draw skybox, does not modify depth buffer
    glDisable(GL_BLEND);
    if(drawSkyBox_)
        skybox_->draw(camera);

    // first light pass: standard depth test, no blending
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    // draw one pass for each light
    for(unsigned int i=0; i<lightNodes_.size(); i++) {

        // determine current light position and set it in all materials
        QMatrix4x4 lightToWorld = nodes_["World"]->toParentTransform(lightNodes_[i]);
<<<<<<< HEAD
        for(auto mat : materials_) {
            auto phong = mat.second; // mat is of type (key, value)
            phong->lights[i].position_WC = lightToWorld * QVector3D(0,0,0);
        }
=======
        planetMaterial_->lights[i].position_WC = lightToWorld * QVector3D(0,0,0);
        groundMaterial_->lights[i].position_WC = lightToWorld * QVector3D(0,0,0);
>>>>>>> e17cfa1db82653f3a745c7c5e9e922b04f381038

        // draw light pass i
        nodes_["World"]->draw(camera, i);

        // settings for i>0 (add light contributions using alpha blending)
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE,GL_ONE);
        glDepthFunc(GL_EQUAL);
    }
}

void Scene::post_draw_full_(QOpenGLFramebufferObject &fbo, Node& node)
{
    // set up camera for post processing
    QMatrix4x4 view, projection;
    projection.ortho(-1,1,-1,1,-1,1);
    Camera camera(view, projection);

    // width and height of FBO / viewport
    int w = fbo.size().width();
    int h = fbo.size().height();

    // use the texture from the FBO during rendering
    for(auto mat : post_materials_) {
        mat.second->post_texture_id = fbo.texture();
        mat.second->image_size = QSize(w,h);
    }

    // initial state for drawing full-viewport rectangles
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    // draw single full screen rectangle with post processing material
    node.draw(camera);
}

void Scene::post_draw_split_(QOpenGLFramebufferObject &fbo1, Node& node1,
                             QOpenGLFramebufferObject &fbo2, Node& node2)
{
    // set up camera for post processing
    QMatrix4x4 view, projection;
    projection.ortho(-1,1,-1,1,-1,1);
    Camera camera(view, projection);

    // width and height of FBO / viewport
    int w = fbo1.size().width();
    int h = fbo1.size().height();
    int halfw = w/2;

    // initial state for drawing full-viewport rectangles
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    // left half of node1

    // use texture from fbo1 during rendering
    for(auto mat : post_materials_) {
        mat.second->post_texture_id = fbo1.texture();
        mat.second->image_size = QSize(w,h);
    }
    glEnable(GL_SCISSOR_TEST);
    glScissor(0,0,halfw,h);
    node1.draw(camera);

    // right half of node2

    // use texture from fbo2 during rendering
    for(auto mat : post_materials_) {
        mat.second->post_texture_id = fbo2.texture();
        mat.second->image_size = QSize(w,h);
    }

    glScissor(halfw,0,w-halfw,h);
    node2.draw(camera);
    glDisable(GL_SCISSOR_TEST);
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
}

void Scene::setSceneNode(QString node)
{
    auto n = nodes_[node];
    assert(n);

    nodes_["Scene"]->children.clear();
    nodes_["Scene"]->children.push_back(n);

    update();
}

<<<<<<< HEAD
// methods to change scene parameters ---------------------------------------------------------
void Scene::setBackgroundColor(QVector3D rgb) {
    bgcolor_ = rgb; update();
}
void Scene::setLightIntensity(size_t i, float v)
{
    if(i>=lightNodes_.size())
        return;
    for(auto mat : materials_)
        mat.second->lights[i].intensity = v; update();
}
void Scene::setAmbientScale(float v)
{
    materials_["red"]->phong.k_ambient = materials_["red_original"]->phong.k_ambient * v;
    update();
}
void Scene::setDiffuseScale(float v)
{
    materials_["red"]->phong.k_diffuse = materials_["red_original"]->phong.k_diffuse * v;
    update();
}
void Scene::setSpecularScale(float v)
{
    materials_["red"]->phong.k_specular = materials_["red_original"]->phong.k_specular * v;
    update();
}
void Scene::setShininess(float v)
{
    materials_["red"]->phong.shininess = v;
    update();
}
void Scene::toggleEnvMap(bool v)
{
    materials_["red"]->tex.useEnvironmentTexture = v;
    update();
}
void Scene::toggleSkyBox(bool v)
{
    drawSkyBox_ = v;
    update();
}

void Scene::setSkylightScale(float v) {
    skybox_->material()->intensity_scale = v; update();
}

void Scene::setMirrorScale(float v)
{
    materials_["red"]->envmap.k_mirror = materials_["red_original"]->envmap.k_mirror * v;
    update();
}
void Scene::setRefractScale(float v)
{
    materials_["red"]->envmap.k_refract= materials_["red_original"]->envmap.k_refract * v;
    update();
}
void Scene::setRefractRatio(float v)
{
    materials_["red"]->envmap.refract_ratio = v;
    update();
}
=======
void Scene::moveGround(QVector2D movement)
{
    QMatrix4x4 mat;

    mat.translate(0, 0, 0.5);
    mat.rotate(rotation_angle.x(), QVector3D(0,1,0));

    nodes_["Camera"]->transformation.rotate(rotation_angle.x()*180/3.14159265359, QVector3D(0,1,0));
    groundMaterial_->translation += QVector2D(movement.y(),movement.x());
//    if(rotation_angle.x()!=0){
//        nodes_["Camera"]->transformation = mat;
//    }
}

// pass key/mouse events on to navigator objects
void Scene::keyPressEvent(QKeyEvent *event) {

    if(plane_started==false && event->key() == Qt::Key_Up)
        plane_started = true;
    // dispatch: when Modifier is pressed, navigate light, else camera
    if(event->modifiers() & Qt::AltModifier)
        lightNavigator_->keyPressEvent(event);
    else
        //cameraNavigator_->keyPressEvent(event);
        planeNavigator_->keyPressEvent(event);
>>>>>>> e17cfa1db82653f3a745c7c5e9e922b04f381038

// change kernel size of al post processing filters
void Scene::setPostFilterKernelSize(int n) {
    for(auto mat : post_materials_)
        mat.second->kernel_size = QSize(n,n);
    update();
}
<<<<<<< HEAD

// change post processing filter
void Scene::useSimpleBlur() {
    nodes_["post_pass_1"] = nodes_["blur"];
    nodes_["post_pass_2"] = nullptr;
    update();
}
void Scene::useTwoPassGauss() {
    nodes_["post_pass_1"] = nodes_["gauss_1"];
    nodes_["post_pass_2"] = nodes_["gauss_2"];
    update();
=======
void Scene::keyReleaseEvent(QKeyEvent *event){
    planeNavigator_->keyReleaseEvent(event);
>>>>>>> e17cfa1db82653f3a745c7c5e9e922b04f381038
}
void Scene::toggleJittering(bool value)
{
    for(auto m : post_materials_) m.second->use_jitter = value;
    update();
}
void Scene::toggleSplitDisplay(bool value)
{
    split_display_ = value;
    update();
}
void Scene::toggleFBODisplay(bool value)
{
    show_FBOs_ = value;
    update();
}

// navigation -------------------------------------------------------------------------------

void Scene::keyPressEvent(QKeyEvent *event) {

    // if Alt is pressed, pass on to light navigator, else camera navigator
     if(event->modifiers() & Qt::AltModifier) {
         lightNavigator_->keyPressEvent(event);
     } else {
         cameraNavigator_->keyPressEvent(event);
     }
     update();

}
void Scene::mousePressEvent(QMouseEvent *event)
{
    navigator_->mousePressEvent(event); update();
}
void Scene::mouseMoveEvent(QMouseEvent *event)
{
    navigator_->mouseMoveEvent(event); update();
}
void Scene::mouseReleaseEvent(QMouseEvent *event)
{
    navigator_->mouseReleaseEvent(event); update();
}
void Scene::wheelEvent(QWheelEvent *event) {
    navigator_->wheelEvent(event); update();
}

// trigger a redraw of the widget through this method
void Scene::update()
{
    parent_->update();
}

void Scene::updateViewport(size_t width, size_t height)
{
    // make sure the OpenGL viewport projection is correct
    glViewport(0,0,GLint(width),GLint(height));

<<<<<<< HEAD
    // reset (and re-create) FBOs if image size changes
    fbo1_ = fbo2_ = nullptr;
}
=======
void Scene::refreshTexture(){
    if(planeNavigator_->speedarr!=QVector3D(0,0,0)){
        if(planeNavigator_->speedarr.x()==1)
            planespeed[0]+=0.00001f;
        if(planeNavigator_->speedarr.x()==-1 && planespeed[0]>0.0002f)
            planespeed[0]-=0.00001f;
        if(planeNavigator_->speedarr.y()==1)
        {
            planespeed[1]+=0.00004f;
            rotation_angle[0]+=0.00004f;
        }
        if(planeNavigator_->speedarr.y()==-1)
        {
            planespeed[1]-=0.00004f;
            rotation_angle[0]-=0.00004f;
        }
    }
    qDebug() << "X: " << planespeed.x() << "Y: " << planespeed.y() << "rotationangle: " << rotation_angle.x();
    moveGround(QVector2D(planespeed.x(),planespeed.y()));
}

>>>>>>> e17cfa1db82653f3a745c7c5e9e922b04f381038

