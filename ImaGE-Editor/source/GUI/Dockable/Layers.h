#pragma once
#include <utility>
#include <GUI/GUIWindow.h>
#include "Events/EventCallback.h"

namespace GUI {

  class LayerWindow : public GUIWindow {
  public:
    LayerWindow(const char* name);
    void Run() override;

  private:
    static inline constexpr float SAME_LINE_SPACING{ 350.f };
    static inline constexpr float INPUT_SIZE{ 150 };

  private:
    static int sSelectedLayer;
    bool mIsActive;

    void LayerNameNode();
    void VisibilityToggleNode();
    void CollisionMatrixNode();

    EVENT_CALLBACK_DECL(OnPrefabEditor);
    EVENT_CALLBACK_DECL(OnSceneStop);
  };

} // namespace GUI
