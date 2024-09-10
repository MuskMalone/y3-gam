#include <pch.h>
#include <ImGui/imgui.h>
#include "ImGuiHelpers.h"
//#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <string>
#include <glm/glm.hpp>

namespace ImGuiHelpers
{

  bool InputSliderVec3(std::string const& label, glm::vec3& vector, float min, float max)
  {
    bool modified{ false };

    ImGui::Text(label.c_str());
    if (ImGui::BeginTable("##sliderTable", 3))
    {
      ImGui::TableNextColumn();
      ImGui::Text("x");
      ImGui::SameLine();
      if (ImGui::SliderFloat("##xComp", &vector.x, min, max)) { modified = true; }

      ImGui::TableNextColumn();
      ImGui::Text("y");
      ImGui::SameLine();
      if (ImGui::SliderFloat("##yComp", &vector.y, min, max)) { modified = true; }

      ImGui::TableNextColumn();
      ImGui::Text("z");
      ImGui::SameLine();
      if (ImGui::SliderFloat("##zComp", &vector.z, min, max)) { modified = true; }

      ImGui::EndTable();
    }
    return modified;
  }

  bool InputSliderVec4(std::string const& label, glm::vec4& vector, float min, float max)
  {
    bool modified{ false };

    ImGui::Text(label.c_str());
    if (ImGui::BeginTable("##sliderTable", 4))
    {
      ImGui::TableNextColumn();
      ImGui::Text("x");
      ImGui::SameLine();
      if (ImGui::SliderFloat("##xComp", &vector.x, min, max)) { modified = true; }

      ImGui::TableNextColumn();
      ImGui::Text("y");
      ImGui::SameLine();
      if (ImGui::SliderFloat("##yComp", &vector.y, min, max)) { modified = true; }

      ImGui::TableNextColumn();
      ImGui::Text("z");
      ImGui::SameLine();
      if (ImGui::SliderFloat("##zComp", &vector.z, min, max)) { modified = true; }

      ImGui::TableNextColumn();
      ImGui::Text("z");
      ImGui::SameLine();
      if (ImGui::SliderFloat("##wComp", &vector.w, min, max)) { modified = true; }

      ImGui::EndTable();
    }
    return modified;
  }

} // namespace ImGuiHelpers
