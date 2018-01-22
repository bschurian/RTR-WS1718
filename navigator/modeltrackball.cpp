#include "navigator/modeltrackball.h"

#include <QtMath>
#include <iostream>

using namespace std;

void ModelTrackball::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        last_pos_ = QVector2D(event->x(), event->y());
}

void ModelTrackball::mouseMoveEvent(QMouseEvent *event)
{
    // how much did the mouse move?
    QVector2D now = QVector2D(event->x(), event->y());
    QVector2D delta = now - last_pos_;
    last_pos_= now;

    // apply to panning or rotation
    if(event->modifiers() & Qt::ShiftModifier)
        pan(QVector3D(delta * QVector2D(1,-1), 0));
    else
        rotate(delta*rotation_sensitivity);
}

void ModelTrackball::mouseReleaseEvent(QMouseEvent *)
{
    // qDebug() << "pos = " << node_->transformation.column(3);
}

void ModelTrackball::rotate(QVector2D xy)
{
    // transformation from camera coords to model coords
    auto camToWorld = world_->toParentTransform(camera_);
    auto nodeToWorld = world_->toParentTransform(node_);
    auto camToNode = nodeToWorld.inverted()*camToWorld;

    // main axes converted to model space
    auto xAxis = camToNode*QVector4D(1,0,0,0);
    auto yAxis = camToNode*QVector4D(0,1,0,0);

    // rotate
    node_->transformation.rotate(qDegreesToRadians(xy[1]), xAxis.toVector3D());
    node_->transformation.rotate(qDegreesToRadians(xy[0]), yAxis.toVector3D());

}

void ModelTrackball::pan(QVector4D translation_ec)
{
    // transformation from camera coords to model coords
    auto camToWorld = world_->toParentTransform(camera_);
    auto nodeToWorld = world_->toParentTransform(node_);
    auto camToNode = nodeToWorld.inverted()*camToWorld;

    QVector4D translation_mc = camToNode * translation_ec * pan_sensitivity;
    node_->transformation.translate(translation_mc.toVector3D());

}
