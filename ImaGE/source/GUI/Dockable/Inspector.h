#pragma once
#ifndef IMGUI_DISABLE
#include <GUI/GUIWindow.h>
#include <GUI/GUIManager.h>

#include <imgui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <GUI/Styles/FontAwesome6Icons.h>

#include "Core/EntityManager.h"
#include <Reflection/ObjectFactory.h>
#include <Events/EventCallback.h>
#include <Core/Component/Components.h>

namespace GUI {
  class Inspector : public GUIWindow {
  public:
    Inspector(std::string const& name);
    void Run() override;

    inline void SetIsComponentEdited(bool isComponentEdited) noexcept { mIsComponentEdited = isComponentEdited; }

  private:
    // Kindly put in alphabetical order, thank you!
    bool ColliderComponentWindow(ECS::Entity entity, std::string const& icon, bool highlight = false);
    bool LayerComponentWindow(ECS::Entity entity, std::string const& icon, bool highlight = false);
    bool MaterialComponentWindow(ECS::Entity entity, std::string const& icon, bool highlight = false);
    bool MeshComponentWindow(ECS::Entity entity, std::string const& icon, bool highlight = false);
    bool RigidBodyComponentWindow(ECS::Entity entity, std::string const& icon, bool highlight = false);
    bool ScriptComponentWindow(ECS::Entity entity, std::string const& icon, bool highlight = false);
    bool TagComponentWindow(ECS::Entity entity, std::string const& icon, bool highlight = false);
    bool TextComponentWindow(ECS::Entity entity, std::string const& icon, bool highlight = false);
    bool TransformComponentWindow(ECS::Entity entity, std::string const& icon, bool highlight = false);
    
    template<typename Component>
    bool WindowBegin(std::string const& windowName, std::string const& icon, bool highlight = false);

    void WindowEnd(bool isOpen);
    void DrawAddButton();

  private:
    std::map<std::string, bool> mComponentOpenStatusMap;
    GUI::Styler& mStyler;
    Reflection::ObjectFactory& mObjFactory;
    ECS::Entity mPreviousEntity;
    bool mIsComponentEdited, mFirstEdit, mEditingPrefab;
    bool mEntityChanged;

    static inline constexpr int INPUT_SIZE{ 200 };
    static inline constexpr float FIRST_COLUMN_LENGTH{ 130 };
    static inline constexpr ImU32 sComponentHighlightCol{ IM_COL32(253, 208, 23, 255) };

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
    bool DrawOptionsListButton(std::string const& windowName);
  };
#include "Inspector.tpp"
} // namespace GUI

#endif  // IMGUI_DISABLE