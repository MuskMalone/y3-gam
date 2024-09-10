#include <pch.h>
#include "ObjectManager.h"
#include <imgui/imgui.h>

namespace GUI
{

  ObjectManager::ObjectManager(std::string const& windowName, Scene& scene) 
    : GUIWindow(windowName, scene), m_selectedType{} {}

  void ObjectManager::Run()
  {
    ImGui::Begin(m_windowName.c_str());

    float const winHeight = ImGui::GetContentRegionAvail().y;

    ListObjects(winHeight * 0.69f);
    UpdateControls(winHeight * 0.29f);

    ImGui::End();
  }

  void ObjectManager::ListObjects(float height)
  {
    ImGui::BeginChild("Objects", ImVec2(0.f, height));

    ImGui::Text(("Objects (" + std::to_string(GetSceneObjects().size()) + ")").c_str());
    ImGui::Separator();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1.f);

    unsigned i{};
    for (auto& obj : GetSceneObjects())
    {
      if (ImGui::Selectable(("Object " + std::to_string(i) + " [" 
        + obj->meshRef->name + "]").c_str()))
      {
        GUIWindow::m_selectedObj = obj;
      }

      // continue highlighting the current object
      if (obj == GUIWindow::m_selectedObj)
      {
        ImVec2 p_min = ImGui::GetItemRectMin();
        ImVec2 p_max = ImGui::GetItemRectMax();
        ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, IM_COL32(255, 255, 255, 100));
      }

      ++i;
    }

    ImGui::EndChild();
  }

  void ObjectManager::UpdateControls(float height)
  {
    ImGui::BeginChild("Controls", ImVec2(0.f, height));

#ifdef A1_STUFF
    ImGui::Text("Object Type");
    if (ImGui::BeginCombo("##TypeSelection", Graphics::MESH_NAMES[m_selectedType]))
    {
      for (int i{}; i < static_cast<int>(Graphics::MeshType::NUM_TYPES); ++i)
      {
        if (ImGui::Selectable(Graphics::MESH_NAMES[i]))
        {
          m_selectedType = i;
          break;
        }
      }

      ImGui::EndCombo();
    }

    ImGui::Separator();
    ImGui::NewLine();
    auto& objects{ GetSceneObjects() };
    if (ImGui::Button("Add Object"))
    {
      objects.emplace_back(static_cast<Graphics::MeshType>(m_selectedType));
      GUIWindow::m_selectedObjIdx = objects.size() - 1;
    }

    ImGui::SameLine();
#else
    auto& objects{ GetSceneObjects() };
#endif

    bool const shouldDisableRemoval{ objects.empty() };
    if (shouldDisableRemoval) { ImGui::BeginDisabled(); }

    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.f));
    if (ImGui::Button("Remove"))
    {
      unsigned idxToErase{};
      for (; idxToErase < objects.size(); ++idxToErase)
      {
        if (GUIWindow::m_selectedObj == objects[idxToErase]) { break; }
      }
      objects.erase(objects.begin() + idxToErase);
      if (idxToErase == 0) {
        GUIWindow::m_selectedObj = objects.front();
      }
      else {
        GUIWindow::m_selectedObj = objects[idxToErase - 1];
      }
      m_sceneRef.RecomputeBVH();
    }
    ImGui::PopStyleColor();

    if (shouldDisableRemoval) { ImGui::EndDisabled(); }

    ImGui::EndChild();
  }

} // namespace GUI
