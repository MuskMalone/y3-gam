#pragma once
#include <GUI/GUIWindow.h>
#include <Graphics/Framebuffer.h>

namespace GUI
{

  class Viewport : public GUIWindow
  {
  public:
    Viewport(std::string const& name, Graphics::Temp::Framebuffer const& framebuffer);
    
    void Run() override;

  private:
    Graphics::Temp::Framebuffer const& mFramebuffer;

    void ReceivePayload();

  };
  
} // namespace GUI
