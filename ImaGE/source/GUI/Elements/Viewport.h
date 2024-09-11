#pragma once
#include <GUI/GUIWindow.h>
#include <Graphics/Framebuffer.h>

namespace GUI
{

  class Viewport : public GUIWindow
  {
  public:
    Viewport(std::string const& name, Graphics::Framebuffer const& framebuffer);
    
    void Run();

  private:
    Graphics::Framebuffer const& m_framebuffer;
  };
  
} // namespace GUI
