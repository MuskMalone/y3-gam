#pragma once
#include <utility>
#include <GUI/GUIWindow.h>
#include <Core/Systems/LayerSystem/LayerSystem.h>

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

    std::weak_ptr<Systems::LayerSystem> mLayerSystem; // using weak_ptr so SystemManager still retains full ownership
  };

} // namespace GUI
