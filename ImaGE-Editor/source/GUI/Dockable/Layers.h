#pragma once
#include <GUI/GUIWindow.h>

namespace GUI {
  class Layers : public GUIWindow {
  public:
    Layers(const char* name);
    void Run() override;

  private:
    static inline constexpr float SAME_LINE_SPACING{ 125.f };
    static inline constexpr float INPUT_SIZE{ 200 };

    void LayerNameNode();
    void VisibilityToggleNode();
    void CollisionMatrixNode();
  };

} // namespace GUI
