#pragma once
#include <glad/glad.h>
#include <string>

namespace Graphics
{

  class Framebuffer
  {
  public:
    Framebuffer(unsigned width, unsigned height);

    inline GLuint GetTextureID() const noexcept { return m_texture; }
    inline void Bind() const { glBindFramebuffer(GL_FRAMEBUFFER, m_fbo); }
    inline void Unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
    void Resize(unsigned width, unsigned height);
    void Release();

  private:
    GLuint m_fbo, m_rbo;
    GLuint m_texture;
  };

} // namespace Graphics
