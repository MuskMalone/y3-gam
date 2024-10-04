#pragma once
#include "pch.h"
#include <GUI/GUIWindow.h>
#include <Graphics/RenderTarget.h>
#include <memory>

namespace GUI
{

  class Viewport : public GUIWindow
  {
  public:
    Viewport(std::string const& name);
    
    void Run() override {}  // not in use
    void Render(Graphics::RenderTarget& renderTarget);

  private:
    void ProcessCameraInputs(Graphics::EditorCamera& cam);
    void ReceivePayload();

  };
  
} // namespace GUI
