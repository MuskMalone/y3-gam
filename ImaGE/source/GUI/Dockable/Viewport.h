#pragma once
#ifndef IMGUI_DISABLE
#include <GUI/GUIWindow.h>
#include <Graphics/Framebuffer.h>
#include <memory>

namespace GUI
{

  class Viewport : public GUIWindow
  {
  public:
    Viewport(std::string const& name);
    
    void Run() override {}  // not in use
    void Update(std::shared_ptr<Graphics::Temp::Framebuffer> const& framebuffer);

  private:
    void ReceivePayload();

  };
  
} // namespace GUI
#endif  // IMGUI_DISABLE
