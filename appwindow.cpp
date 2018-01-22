/*
 *   Implementation of class AppWindow for the RTR demo App
 *   Author: Hartmut Schirmacher
 *
 */

#include <assert.h>

#include <QApplication>
#include <QKeyEvent>
#include <QDesktopWidget>
#include <QDialog>
#include <QVBoxLayout>

#include "appwindow.h"
#include "ui_appwindow.h"
#include "scene.h"

AppWindow::AppWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppWindow)
{
    // connects what comes out of the Qt designer to this class
    ui->setupUi(this);

    // window title (app name is set in main.cpp)
    this->setWindowTitle(QCoreApplication::applicationName());

    // have we saved the size of the app window, then restore that size
    bool restored = restoreGeometry(settings_.value("window_geometry").toByteArray());

    // else set sensible initial size
    if(!restored) {

        // available part of the desktop (minus dock/taskbar etc.)
        QDesktopWidget desktop;
        QRect avail = desktop.availableGeometry(desktop.primaryScreen());

        // default size = 70% of available desktop
        setGeometry(50,50,avail.width()*0.7, avail.height()*0.7);
    }

    // quit button -> close app window
    connect(ui->quitButton, &QPushButton::clicked, [this]{ close(); });

    // when last window closes, quit
    connect(qApp, &QGuiApplication::lastWindowClosed, []{ qApp->quit(); });

    // pass 1
    connect(ui->blackBgRadioButton, &QRadioButton::clicked,
            [this](bool) { scene().setBackgroundColor(QVector3D(0,0,0)); } );
    connect(ui->greyBgRadioButton, &QRadioButton::clicked,
            [this](bool) { scene().setBackgroundColor(QVector3D(0.4f,0.4f,0.4f)); } );
    connect(ui->whiteBgRadioButton, &QRadioButton::clicked,
            [this](bool) { scene().setBackgroundColor(QVector3D(1,1,1)); } );
    connect(ui->light0Slider, &QSlider::valueChanged,
            [this](int value) { scene().setLightIntensity(0, float(value)/100.0); } );
    connect(ui->modelComboBox, &QComboBox::currentTextChanged,
            [this](QString value) { scene().setSceneNode(value); } );

    connect(ui->ambientSlider, &QSlider::valueChanged,
            [this](int value) { scene().setAmbientScale(float(value)/20.0); } );
    connect(ui->diffuseSlider, &QSlider::valueChanged,
            [this](int value) { scene().setDiffuseScale(float(value)/20.0); } );
    connect(ui->specularSlider, &QSlider::valueChanged,
            [this](int value) { scene().setSpecularScale(float(value)/20.0); } );
    connect(ui->shininessSlider, &QSlider::valueChanged,
            [this](int value) { scene().setShininess(float(value)); } );
    connect(ui->envMapCheckbox, &QCheckBox::toggled,
            [this](bool value) { scene().toggleEnvMap(value); } );
    connect(ui->mirrorSlider, &QSlider::valueChanged,
            [this](int value) { scene().setMirrorScale(float(value)/20.0); } );
    connect(ui->skyBoxToggle, &QCheckBox::toggled,
            [this](bool value) { scene().toggleSkyBox(value); } );
    connect(ui->skylightSlider, &QSlider::valueChanged,
            [this](int value) { scene().setSkylightScale(float(value)/20.0); } );

    // post processing parameters -------------------------------
    connect(ui->postFilterComboBox, &QComboBox::currentTextChanged,
            [this](QString value) {
        if(value == "Blur") {
            hideBufferContents();
            ui->post_kernel_size->setEnabled(true);
            scene().useSimpleBlur();
        } else if(value == "2-Pass 9x9 Gauss") {
            scene().useTwoPassGauss();
            hideBufferContents();
            ui->post_kernel_size->setDisabled(true);
        }
    } );
    connect(ui->splitScreenCheckbox, &QCheckBox::toggled,
            [this](bool value) { scene().toggleSplitDisplay(value); } );
    connect(ui->showFBOtoggle, &QCheckBox::toggled,
            [this](bool value) {
        scene().toggleFBODisplay(value);
        if(!value)
            hideBufferContents();
    } );
    connect(ui->jitterCheckbox, &QCheckBox::toggled,[this](bool value){ scene().toggleJittering(value); });

    // strange cast here: see https://stackoverflow.com/questions/16794695/connecting-overloaded-signals-and-slots-in-qt-5
    connect(ui->post_kernel_size, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [this](int value) { scene().setPostFilterKernelSize(value); } );

}

