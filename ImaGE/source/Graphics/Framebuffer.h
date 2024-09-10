#pragma once
#include <glad/glad.h>
#include <string>

namespace Graphics
{

  class Framebuffer
  {
  public:
    Framebuffer(std::string name, unsigned width, unsigned height);

    inline GLuint GetTextureID() const noexcept { return m_texture; }
    inline std::string GetName() const noexcept { return m_name; }
    inline void Bind() const { glBindFramebuffer(GL_FRAMEBUFFER, m_fbo); }
    inline void Unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
    void Resize(unsigned width, unsigned height);
    void Release();

  private:
    std::string const m_name;
    GLuint m_fbo, m_rbo;
    GLuint m_texture;
  };

} // namespace Graphics
