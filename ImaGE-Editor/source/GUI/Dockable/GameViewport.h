#pragma once
#include <GUI/GUIWindow.h>
#include "Graphics/Texture.h"
#include <memory>
#include <Events/EventCallback.h>

namespace GUI {

    class GameViewport : public GUIWindow {
    public:
        GameViewport(const char* name);

        void Run() override {}  // empty
        void Render(std::shared_ptr<Graphics::Texture> const& tex);

    private:
      /*!*********************************************************************
      \brief
        Switches the focus to this window when the scene starts
      ************************************************************************/
      EVENT_CALLBACK_DECL(OnScenePlay);

      bool mFocusWindow;
    };

} // namespace GUI
