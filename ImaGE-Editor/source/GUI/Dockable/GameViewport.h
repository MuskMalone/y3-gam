#pragma once
#include <GUI/GUIWindow.h>
#include <Graphics/RenderTarget.h>
#include <memory>

namespace GUI {

    class GameViewport : public GUIWindow {
    public:
        GameViewport(const char* name);

        void Run() override; // Declare the Run method without defining it here

    private:
        //Graphics::RenderTarget& mRenderTarget; // Reference to the render target for the game view

        void ProcessCameraInputs(); // Handle camera inputs if needed
    };

} // namespace GUI
