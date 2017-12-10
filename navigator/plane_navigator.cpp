#include "navigator/plane_navigator.h"

#include <QtMath>
#include <iostream>

using namespace std;

PlaneNavigator::PlaneNavigator(std::shared_ptr<Node> node,
                                     std::shared_ptr<Node> world,
                                     std::shared_ptr<Node> camera)
    : NodeNavigator(node,world,camera)
{}

void PlaneNavigator::keyPressEvent(QKeyEvent *event)
{
    // depending on key press, speed up or down, speedsrr=Vec3 -> z could be added in the future
    switch(event->key()) {
        case Qt::Key_Up:
            speedarr[0] = 1; //schneller
            break;
        case Qt::Key_Down:
            speedarr[0] = -1; //langsamer
            break;
        case Qt::Key_Left:
            speedarr[1] = -1; //nach links
            break;
        case Qt::Key_Right:
            speedarr[1] = 1; //nach rechts
            break;
        default:
            return;
    }
}

void PlaneNavigator::keyReleaseEvent(QKeyEvent *event)
{
    qDebug() << "RELEASE_plane";
    switch(event->key()) { //Set to no more change when released
        case Qt::Key_Up:
            speedarr[0] = 0;
            break;
        case Qt::Key_Down:
            speedarr[0] = 0;
            break;
        case Qt::Key_Left:
            speedarr[1] = 0;
            break;
        case Qt::Key_Right:
            speedarr[1] = 0;
            break;
        default:
            return;
    }
}


