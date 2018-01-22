#include "postmaterial.h"
#include "QOpenGLFunctions"

void PostMaterial::apply(unsigned int)
{
    prog_->bind();

    // bind texture manually using its OpenGL ID
    QOpenGLFunctions gl(QOpenGLContext::currentContext());
    gl.glActiveTexture(GL_TEXTURE0 + tex_unit);
    gl.glBindTexture(GL_TEXTURE_2D, post_texture_id);

    prog_->setUniformValue("post_tex", tex_unit);
    prog_->setUniformValue("image_width", (GLint)image_size.width());
    prog_->setUniformValue("image_height", (GLint)image_size.height());
    prog_->setUniformValue("kernel_width", (GLint)kernel_size.width());
    prog_->setUniformValue("kernel_height", (GLint)kernel_size.height());
    prog_->setUniformValue("use_jitter", use_jitter);
}
