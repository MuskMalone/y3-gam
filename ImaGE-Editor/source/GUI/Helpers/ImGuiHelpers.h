/*!*********************************************************************
\file   ImGuiHelpers.h
\date   5-October-2024
\brief  Contains the definition of helper functions for ImGui elements.
        Also contains convenience operator overloads for ImGui vectors.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#ifndef IMGUI_DISABLE
#include <ImGui/imgui_internal.h>

namespace ECS { class Entity; }

namespace ImGuiHelpers
{
  /*!*********************************************************************
  \brief
    Creates a DragDropTarget on the current window
    Example usage:
    if (BeginDrapDropTargetWindow("drop"))
    {
      std::cout << "Drop received\n";
      ImGui::EndDragDropTarget();
    }
  \param payloadName
    The name of the payload
  \return
    The ImGui payload and nullptr otherwise
  ************************************************************************/
  ImGuiPayload const* BeginDrapDropTargetWindow(const char* payloadName);

  // should be called within a ImGui::BeginDragDropTarget block
  // if (ImGui::BeginDragDropTarget()) {
  //   ImGuiHelpers::AssetDragDropBehavior(entity);
  // }
  // im using this for both inspector and hierarchy
  bool AssetDragDropBehavior(ECS::Entity entity);

  ImGuiID GetTreeNodeId(std::string const& nodeName, ImGuiID parentPath);

  /*!*********************************************************************
  \brief
    Helper function to create a table input for vec2
  ************************************************************************/
  bool TableInputFloat2(std::string const& propertyName, float* property, float fieldWidth, bool disabled, float minVal, float maxVal, float step, const char* fmt = "%.2f");

  /*!*********************************************************************
  \brief
    Helper function to create a table input for vec3
  ************************************************************************/
  bool TableInputFloat3(std::string const& propertyName, float* property, float fieldWidth, bool disabled, float minVal, float maxVal, float step);

  /*!*********************************************************************
  \brief
    Helper function to create a table input for dvec3
  ************************************************************************/
  bool TableInputDouble3(std::string const& propertyName, glm::dvec3& property, float fieldWidth, bool disabled, double minVal, double maxVal, float step);

  /*!*********************************************************************
  \brief
    Helper function to create an input for dvec3
  \param propertyName
    The name of the property
  \param property
    Reference to the dvec3
  \param fieldWidth
    The width of the field
  \param disabled
    Whether it is disabled
  \return
    True if the field was modified and false otherwise
  ************************************************************************/
	bool InputDouble3(std::string propertyName, glm::dvec3& property, float fieldWidth, bool disabled);

  /*!*********************************************************************
  \brief
    Helper function to create an input for a double
  \param propertyName
    The name of the property
  \param property
    Pointer to the property
  \param disabled
    Whether it is disabled
  ************************************************************************/
  void InputDouble1(std::string const& propertyName, double& property, bool disabled);

  /*!*********************************************************************
  \brief
    Helper function to create a check box
  \param propertyName
    The name of the property
  \param property
    Pointer to the bool
  \param disabled
    Whether it is disabled
  ************************************************************************/
  bool InputCheckBox(std::string const& propertyName, bool& property, bool disabled);

  void WrapMousePos(int axises_mask);
  void WrapMousePosEx(int axises_mask, const ImRect& wrap_rect);

  bool IsUnderCanvasEntity(ECS::Entity entity);
} // namespace ImGuiHelpers

// operator overloads
ImVec2 operator+(ImVec2 const& lhs, ImVec2 const& rhs);
ImVec2 operator+(ImVec2 const& lhs, float rhs);
ImVec2 operator-(ImVec2 const& lhs, ImVec2 const& rhs);
ImVec2 operator-(ImVec2 const& lhs, float rhs);
ImVec2 operator*(ImVec2 const& lhs, float rhs);
ImVec2 operator*(float lhs, ImVec2 const& rhs);
ImVec2 operator/(ImVec2 const& lhs, float rhs);
ImVec4 operator+(ImVec4 const& lhs, ImVec4 const& rhs);
ImVec4 operator+(ImVec4 const& lhs, float rhs);
ImVec4 operator-(ImVec4 const& lhs, ImVec4 const& rhs);
ImVec4 operator-(ImVec4 const& lhs, float rhs);

#endif  // IMGUI_DISABLE
