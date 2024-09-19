#pragma once
#ifndef IMGUI_DISABLE
#include "GUI/GUIWindow.h"
#include "GUI/GUIManager.h"

#include "Core/Entity.h"
#include "Core/EntityManager.h"
#include <Reflection/ObjectFactory.h>

namespace GUI {
  class Inspector : public GUIWindow {
  public:
    Inspector(std::string const& name);
    void Run() override;

  private:
    void TagComponentWindow(ECS::Entity entity);
    void LayerComponentWindow(ECS::Entity entity);
    void TransformComponentWindow(ECS::Entity entity);
    void RigidBodyComponentWindow(ECS::Entity entity);
    void ColliderComponentWindow(ECS::Entity entity);
    void ShowAddComponentPopup();
    bool WindowBegin(std::string windowName);
    void WindowEnd(bool isOpen);

  private:
    std::map<std::string, bool> mComponentOpenStatusMap;
    Reflection::ObjectFactory& mObjFactory;
    ECS::Entity mPreviousEntity;
    bool mFirstOpen;
    bool mEntityChanged;
  };
} // namespace GUI

#endif  // IMGUI_DISABLE
