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
    static inline constexpr float SAME_LINE_SPACING{ 350.f };
    static inline constexpr float INPUT_SIZE{ 150 };

  private:
    static int sSelectedLayer;

    void LayerNameNode();
    void VisibilityToggleNode();
    void CollisionMatrixNode();

    std::weak_ptr<Systems::LayerSystem> mLayerSystem; // using weak_ptr so SystemManager still retains full ownership
  };

} // namespace GUI
