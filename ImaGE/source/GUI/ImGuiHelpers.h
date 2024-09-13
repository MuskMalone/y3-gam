#pragma once

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
