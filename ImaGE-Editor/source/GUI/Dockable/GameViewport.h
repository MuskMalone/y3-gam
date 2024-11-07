#pragma once
#include <GUI/GUIWindow.h>
#include <Graphics/RenderTarget.h>
#include "Graphics/Texture.h"
#include <memory>

namespace GUI {

    class GameViewport : public GUIWindow {
    public:
        GameViewport(const char* name);

        void Run() override;
        void Render(std::shared_ptr<Graphics::Texture> const& tex);
    };

} // namespace GUI
