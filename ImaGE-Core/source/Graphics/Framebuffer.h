#pragma once
#include <glad/glad.h>
#include <string>

namespace Graphics
{

  class Framebuffer
  {
  public:
    Framebuffer(unsigned width, unsigned height);

    inline GLuint GetTextureID() const noexcept { return mTexture; }
    inline void Bind() const { glBindFramebuffer(GL_FRAMEBUFFER, mFBO); }
    inline void Unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
    void Resize(unsigned width, unsigned height);
    void Release();

  private:
    GLuint mFBO, mRBO;
    GLuint mTexture;
    unsigned mWidth, mHeight;
  };

} // namespace Graphics
