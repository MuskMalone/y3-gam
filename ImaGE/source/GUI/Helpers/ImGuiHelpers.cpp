#include <pch.h>
#include "ImGuiHelpers.h"
#include <ImGui/imgui.h>
//#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <string>

namespace ImGuiHelpers
{

  bool BeginDrapDropTargetWindow(const char* payloadName)
  {
    ImRect inner_rect = ImGui::GetCurrentWindow()->InnerRect;
    if (ImGui::BeginDragDropTargetCustom(inner_rect, ImGui::GetID("##WindowBgArea")))
      if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadName, ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
      {
        if (payload->IsPreview())
        {
          ImDrawList* draw_list = ImGui::GetForegroundDrawList();
          draw_list->AddRectFilled(inner_rect.Min, inner_rect.Max, ImGui::GetColorU32(ImGuiCol_DragDropTarget, 0.05f));
          draw_list->AddRect(inner_rect.Min, inner_rect.Max, ImGui::GetColorU32(ImGuiCol_DragDropTarget), 0.0f, 0, 2.0f);
        }
        if (payload->IsDelivery())
          return true;
        ImGui::EndDragDropTarget();
      }
    return false;
  }

} // namespace ImGuiHelpers


// operator overloads
ImVec2 operator+(ImVec2 const& lhs, ImVec2 const& rhs) {
  return { lhs.x + rhs.x, lhs.y + rhs.y };
}

ImVec2 operator+(ImVec2 const& lhs, float rhs) {
  return { lhs.x + rhs, lhs.y + rhs };
}

ImVec2 operator-(ImVec2 const& lhs, ImVec2 const& rhs) {
  return { lhs.x - rhs.x, lhs.y - rhs.y };
}

ImVec2 operator-(ImVec2 const& lhs, float rhs) {
  return { lhs.x - rhs, lhs.y - rhs };
}

ImVec4 operator+(ImVec4 const& lhs, ImVec4 const& rhs) {
  return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w };
}

ImVec4 operator+(ImVec4 const& lhs, float rhs) {
  return { lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs };
}

ImVec4 operator-(ImVec4 const& lhs, ImVec4 const& rhs) {
  return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w };
}

ImVec4 operator-(ImVec4 const& lhs, float rhs) {
  return { lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs };
}
