#include "material/wireframe.h"

void WireframeMaterial::apply(unsigned int)
{
    prog_->bind();
    prog_->setUniformValue("wireframeColor", wireframeColor);

    qDebug() << "applying wireframe material";
}

