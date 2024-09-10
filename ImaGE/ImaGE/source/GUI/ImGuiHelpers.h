#pragma once

namespace ImGuiHelpers
{

  bool InputSliderVec3(std::string const& label, glm::vec3& vector, float min, float max);
  bool InputSliderVec4(std::string const& label, glm::vec4& vector, float min, float max);

} // namespace ImGuiHelpers
