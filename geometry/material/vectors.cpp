#include "material/vectors.h"

void VectorsMaterial::apply(unsigned int)
{
    prog_->bind();
    prog_->setUniformValue("vectorColor", vectorColor);
    prog_->setUniformValue("vectorToShow", vectorToShow);
    prog_->setUniformValue("scale", scale);

}

