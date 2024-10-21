/*!*********************************************************************
\file   Inspector.h
\author
\date   5-October-2024
\brief  Class encapsulating functions to run the inspector / property
        window of the editor. Displays and allows modification of
        components for the currently selected entity.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include "GUI/GUIWindow.h"
#include "GUI/GUIManager.h"

#include <imgui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <GUI/Styles/FontAwesome6Icons.h>

#include "Core/EntityManager.h"
#include <Reflection/ObjectFactory.h>
#include <Events/EventCallback.h>
#include <Physics/PhysicsSystem.h>

namespace GUI {
  class Inspector : public GUIWindow {
  public:
    Inspector(const char* name);
    void Run() override;

  private:

    void DrawAddComponentButton(rttr::type const& compType);

    /*!*********************************************************************
    \brief
      Draws the add component button of the window
    ************************************************************************/
    void DrawAddButton();

  private:
    std::map<std::string, bool> mComponentOpenStatusMap;
    GUI::Styler& mStyler;
    Reflection::ObjectFactory& mObjFactory;
    ECS::Entity mPreviousEntity;
    bool mEditingPrefab;
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

    /*!*********************************************************************
    \brief
      Displays the removed components of a prefab instance
    \param type
      The rttr::type of the component
    ************************************************************************/
    void DisplayRemovedComponent(rttr::type const& type);

    /*!*********************************************************************
    \brief
      Draws the add component option for a particular component
    \param name
      The component in the form of an rttr::variant
    \param compType
      The type of the component
    \param icon
      The icon to display for the component
    ************************************************************************/
    bool WindowBegin(rttr::variant& component, rttr::type const& compType, bool highlight);

    /*!*********************************************************************
    \brief
      Helper function to end the property window of a component. Should
      be called with WindowBegin.
    \param isOpen
      Whether the window is open
    ************************************************************************/
    void WindowEnd(bool const isOpen);

    void DisplayProperties(rttr::instance compInst);

    bool Vec3Input(rttr::property const& prop, glm::vec3& data);
  };
} // namespace GUI