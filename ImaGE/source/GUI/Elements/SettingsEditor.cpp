#include <pch.h>
#include "SettingsEditor.h"
#include <imgui/imgui.h>
#include <BoundingVolumes/BoundingSphere.h>


namespace GUI
{
  SettingsEditor::SettingsEditor(std::string const& windowName, Scene& scene)
    : GUIWindow(windowName, scene) {}

  void SettingsEditor::Run()
  {
    ImGui::Begin(m_windowName.c_str());

    ImGui::NewLine();
    ImGui::Text("Show Bounding Volume");
    ImGui::SameLine();
    ImGui::Checkbox("##boundingVolumes", &Settings::showBoundingVol);

#ifdef A2_STUFF
    ImGui::NewLine();
    ImGui::Text("BV Type");

    if (ImGui::BeginCombo("##BVTypeDropdown", BV::BV_TYPE_NAMES[static_cast<int>(Settings::bvType)]))
    {
      for (int i{}; i < static_cast<int>(BV::BVType::NUM_TYPES); ++i)
      {
        if (ImGui::Selectable(BV::BV_TYPE_NAMES[i]))
        {
          auto const selected{ static_cast<BV::BVType>(i) };
          if (selected != Settings::bvType)
          {
            if (selected >= BV::BVType::OBB) {
              Settings::showTree = false;
            }
            m_sceneRef.ChangeAllBVs(selected);
          }
          break;
        }
      }

      ImGui::EndCombo();
    }

    if (Settings::bvType != BV::BVType::AABB)
    {
      if (Settings::bvType == BV::BVType::LARSSON)
      {
        ImGui::NewLine();
        static constexpr const char* eposStr[]{ "EPOS-6", "EPOS-14", "EPOS-26", "EPOS-98" };
        if (ImGui::BeginCombo("##EPOSDropdown", eposStr[static_cast<int>(BV::BoundingSphere::eposMode)]))
        {
          for (int i{}; i < static_cast<int>(BV::BoundingSphere::EPOS::NUM_EPOS); ++i)
          {
            if (ImGui::Selectable(eposStr[i]))
            {
              BV::BoundingSphere::eposMode = static_cast<BV::BoundingSphere::EPOS>(i);
              m_sceneRef.RecomputeAllBVs();
              break;
            }
          }

          ImGui::EndCombo();
        }
      }

      ImGui::NewLine();
      if (ImGui::Button("Recompute Initial Points")) {
        m_sceneRef.RandomizeBVInitialPoints();
      }
    }
#endif

    ImGui::NewLine();
    ImGui::Text("Render Mode");
    auto const& renderModes{ Settings::renderModeNames };
    if (ImGui::BeginCombo("##renderMode", renderModes[Settings::renderMode]))
    {
      for (int i{}; i < static_cast<int>(Settings::RenderMode::NUM_MODES); ++i)
      {
        if (ImGui::Selectable(renderModes[i]))
        {
          Settings::SetRenderMode(static_cast<Settings::RenderMode>(i));
          break;
        }
      }

      ImGui::EndCombo();
    }

    if (Settings::renderMode == Settings::RenderMode::POINTS)
    {
      ImGui::NewLine();
      ImGui::Text("Point Size");
      if (ImGui::SliderFloat("##pointSize", &Settings::pointSize, 1.f, 50.f)) {
        Settings::SetPointSize(Settings::pointSize);
      }
    }

    ImGui::NewLine();
    if (ImGui::Button("Reset Camera"))
    {
      m_sceneRef.ResetCamera();
    }

    ImGui::NewLine();
    ImGui::Text("Line Width");
    if (ImGui::SliderFloat("##pointSize", &Settings::lineWidth, 1.f, 50.f)) {
      Settings::SetLineWidth(Settings::lineWidth);
    }

    ImGui::NewLine();
    ImGui::Text("Background Color");
    if (ImGui::ColorEdit4("##clearClr", Settings::clearClr))
    {
      glClearColor(Settings::clearClr[0], Settings::clearClr[1], Settings::clearClr[2], Settings::clearClr[3]);
    }

#ifdef A1_STUFF
    ImGui::NewLine();
    ImGui::Text("Collided Color");
    ImGui::ColorEdit4("##collidedClr", reinterpret_cast<float*>(&m_settings.collidedClr));
#endif

    ImGui::NewLine();
    ImGui::Text("Depth Buffer Test");
    ImGui::SameLine();
    if (ImGui::Checkbox("##depthBufferTest", &Settings::enableDepthBuffer))
    {
      if (Settings::enableDepthBuffer) {
        glEnable(GL_DEPTH_TEST);
      }
      else {
        glDisable(GL_DEPTH_TEST);
      }
    }

    ImGui::NewLine();
    ImGui::Text("Back-face Culling");
    ImGui::SameLine();
    if (ImGui::Checkbox("##backfaceCulling", &Settings::backfaceCulling))
    {
      if (Settings::backfaceCulling) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
      }
      else {
        glDisable(GL_CULL_FACE);
      }
    }



    ImGui::End();
  }

} // namespace GUI
