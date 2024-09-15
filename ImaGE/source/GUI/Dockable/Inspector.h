#pragma once
#include "GUI/GUIWindow.h"
#include "GUI/GUIManager.h"

#include "Core/Entity.h"
#include "Core/EntityManager.h"

namespace GUI {
  class Inspector : public GUIWindow {
  public:
    Inspector(std::string const& name);
    void Run() override;

  private:
    void TagComponentWindow(ECS::Entity entity);
    void LayerComponentWindow(ECS::Entity entity);
    void TransformComponentWindow(ECS::Entity entity);
  };
} // namespace GUI
