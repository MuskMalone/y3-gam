/*!*********************************************************************
\file   ImGuiHelpers.cpp
\date   5-October-2024
\brief  Contains the definition of helper functions for ImGui elements.
        Also contains convenience operator overloads for ImGui vectors.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "ImGuiHelpers.h"
#include <ImGui/imgui.h>
#include <string>
#include "Color.h"
#include <Core/Entity.h>
#include <GUI/Helpers/AssetPayload.h>
#include <Asset/IGEAssets.h>
#include <Core/Components/Components.h>

namespace {
  bool IsUnderCanvasEntity(ECS::Entity entity);
}

namespace ImGuiHelpers
{
  // How to accept payload in entire window as well as Tree nodes?
  // https://github.com/ocornut/imgui/issues/5539
  ImGuiPayload const* BeginDrapDropTargetWindow(const char* payloadName)
  {
    ImRect const inner_rect = ImGui::GetCurrentWindow()->InnerRect;
    if (ImGui::BeginDragDropTargetCustom(inner_rect, ImGui::GetID("##WindowBgArea"))) {
      if (ImGuiPayload const* payload = ImGui::AcceptDragDropPayload(payloadName, ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
      {
        if (payload->IsPreview())
        {
          ImDrawList* draw_list = ImGui::GetForegroundDrawList();
          draw_list->AddRectFilled(inner_rect.Min, inner_rect.Max, ImGui::GetColorU32(ImGuiCol_DragDropTarget, 0.05f));
          draw_list->AddRect(inner_rect.Min, inner_rect.Max, ImGui::GetColorU32(ImGuiCol_DragDropTarget), 0.0f, 0, 2.0f);
        }
        if (payload->IsDelivery()) {
          return payload;
        }
      }
      ImGui::EndDragDropTarget();
    }

    return nullptr;
  }

  bool AssetDragDropBehavior(ECS::Entity entity) {
    ImGuiPayload const* drop = ImGui::AcceptDragDropPayload(GUI::AssetPayload::sAssetDragDropPayload);
    if (!drop) { return false; }

    GUI::AssetPayload assetPayload{ reinterpret_cast<const char*>(drop->Data) };
    switch (assetPayload.mAssetType)
    {
    case GUI::AssetPayload::MATERIAL:
    {
      try {
        IGE::Assets::GUID const matGUID{ IGE_ASSETMGR.LoadRef<IGE::Assets::MaterialAsset>(assetPayload.GetFilePath()) };

        // if entity has material component, simply set the material
        if (entity.HasComponent<Component::Material>()) {
          entity.GetComponent<Component::Material>().SetGUID(matGUID);
        }
        // else add the component and set the material
        else {
          entity.EmplaceComponent<Component::Material>(matGUID);
        }
      }
      catch (Debug::ExceptionBase&) {
        IGE_DBGLOGGER.LogError("Unable to get GUID of " + assetPayload.GetFilePath());
      }
      break;
    }
    case GUI::AssetPayload::SPRITE:
    {
      // because we have Sprite2D and Image components, we will
      // climb up the hierarchy to check if it is a child of a canvas entity
      bool const hasCanvasParent{ IsUnderCanvasEntity(entity) };
      try {
        IGE::Assets::GUID const texGUID{ IGE_ASSETMGR.LoadRef<IGE::Assets::TextureAsset>(assetPayload.GetFilePath()) };
        
        // under Canvas so we use the Image component
        if (hasCanvasParent) {
          // add Image component if it doesn't exist
          if (entity.HasComponent<Component::Image>()) {
            entity.GetComponent<Component::Image>().textureAsset = texGUID;
          }
          // else add the component and set the guid
          else {
            entity.EmplaceComponent<Component::Image>(texGUID);
          }
        }
        // not under Canvas, handle Sprite2D case
        else {
          // add Sprite2D component if it doesn't exist
          if (entity.HasComponent<Component::Sprite2D>()) {
            entity.GetComponent<Component::Sprite2D>().textureAsset = texGUID;
          }
          // else add the component and set the guid
          else {
            entity.EmplaceComponent<Component::Sprite2D>(texGUID);
          }
        }
      }
      catch (Debug::ExceptionBase&) {
        IGE_DBGLOGGER.LogError("Unable to get GUID of " + assetPayload.GetFilePath());
      }
      break;
    }
    default:
      break;
    }

    return true;
  }

  bool TableInputFloat2(std::string const& propertyName, float* property, float fieldWidth, bool disabled, float minVal, float maxVal, float step, const char* fmt) {
    ImGui::BeginDisabled(disabled);

    bool valChanged{ false };
    float const elemSize{ ImGui::GetContentRegionAvail().x * 0.9f }, cursorOffset{ ImGui::GetContentRegionAvail().x * 0.05f };
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(propertyName.c_str());
    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(elemSize);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cursorOffset);

    std::string labelX{ "##X" + propertyName };
    if (ImGui::DragFloat(labelX.c_str(), &property[0], step, minVal, maxVal, fmt)) {
      valChanged = true;
    }

    ImGui::TableSetColumnIndex(2);
    ImGui::SetNextItemWidth(elemSize);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cursorOffset);
    std::string labelY{ "##Y" + propertyName };
    if (ImGui::DragFloat(labelY.c_str(), &property[1], step, minVal, maxVal, fmt)) {
      valChanged = true;
    }

    ImGui::EndDisabled();

    return valChanged;
  }

  bool TableInputFloat3(std::string const& propertyName, float* property, float fieldWidth, bool disabled, float minVal, float maxVal, float step) {
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
    float const elemSize{ ImGui::GetContentRegionAvail().x * 0.9f }, cursorOffset{ ImGui::GetContentRegionAvail().x * 0.05f };
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(propertyName.c_str());
    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(elemSize);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cursorOffset);

    ImGui::PushStyleColor(ImGuiCol_FrameBg, Col32ToImVec4(Color::IMGUI_COLOR_RED));
    std::string labelX{ "##X" + propertyName };
    if (ImGui::DragFloat(labelX.c_str(), &property[0], step, minVal, maxVal)) {
      valChanged = true;
    }
    ImGui::PopStyleColor();

    ImGui::TableSetColumnIndex(2);
    ImGui::SetNextItemWidth(elemSize);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cursorOffset);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, Col32ToImVec4(Color::IMGUI_COLOR_GREEN));
    std::string labelY{ "##Y" + propertyName };
    if (ImGui::DragFloat(labelY.c_str(), &property[1], step, minVal, maxVal)) {
      valChanged = true;
    }

    ImGui::PopStyleColor();

    ImGui::TableSetColumnIndex(3);
    ImGui::SetNextItemWidth(elemSize);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cursorOffset);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, Col32ToImVec4(Color::IMGUI_COLOR_BLUE));
    std::string labelZ{ "##Z" + propertyName };
    if (ImGui::DragFloat(labelZ.c_str(), &property[2], step, minVal, maxVal)) {
      valChanged = true;
    }
    ImGui::PopStyleColor();

    ImGui::EndDisabled();

    return valChanged;
  }

  bool TableInputDouble3(std::string const& propertyName, glm::dvec3& property, float fieldWidth, bool disabled, double minVal, double maxVal, float step) {
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
    float const elemSize{ ImGui::GetContentRegionAvail().x * 0.9f }, cursorOffset{ ImGui::GetContentRegionAvail().x * 0.05f };
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(propertyName.c_str());
    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(elemSize);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cursorOffset);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, Col32ToImVec4(Color::IMGUI_COLOR_RED));
    std::string labelX{ "##X" + propertyName };
    if (ImGui::DragScalar(labelX.c_str(), ImGuiDataType_Double, &property.x, step, &minVal, &maxVal, "%.3f")) {
      valChanged = true;
    }
    ImGui::PopStyleColor();

    ImGui::TableSetColumnIndex(2);
    ImGui::SetNextItemWidth(elemSize);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cursorOffset);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, Col32ToImVec4(Color::IMGUI_COLOR_GREEN));
    std::string labelY{ "##Y" + propertyName };
    if (ImGui::DragScalar(labelY.c_str(), ImGuiDataType_Double, &property.y, step, &minVal, &maxVal, "%.3f")) {
      valChanged = true;
    }

    ImGui::PopStyleColor();

    ImGui::TableSetColumnIndex(3);
    ImGui::SetNextItemWidth(elemSize);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cursorOffset);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, Col32ToImVec4(Color::IMGUI_COLOR_BLUE));
    std::string labelZ{ "##Z" + propertyName };
    if (ImGui::DragScalar(labelZ.c_str(), ImGuiDataType_Double, &property.z, step, &minVal, &maxVal, "%.3f")) {
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

  void InputDouble1(std::string const& propertyName, double& property, bool disabled) {
    ImGui::BeginDisabled(disabled);
    ImGui::TableNextColumn();
    ImGui::Text(propertyName.c_str());
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(ImGui::GetWindowSize().x);
    ImGui::InputDouble(("##" + propertyName).c_str(), &property, 0, 0, "%.2f");
    ImGui::EndDisabled();
  }

  bool InputCheckBox(std::string const& propertyName, bool& property, bool disabled) {
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

  // source: https://github.com/ocornut/imgui/issues/228#issuecomment-1723778093
  // When multi-viewports are disabled: wrap in main viewport.
  // When multi-viewports are enabled: wrap in monitor.
  // FIXME: Experimental: not sure how this behaves with multi-monitor and monitor coordinates gaps.
  void WrapMousePos(int axises_mask)
  {
    ImGuiContext& g = *GImGui;
#ifdef IMGUI_HAS_DOCK
    if (g.IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      const ImGuiPlatformMonitor* monitor = ImGui::GetViewportPlatformMonitor(g.MouseViewport);
      WrapMousePosEx(axises_mask, ImRect(monitor->MainPos, monitor->MainPos + monitor->MainSize));
    }
    else
#endif
    {
      ImGuiViewport* viewport = ImGui::GetMainViewport();
      WrapMousePosEx(axises_mask, ImRect(viewport->Pos, viewport->Pos + viewport->Size));
    }
  }

  void WrapMousePosEx(int axises_mask, const ImRect& wrap_rect)
  {
    ImGuiContext& g = *GImGui;
    IM_ASSERT(axises_mask == 1 || axises_mask == 2 || axises_mask == (1 | 2));
    ImVec2 p_mouse = g.IO.MousePos;
    for (int axis = 0; axis < 2; axis++)
    {
      if ((axises_mask & (1 << axis)) == 0) {
        continue;
      }
      float size = wrap_rect.Max[axis] - wrap_rect.Min[axis];
      if (p_mouse[axis] >= wrap_rect.Max[axis] - 1.f) {
        p_mouse[axis] = wrap_rect.Min[axis] + 1.0f;
      }
      else if (p_mouse[axis] <= wrap_rect.Min[axis] + 1.f) {
        p_mouse[axis] = wrap_rect.Max[axis] - 1.0f;
      }
    }
    if (p_mouse.x != g.IO.MousePos.x || p_mouse.y != g.IO.MousePos.y) {
      ImGui::TeleportMousePos(p_mouse);
    }
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

namespace {
  bool IsUnderCanvasEntity(ECS::Entity entity) {
    ECS::EntityManager& em{ IGE_ENTITYMGR };

    // keep checking if next parent has Canvas component
    while (em.HasParent(entity)) {
      entity = em.GetParentEntity(entity);
      if (entity.HasComponent<Component::Canvas>()) { return true; }
    }

    return false;
  }
}
