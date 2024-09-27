#pragma once
#include "pch.h"
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
    void Update(std::shared_ptr<Graphics::Framebuffer> const& framebuffer);

  private:
    void ReceivePayload();

  };
  
} // namespace GUI
