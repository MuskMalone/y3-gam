#pragma once
#ifndef IMGUI_DISABLE
#include "GUI/GUIWindow.h"
#include "GUI/GUIManager.h"

#include <imgui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <GUI/Styles/FontAwesome6Icons.h>

#include "Core/Entity.h"
#include "Core/EntityManager.h"
#include <Reflection/ObjectFactory.h>

namespace GUI {
  class Inspector : public GUIWindow {
  public:
    Inspector(std::string const& name);
    void Run() override;

    static bool const GetIsComponentEdited();
    static void SetIsComponentEdited(bool isComponentEdited);

  private:
    // Kindly put in alphabetical order, thank you!
    void ColliderComponentWindow(ECS::Entity entity, std::string const& icon);
    void LayerComponentWindow(ECS::Entity entity, std::string const& icon);
    void MaterialComponentWindow(ECS::Entity entity, std::string const& icon);
    void MeshComponentWindow(ECS::Entity entity, std::string const& icon);
    void RigidBodyComponentWindow(ECS::Entity entity, std::string const& icon);
    void ScriptComponentWindow(ECS::Entity entity, std::string const& icon);
    void TagComponentWindow(ECS::Entity entity, std::string const& icon);
    void TextComponentWindow(ECS::Entity entity, std::string const& icon);
    void TransformComponentWindow(ECS::Entity entity, std::string const& icon);
    
    template<typename Component>
    bool WindowBegin(std::string windowName, std::string const& icon);

    void WindowEnd(bool isOpen);
    void DrawAddButton();

  private:
    std::map<std::string, bool> mComponentOpenStatusMap;
    Reflection::ObjectFactory& mObjFactory;
    ECS::Entity mPreviousEntity;
    bool mEntityChanged;

    static bool sIsComponentEdited;
  };

  template<typename Component>
  bool Inspector::WindowBegin(std::string windowName, std::string const& icon) {
    ImGui::Separator();

    if (mEntityChanged) {
      bool& openMapStatus = mComponentOpenStatusMap[windowName];
      ImGui::SetNextItemOpen(openMapStatus, ImGuiCond_Always);
    }

    std::string display{ icon + "   " + windowName };
    bool isOpen{ ImGui::TreeNode(display.c_str())};

    if (isOpen) {
      ImGui::PushFont(GUIManager::GetCustomFonts()[(int)GUIManager::MontserratLight]);
      // Must close component window if a component was removed
      if (!DrawOptionsListButton<Component>(windowName)) {
        WindowEnd(true);
        return false;
      }
    }

    mComponentOpenStatusMap[windowName] = isOpen;
    return isOpen;
  }

} // namespace GUI

#endif  // IMGUI_DISABLE