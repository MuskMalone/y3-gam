#pragma once
#include <glm/vec4.hpp> 
#include <imgui/imgui.h>

namespace Color {
  constexpr auto COLOR_GREEN = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
  constexpr auto COLOR_BLUE = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
  constexpr auto COLOR_WHITE = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
  constexpr auto COLOR_BLACK = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  constexpr auto COLOR_YELLOW = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
  constexpr auto COLOR_CYAN = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
  constexpr auto COLOR_MAGENTA = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
  constexpr auto COLOR_ORANGE = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
  constexpr auto COLOR_PURPLE = glm::vec4(0.5f, 0.0f, 0.5f, 1.0f);
  constexpr auto COLOR_GREY = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
  constexpr auto COLOR_DARK_GREY = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
  constexpr auto COLOR_LIGHT_GREY = glm::vec4(0.75f, 0.75f, 0.75f, 1.0f);
  constexpr auto COLOR_PINK = glm::vec4(1.0f, 0.75f, 0.8f, 1.0f);
  constexpr auto COLOR_BROWN = glm::vec4(0.6f, 0.3f, 0.1f, 1.0f);
  constexpr auto COLOR_LIME = glm::vec4(0.5f, 1.0f, 0.0f, 1.0f);

  constexpr auto IMGUI_COLOR_RED = IM_COL32(255, 0, 0, 255);
  constexpr auto IMGUI_COLOR_GREEN = IM_COL32(0, 255, 0, 255);
  constexpr auto IMGUI_COLOR_BLUE = IM_COL32(0, 0, 255, 255);
  constexpr auto IMGUI_COLOR_WHITE = IM_COL32(255, 255, 255, 255);
  constexpr auto IMGUI_COLOR_BLACK = IM_COL32(0, 0, 0, 255);
  constexpr auto IMGUI_COLOR_YELLOW = IM_COL32(255, 255, 0, 255);
  constexpr auto IMGUI_COLOR_CYAN = IM_COL32(0, 255, 255, 255);
  constexpr auto IMGUI_COLOR_MAGENTA = IM_COL32(255, 0, 255, 255);
  constexpr auto IMGUI_COLOR_ORANGE = IM_COL32(255, 127, 0, 255);
  constexpr auto IMGUI_COLOR_PURPLE = IM_COL32(127, 0, 127, 255);
  constexpr auto IMGUI_COLOR_GREY = IM_COL32(127, 127, 127, 255);
  constexpr auto IMGUI_COLOR_DARK_GREY = IM_COL32(63, 63, 63, 255);
  constexpr auto IMGUI_COLOR_LIGHT_GREY = IM_COL32(191, 191, 191, 255);
  constexpr auto IMGUI_COLOR_PINK = IM_COL32(255, 191, 204, 255);
  constexpr auto IMGUI_COLOR_BROWN = IM_COL32(153, 76, 26, 255);
  constexpr auto IMGUI_COLOR_LIME = IM_COL32(127, 255, 0, 255);
}