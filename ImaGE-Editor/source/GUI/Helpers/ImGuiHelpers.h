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
    True if the drop was received and false otherwise
  ************************************************************************/
  bool BeginDrapDropTargetWindow(const char* payloadName);

  /*!*********************************************************************
  \brief
    Helper function to create a table input for vec3
  \param propertyName
    The name of the property
  \param property
    Pointer to the property
  \param fieldWidth
    The width of the field
  \param disabled
    Whether it is disabled
  \param minVal
    The minimum value
  \param maxVal
    The maximum value
  \param step
    The increment for the input
  \return
    True if the field was modified and false otherwise
  ************************************************************************/
  bool TableInputFloat3(std::string propertyName, float* property, float fieldWidth, bool disabled, float minVal, float maxVal, float step);

  /*!*********************************************************************
  \brief
    Helper function to create a table input for dvec3
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
  bool TableInputDouble3(std::string propertyName, glm::dvec3& property, float fieldWidth, bool disabled);

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
  void InputDouble1(std::string propertyName, double& property, bool disabled);

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
  bool InputCheckBox(std::string propertyName, bool& property, bool disabled);

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
