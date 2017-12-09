#include "material/vectors.h"

void VectorsMaterial::apply(unsigned int)
{
    prog_->bind();
    prog_->setUniformValue("vectorColor", vectorColor);
    prog_->setUniformValue("vectorToShow", vectorToShow);
    prog_->setUniformValue("scale", scale);

    // bump & displacement mapping
    prog_->setUniformValue("bump.scale", bump.scale);
    prog_->setUniformValue("bump.tex", 0); bump.tex->bind(0);
    prog_->setUniformValue("displacement.scale", displacement.scale);
    prog_->setUniformValue("displacement.tex", 1); displacement.tex->bind(1);

}

