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

    // mini controllers (lambda expressions) for UI functionality
    connect(ui->blackBgRadioButton, &QRadioButton::clicked, [this](bool)
    {
        scene().setBackgroundColor(QVector3D(0,0,0));
    });

    connect(ui->greyBgRadioButton, &QRadioButton::clicked, [this](bool)
    {
        scene().setBackgroundColor(QVector3D(0.4f,0.4f,0.4f));
    });

    connect(ui->whiteBgRadioButton, &QRadioButton::clicked, [this](bool)
    {
        scene().setBackgroundColor(QVector3D(1,1,1));
    });

    connect(ui->modelComboBox, &QComboBox::currentTextChanged, [this](QString value)
    {

        scene().setSceneNode(value);
    });

    connect(ui->materialComboBox, &QComboBox::currentTextChanged, [this](QString value)
    {

        scene().setMaterialNode(value,ui->modelComboBox->currentText());
    });

    connect(ui->light0Slider, &QSlider::valueChanged, [this](int value)
    {
        scene().setLightIntensity(0, float(value)/100); // slider goes from 0...1000
    });

    connect(ui->shadesSpinBox,static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int shades)
    {
       scene().setToonShades(shades);
    });
    connect(ui->shininesslSlider,&QSlider::valueChanged, [this](float shininess)
    {
       scene().setToonShininess(shininess);
    });
    connect(ui->dotEdit_r,&QLineEdit::textChanged, [this](QString value)
    {
       scene().setDotColor("r",value);
    });

    connect(ui->dotEdit_g,&QLineEdit::textChanged, [this](QString value)
    {
       scene().setDotColor("g",value);
    });

    connect(ui->dotEdit_b,&QLineEdit::textChanged, [this](QString value)
    {
       scene().setDotColor("b",value);
    });
    connect(ui->depthSpinBox,static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double depth)
    {
       scene().setWaveDepth(depth);
    });
    connect(ui->speedSpinBox,static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double speed)
    {
       scene().setWaveSpeed(speed);
    });
    connect(ui->heigthSpinBox,static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double heigth)
    {
       scene().setWaveHeigth(heigth);
    });

}

// called when the window is initially shown
void AppWindow::setDefaultUIValues() {

    // the following commands will trigger signals that will
    // result in scene methods being called. this can only
    // be done once the scene has actually been instantiated.

    // note: Qt will only trigger the actions if the value actually
    // changes. As a workaround, we always change the value to one
    // wrong value, and then the desired value :-(

    // scene rendering parameters
    ui->blackBgRadioButton->setChecked(true);
    ui->greyBgRadioButton->setChecked(true);
    ui->light0Slider->setValue(0);
    ui->light0Slider->setValue(80);
    ui->modelComboBox->setCurrentText("Cube");
    ui->modelComboBox->setCurrentText("Duck");
    ui->shadesSpinBox->setValue(4);
    ui->shadesSpinBox->setValue(3);
    ui->shininesslSlider->setValue(70);
    ui->shininesslSlider->setValue(80);
    ui->depthSpinBox->setValue(0.2);
    ui->depthSpinBox->setValue(0.1);
    ui->speedSpinBox->setValue(2.0f);
    ui->speedSpinBox->setValue(1.0f);
    ui->heigthSpinBox->setValue(2.0f);
    ui->heigthSpinBox->setValue(0.07f);

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
