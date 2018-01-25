# PROJECT FILE FOR RTRAPP

# We want the most current C++ standard.
# i.e. for std::make_unique
CONFIG += c++14

# QT MODULES TO BE USED
QT           += gui widgets

# HEADERS TO BE PROCESSED BY QT
HEADERS      += \
    camera.h \
    appwindow.h \
    material/material.h \
    node.h \
    rtrglwidget.h \
    scene.h \
    geometry/cube.h \
    mesh/bbox.h \
    mesh/objloader.h \
    mesh/indexbuffer.h \
    mesh/mesh.h \
    mesh/vertexbuffer.h \
    mesh/geometrybuffers.h \
    navigator/nodenavigator.h \ 
    material/phong.h \
    navigator/position_navigator.h \
    navigator/rotate_y.h \
    imagedisplaybutton.h \
    imagedisplaydialog.h \
    cubemap.h \
    material/postmaterial.h \
    material/postmaterial.h \
    material/texphong.h \
    geometry/parametric.h \
    navigator/modeltrackball.h \
    navigator/rotate_y.h \
    material/skyboxmaterial.h \
    skybox.h

# C++ SOURCE FILES TO BE COMPILED AND LINKED TOGETHER
SOURCES      += \
    main.cpp \
    appwindow.cpp \
    camera.cpp \
    material/material.cpp \
    node.cpp \
    scene.cpp \
    geometry/cube.cpp \
    mesh/bbox.cpp \
    mesh/geometrybuffers.cpp \
    mesh/objloader.cpp \
    mesh/indexbuffer.cpp \
    mesh/mesh.cpp \
    rtrglwidget.cpp \
    navigator/nodenavigator.cpp \ 
    material/phong.cpp \
    navigator/position_navigator.cpp \
    navigator/rotate_y.cpp \
    imagedisplaybutton.cpp \
    imagedisplaydialog.cpp \
    cubemap.cpp \
    material/postmaterial.cpp \
    material/texphong.cpp \
    geometry/parametric.cpp \
    navigator/modeltrackball.cpp \
    material/skyboxmaterial.cpp \
    skybox.cpp

# RESOURCE FILES TO BE PROCESSED BY QT
RESOURCES    += \
    models_and_textures.qrc \
    shaders.qrc

# UI DEFINITION FILES TO BE PROCESSED BY QT (FROM DESIGNER APP)
FORMS        += \
    appwindow.ui \ 
    imagedisplaybutton.ui \
    imagedisplaydialog.ui

# additional libs needed on Windows
win32: LIBS += -lopengl32

# hack to work around a bug in QtCreator, not always running qmake when it should
# see https://bugreports.qt.io/browse/QTBUG-13334
shaders.depends += \
    $$PWD/assets/shaders/phong.frag $$PWD/assets/shaders/phong.vert 
shaders.commands =
QMAKE_EXTRA_TARGETS += shaders
