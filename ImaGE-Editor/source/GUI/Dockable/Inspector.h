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
#include <Core/Components/Components.h>
#include <Physics/PhysicsSystem.h>

namespace GUI {
  class Inspector : public GUIWindow {
  public:
    Inspector(const char* name);
    void Run() override;

    inline void SetIsComponentEdited(bool isComponentEdited) noexcept { mIsComponentEdited = isComponentEdited; }

  private:
    /*!*********************************************************************
    \brief
      Wrapper functions to display each component's window depending on
      its properties
    \param entity
      The entity being inspected
    \param icon
      The icon to display for the component
    \param highlight
      Whether to highlight the component
    \return
      True if any part of the component was modified and false otherwise
    ************************************************************************/
    // Kindly put in alphabetical order, thank you!
    bool BoxColliderComponentWindow(ECS::Entity entity, bool highlight = false);
    bool CapsuleColliderComponentWindow(ECS::Entity entity, bool highlight = false);
    bool LayerComponentWindow(ECS::Entity entity, bool highlight = false);
    bool LightComponentWindow(ECS::Entity entity, bool highlight = false);
    bool MaterialComponentWindow(ECS::Entity entity, bool highlight = false);
    bool MeshComponentWindow(ECS::Entity entity, bool highlight = false);
    bool RigidBodyComponentWindow(ECS::Entity entity, bool highlight = false);
    bool ScriptComponentWindow(ECS::Entity entity, bool highlight = false);
    bool SphereColliderComponentWindow(ECS::Entity entity, bool highlight = false);
    bool TagComponentWindow(ECS::Entity entity, bool highlight = false);
    bool TextComponentWindow(ECS::Entity entity, bool highlight = false);
    bool TransformComponentWindow(ECS::Entity entity, bool highlight = false);
    
    /*!*********************************************************************
    \brief
      Helper function to start the property window of a component. Should
      be called with WindowEnd.
    \param windowName
      The name of the property window
    \param highlight
      Whether to highlight the component
    \return
      True if any part of the component was modified and false otherwise
    ************************************************************************/
    template<typename Component>
    bool WindowBegin(std::string const& windowName, bool highlight = false);

    /*!*********************************************************************
    \brief
      Helper function to end the property window of a component. Should
      be called with WindowBegin.
    \param isOpen
      Whether the window is open
    ************************************************************************/
    void WindowEnd(bool isOpen);

    /*!*********************************************************************
    \brief
      Draws the add component button of the window
    ************************************************************************/
    void DrawAddButton();

  private:
    GUI::Styler& mStyler;
    std::unordered_map<std::type_index, const char*> const mComponentIcons;
    std::unordered_map<std::string, bool> mComponentOpenStatusMap;
    Reflection::ObjectFactory& mObjFactory;
    ECS::Entity mPreviousEntity;
    bool mIsComponentEdited, mFirstEdit, mEditingPrefab;
    bool mEntityChanged;

    static inline constexpr int INPUT_SIZE{ 210 };
    static inline constexpr float FIRST_COLUMN_LENGTH{ 175 };
    static inline constexpr ImU32 sComponentHighlightCol{ IM_COL32(253, 208, 23, 255) };
    static inline constexpr float ITEM_SPACING{ 0 };
    static inline constexpr float CELL_PADDING{ 1 };

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
      The name of the component
    \param icon
      The icon to display for the component
    ************************************************************************/
    template<typename Component>
    bool DrawAddComponentButton(std::string const& name);

    /*!*********************************************************************
    \brief
      Draws the options for a particular component
    \param name
      The name of the component
    \return
      True if the main window should be opened and false otherwise
    ************************************************************************/
    template<typename Component>
    bool DrawOptionButton(std::string const& name);

    /*!*********************************************************************
    \brief
      Draws the options button for a particular component
    \param name
      The name of the property window
    \return
      True if the main window should be opened and false otherwise
    ************************************************************************/
    template<typename Component>
    bool DrawOptionsListButton(std::string const& windowName);

    /*!*********************************************************************
    \brief
      Helper function to set up the column for the next row
    \param labelName
      The name of the property
    ************************************************************************/
    void NextRowTable(const char* labelName) const;
  };
#include "Inspector.tpp"
} // namespace GUI