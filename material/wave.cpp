#include "material/wave.h"

void WaveMaterial::apply(unsigned int light_pass)
{
    prog_->bind();

    // globals
    prog_->setUniformValue("time", time);

    // Wave
    prog_->setUniformValue("wave.depth", wave.depth);
    prog_->setUniformValue("wave.speed",  wave.speed);
    prog_->setUniformValue("wave.heigth",  wave.heigth);
}
