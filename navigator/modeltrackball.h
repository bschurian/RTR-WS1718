#pragma once

#include "navigator/nodenavigator.h"

/*
 *  Simple navigator class to rotate a node around its Y axis using cursor keys
 */

class ModelTrackball : public NodeNavigator {
public:

    ModelTrackball(std::shared_ptr<Node> node,
                   std::shared_ptr<Node> world,
                   std::shared_ptr<Node> camera)
        : NodeNavigator(node,world,camera)
    {}

    // use mouse to rotate model around its center
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *) override;

    float pan_sensitivity = 0.003f;
    float rotation_sensitivity = 15.0f;

private:

    QVector2D last_pos_;

    void rotate(QVector2D xy);
    void pan(QVector4D translation_ec);
};

