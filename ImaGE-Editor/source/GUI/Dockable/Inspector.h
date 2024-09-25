#pragma once
#include "pch.h"

#include "GUI/GUIWindow.h"
#include "GUI/GUIManager.h"

#include <imgui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <GUI/Styles/FontAwesome6Icons.h>

#include "Core/Entity.h"
#include "Core/EntityManager.h"
#include <Reflection/ObjectFactory.h>
#include <Events/EventCallback.h>

namespace GUI {
  class Inspector : public GUIWindow {
  public:
    Inspector(std::string const& name);
    void Run() override;

    inline void SetIsComponentEdited(bool isComponentEdited) noexcept { mIsComponentEdited = isComponentEdited; }

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
    bool mIsComponentEdited, mFirstEdit;
    bool mEntityChanged;

    static inline constexpr int INPUT_SIZE{ 200 };
    static inline constexpr float FIRST_COLUMN_LENGTH{ 130 };

    /*!*********************************************************************
    \brief
      This function handles the corresponding events the Inspector
      subscribed to.

      SAVE_SCENE
        - Resets the mIsComponentEdited flag when the scene is saved
      SCENE_STATE_CHANGE
        - Resets the mIsComponentEdited flag when another scene is loaded
    \param event
      The event to be handled
    ************************************************************************/
    EVENT_CALLBACK_DECL(HandleEvent);

    template<typename Component>
    void DrawAddComponentButton(std::string const& name, std::string const& icon);

    template<typename Component>
    bool DrawOptionButton(std::string const& name);

    template<typename Component>
    bool DrawOptionsListButton(std::string windowName);
  };
#include "Inspector.tpp"
} // namespace GUI