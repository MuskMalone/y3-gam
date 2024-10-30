#pragma once
#include <GUI/GUIWindow.h>
#include <Graphics/RenderTarget.h>
#include <memory>

namespace GUI {

    class GameViewport : public GUIWindow {
    public:
        GameViewport(const char* name);

        void Run() override;
        void Render(std::shared_ptr<Graphics::Texture> const& tex);

    //private:
        //Graphics::RenderTarget& mRenderTarget; // Reference to the render target for the game view

        //void ProcessCameraInputs(); // Handle camera inputs if needed
    };

} // namespace GUI
