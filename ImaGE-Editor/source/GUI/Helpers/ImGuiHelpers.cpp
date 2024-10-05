#include <pch.h>
#include "ImGuiHelpers.h"
#include <ImGui/imgui.h>
#include <string>
#include "Color.h"

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

  bool TableInputFloat3(std::string propertyName, float* property, float fieldWidth, bool disabled, float minVal, float maxVal, float step) {
    ImGui::BeginDisabled(disabled);

    // Convert IM_COL32 colors to ImVec4
    auto Col32ToImVec4 = [](ImU32 col) -> ImVec4 {
      return ImVec4(
        ((col >> IM_COL32_R_SHIFT) & 0xFF) / 255.0f,
        ((col >> IM_COL32_G_SHIFT) & 0xFF) / 255.0f,
        ((col >> IM_COL32_B_SHIFT) & 0xFF) / 255.0f,
        ((col >> IM_COL32_A_SHIFT) & 0xFF) / 255.0f
      );
      };

    bool valChanged{ false };
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(propertyName.c_str());
    ImGui::TableSetColumnIndex(1);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, Col32ToImVec4(Color::IMGUI_COLOR_RED));
    std::string labelX{ "##X" + propertyName };
    if (ImGui::DragFloat(labelX.c_str(), &property[0], step, minVal, maxVal)) {
      valChanged = true;
    }
    ImGui::PopStyleColor();

    ImGui::TableSetColumnIndex(2);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, Col32ToImVec4(Color::IMGUI_COLOR_GREEN));
    std::string labelY{ "##Y" + propertyName };
    if (ImGui::DragFloat(labelY.c_str(), &property[1], step, minVal, maxVal)) {
      valChanged = true;
    }

    ImGui::PopStyleColor();

    ImGui::TableSetColumnIndex(3);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, Col32ToImVec4(Color::IMGUI_COLOR_BLUE));
    std::string labelZ{ "##Z" + propertyName };
    if (ImGui::DragFloat(labelZ.c_str(), &property[2], step, minVal, maxVal)) {
      valChanged = true;
    }
    ImGui::PopStyleColor();

    ImGui::EndDisabled();

    return valChanged;
  }

  bool TableInputDouble3(std::string propertyName, glm::dvec3& property, float fieldWidth, bool disabled) {
    ImGui::BeginDisabled(disabled);

    // Convert IM_COL32 colors to ImVec4
    auto Col32ToImVec4 = [](ImU32 col) -> ImVec4 {
      return ImVec4(
        ((col >> IM_COL32_R_SHIFT) & 0xFF) / 255.0f,
        ((col >> IM_COL32_G_SHIFT) & 0xFF) / 255.0f,
        ((col >> IM_COL32_B_SHIFT) & 0xFF) / 255.0f,
        ((col >> IM_COL32_A_SHIFT) & 0xFF) / 255.0f
      );
    };

    bool valChanged{ false };
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(propertyName.c_str());
    ImGui::TableSetColumnIndex(1);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, Col32ToImVec4(Color::IMGUI_COLOR_RED));
    std::string labelX{ "##X" + propertyName };
    if (ImGui::InputDouble(labelX.c_str(), &property.x, 0.0, 0.0, "%.3f")) {
      valChanged = true;
    }
    ImGui::PopStyleColor();

    ImGui::TableSetColumnIndex(2);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, Col32ToImVec4(Color::IMGUI_COLOR_GREEN));
    std::string labelY{ "##Y" + propertyName };
    if (ImGui::InputDouble(labelY.c_str(), &property.y, 0.0, 0.0, "%.3f")) {
      valChanged = true;
    }

    ImGui::PopStyleColor();

    ImGui::TableSetColumnIndex(3);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, Col32ToImVec4(Color::IMGUI_COLOR_BLUE));
    std::string labelZ{ "##Z" + propertyName };
    if (ImGui::InputDouble(labelZ.c_str(), &property.z, 0.0, 0.0, "%.3f")) {
      valChanged = true;
    }
    ImGui::PopStyleColor();

    ImGui::EndDisabled();

    return valChanged;
  }

  bool InputDouble3(std::string propertyName, glm::dvec3& property, float fieldWidth, bool disabled) {
    bool valChanged{ false };

    ImGui::BeginDisabled(disabled);

    ImGui::TableNextColumn();
    ImGui::Text(propertyName.c_str());

    propertyName = "##" + propertyName;

    ImGui::TableNextColumn();

    // Convert IM_COL32 colors to ImVec4
    auto Col32ToImVec4 = [](ImU32 col) -> ImVec4 {
      return ImVec4(
        ((col >> IM_COL32_R_SHIFT) & 0xFF) / 255.0f,
        ((col >> IM_COL32_G_SHIFT) & 0xFF) / 255.0f,
        ((col >> IM_COL32_B_SHIFT) & 0xFF) / 255.0f,
        ((col >> IM_COL32_A_SHIFT) & 0xFF) / 255.0f
      );
    };

    ImGui::SetNextItemWidth(fieldWidth);
    ImGui::TextColored(Col32ToImVec4(Color::IMGUI_COLOR_RED), "X");
    ImGui::SameLine();
    if (ImGui::InputDouble((propertyName + "X").c_str(), &property.x, 0, 0, "%.5f")) { valChanged = true; }

    ImGui::SameLine(0, 3);
    ImGui::SetNextItemWidth(fieldWidth);
    ImGui::TextColored(Col32ToImVec4(Color::IMGUI_COLOR_GREEN), "Y");
    ImGui::SameLine();
    if (ImGui::InputDouble((propertyName + "Y").c_str(), &property.y, 0, 0, "%.5f")) { valChanged = true; }

    ImGui::SameLine(0, 3);
    ImGui::SetNextItemWidth(fieldWidth);
    ImGui::TextColored(Col32ToImVec4(Color::IMGUI_COLOR_BLUE), "Z");
    ImGui::SameLine();
    if (ImGui::InputDouble((propertyName + "Z").c_str(), &property.z, 0, 0, "%.5f")) { valChanged = true; }

    ImGui::EndDisabled();

    return valChanged;
  }

  void InputDouble1(std::string propertyName, double& property, bool disabled) {
    ImGui::BeginDisabled(disabled);
    ImGui::TableNextColumn();
    ImGui::Text(propertyName.c_str());
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(ImGui::GetWindowSize().x);
    ImGui::InputDouble(("##" + propertyName).c_str(), &property, 0, 0, "%.2f");
    ImGui::EndDisabled();
  }

  bool InputCheckBox(std::string propertyName, bool& property, bool disabled) {
    ImGui::PushID(propertyName.c_str());
    bool valChanged{ false };
    ImGui::BeginDisabled(disabled);
    ImGui::TableNextColumn();
    ImGui::Text(propertyName.c_str());
    ImGui::TableNextColumn();
    valChanged = ImGui::Checkbox(("##" + propertyName).c_str(), &property);
    ImGui::EndDisabled();
    ImGui::PopID();
    return valChanged;
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

ImVec2 operator*(ImVec2 const& lhs, float rhs) {
  return { lhs.x * rhs, lhs.y * rhs };
}

ImVec2 operator*(float lhs, ImVec2 const& rhs) {
  return { rhs.x * lhs, rhs.y * lhs };
}

ImVec2 operator/(ImVec2 const& lhs, float rhs) {
  return { lhs.x / rhs, lhs.y / rhs };
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