void AppWindow::displayBufferContents(unsigned int id, QString label, const QImage &img)
{

    qDebug() << "showing FBO, id = " << id;
    auto pixmap = QPixmap::fromImage(img);

    // do we have a layout yet, else create one
    if(ui->fbo_area->layout() == nullptr)
        ui->fbo_area->setLayout(new QVBoxLayout());

    // if button for this ID already exists, update it; else create new
    if(buttons_.find(id) == buttons_.end())
        buttons_[id] = new ImageDisplayButton(this,pixmap,label);
    else
        buttons_[id]->update(pixmap, label);

    // do we still need to add the button to the UI / layout
    QVBoxLayout *layout = (QVBoxLayout*) ui->fbo_area->layout();
    assert(int(id) <= layout->count());
    if(layout->count()==int(id))
        layout->addWidget(buttons_[id]);

}

void AppWindow::hideBufferContents()
{
    for(auto w : buttons_)
        delete w.second;
    buttons_.clear();
    update();
}

// called when the window is initially shown
void AppWindow::setDefaultUIValues() {

    bool b = connect(&(this->scene()), &Scene::displayBufferContents,
                     this, &AppWindow::displayBufferContents);
    assert(b);

    // the following commands will trigger signals that will
    // result in scene methods being called. this can only
    // be done after the scene has actually been instantiated.

    // post processing parameters
    // note: Qt will only trigger the actions if the value actually
    // changes. As a workaround, we always change the value to one
    // wrong value, and then the desired value :-(

    // scene rendering parameters
    ui->light0Slider->setValue(35.0);
    ui->ambientSlider->setValue(20);
    ui->diffuseSlider->setValue(20);
    ui->specularSlider->setValue(10);
    ui->shininessSlider->setValue(20);
    ui->skylightSlider->setValue(20);
    ui->mirrorSlider->setValue(20);
    ui->envMapCheckbox->setChecked(false);
    ui->envMapCheckbox->setChecked(true);
    ui->skyBoxToggle->setChecked(true);
    ui->skyBoxToggle->setChecked(false);
    ui->blackBgRadioButton->setChecked(true);
    ui->greyBgRadioButton->setChecked(true);
    ui->modelComboBox->setCurrentText("Cube");
    ui->modelComboBox->setCurrentText("Duck");

}

void AppWindow::closeEvent(QCloseEvent *event)
{
    settings_.setValue("window_geometry", saveGeometry());
    QWidget::closeEvent(event); // let parent class do its job
}

void AppWindow::showEvent(QShowEvent *event)
{
    if(!wasInitialized) {
        setDefaultUIValues();
        wasInitialized = true;
    }
    QWidget::showEvent(event); // hand to parent
}

AppWindow::~AppWindow()
{
    delete ui;
}

Scene &AppWindow::scene()
{
    return ui->openGLWidget->scene();
}

void AppWindow::showUI()
{
    ui->ui_container->show();
    // default pixel margins
    ui->mainLayout->setContentsMargins(12,12,12,12);

}

void AppWindow::hideUI()
{
    ui->ui_container->hide();
    // zero pixel margins, for borderless OpenGL window
    ui->mainLayout->setContentsMargins(0,0,0,0);
}

void AppWindow::keyPressEvent(QKeyEvent *event)
{
    assert(event);

    switch(event->key()) {

    // key 'h': show/hide UI
    case Qt::Key_H:
        ui->ui_container->isHidden()? showUI() : hideUI();
        return;

    // key 'q': quit app
    case Qt::Key_Q:
        close();
        return;

    } // switch

    // pass on all other events to the scene
    scene().keyPressEvent(event);
}
