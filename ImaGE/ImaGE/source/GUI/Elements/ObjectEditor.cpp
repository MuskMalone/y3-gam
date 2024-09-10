#include <pch.h>
#include "ObjectEditor.h"
#include <imgui/imgui.h>
#include <GUI/ImGuiHelpers.h>
#include <sstream>
#include <glm/gtx/transform.hpp>
#include <BoundingVolumes/BoundingSphere.h>

namespace GUI
{

  ObjectEditor::ObjectEditor(std::string const& windowName, Scene& scene) : GUIWindow(windowName, scene) {}

  void ObjectEditor::Run()
  {
    ImGui::Begin(m_windowName.c_str());

    EditObjects();

    ImGui::End();
  }

  void ObjectEditor::EditObjects()
  {
    ImGui::BeginChild("Edit");

    // if no object selected, don't display controls
    if (GUIWindow::GetSceneObjects().empty() || !GUIWindow::m_selectedObj) {
      ImGui::Text("Currently Editing: No object selected");
      ImGui::EndChild();
      return;
    }

    Object& selectedObj{ *GUIWindow::m_selectedObj };

    std::ostringstream oss{};
    oss << "Currently Editing: " << selectedObj.meshRef->name;
    ImGui::Text(oss.str().c_str());

    ImGui::NewLine();
    
    if (ImGuiHelpers::InputSliderVec3("Position", selectedObj.transform.pos, -30.f, 30.f)) {
      selectedObj.modified = true;
    }
    //if (ImGuiHelpers::InputSliderVec3("Scale", selectedObj.transform.scale, 0.1f, 100.f)) {
    //  selectedObj.modified = true;
    //}
    //if (ImGuiHelpers::InputSliderVec3("Rotate", selectedObj.transform.rot, 0.f, 359.f)) {
    //  selectedObj.modified = true;
    //}

    if (selectedObj.modified) {
      m_sceneRef.RecomputeBVH();
    }

    /*ImGui::NewLine();
    ImGui::Text("Color");
    ImGui::ColorEdit4("##objColor", reinterpret_cast<float*>(&selectedObj.clr));*/

    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.95f);
    if (ImGui::Button("Reset"))
    {
      selectedObj.Reset();
      m_sceneRef.RecomputeBVH();
    }
    ImGui::EndChild();
  }

} // namespace GUI
