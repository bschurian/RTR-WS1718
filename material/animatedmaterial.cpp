#include "material/animatedmaterial.h"
#include <assert.h>

void AnimatedMaterial::apply(unsigned int light_pass)
{
    // globals
    prog_->setUniformValue("time", time);
}
