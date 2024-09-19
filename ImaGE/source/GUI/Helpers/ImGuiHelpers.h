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

} // namespace ImGuiHelpers

// operator overloads
ImVec2 operator+(ImVec2 const& lhs, ImVec2 const& rhs);
ImVec2 operator+(ImVec2 const& lhs, float rhs);
ImVec2 operator-(ImVec2 const& lhs, ImVec2 const& rhs);
ImVec2 operator-(ImVec2 const& lhs, float rhs);
ImVec4 operator+(ImVec4 const& lhs, ImVec4 const& rhs);
ImVec4 operator+(ImVec4 const& lhs, float rhs);
ImVec4 operator-(ImVec4 const& lhs, ImVec4 const& rhs);
ImVec4 operator-(ImVec4 const& lhs, float rhs);

#endif  // IMGUI_DISABLE
