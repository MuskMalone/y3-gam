#pragma once
#include <Graphics/ShaderProgram.h>
#include <vector>
#include <Core/TempObject.h>
#include <glm/glm.hpp>
#include <Core/Camera.h>
#include <Graphics/ShaderStructs.h>
#include <variant>
#include <Graphics/Mesh.h>

#include <Graphics/EditorCamera.h>
#include <GUI/Dockable/Inspector.h>
#include "Core/Entity.h"
// forward declaration
namespace GUI { class GUIWindow; class GUIManager; }

class Scene
{
public:
  Scene(const char* vtxShaderFile, const char* fragShaderFile, glm::vec4 const& clearClr = {});

  void Init();
  void Update(float deltaTime);
  void ResetCamera();
  static void AddMesh(ECS::Entity entity);
  static auto& GetMainCamera() { return mEcam; } // for testing
  void Draw();
  void DrawTopView();

  void DebugDraw(); //@todo TEMP

private:
	static // tch to remove added for testing 
	Graphics::EditorCamera mEcam;
  Graphics::ShaderProgram m_shaders, m_defaultShaders;
  Graphics::Light m_light;
  Graphics::Material m_material;

	  std::vector<Camera> m_cameras;

  //static //tch to remove, added for testing
	 // std::vector<std::shared_ptr<Object>> mObjects;

  bool m_leftClickHeld, m_leftClickTriggered;

  inline void StartPanning() noexcept { m_leftClickHeld = true; }
  inline void EndPanning() noexcept { m_leftClickHeld = false; m_leftClickTriggered = true; }
public:
	static // tch to remove added for testing 
		std::shared_ptr<Graphics::Mesh> mesh0, mesh1; //temp
};

//for testing tch
template <>
inline void GUI::Inspector::DrawAddComponentButton<Component::Mesh>(std::string const& name, std::string const& icon) {
    if (GUIManager::GetSelectedEntity().HasComponent<Component::Mesh>()) {
        return;
    }

    auto fillRowWithColour = [](const ImColor& colour) {
        for (int column = 0; column < ImGui::TableGetColumnCount(); column++) {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, colour, column);
        }
    };

    const float rowHeight = 25.0f;
    auto* window = ImGui::GetCurrentWindow();
    window->DC.CurrLineSize.y = rowHeight;
    ImGui::TableNextRow(0, rowHeight);
    ImGui::TableSetColumnIndex(0);

    window->DC.CurrLineTextBaseOffset = 3.0f;

    const ImVec2 rowAreaMin = ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 0).Min;
    const ImVec2 rowAreaMax = { ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(),
      ImGui::TableGetColumnCount() - 1).Max.x, rowAreaMin.y + rowHeight };

    //ImGui::GetWindowDrawList()->AddRect(rowAreaMin, rowAreaMax, Color::IMGUI_COLOR_RED); // Debug

    ImGui::PushClipRect(rowAreaMin, rowAreaMax, false);
    bool isRowHovered, isRowClicked;
    ImGui::ButtonBehavior(ImRect(rowAreaMin, rowAreaMax), ImGui::GetID(name.c_str()),
        &isRowHovered, &isRowClicked, ImGuiButtonFlags_MouseButtonLeft);
    ImGui::SetItemAllowOverlap();
    ImGui::PopClipRect();

    std::string display{ icon + "   " + name};

    ImGui::PushFont(mStyler.GetCustomFont(GUI::MONTSERRAT_SEMIBOLD));
    ImGui::TextUnformatted(display.c_str());
    ImGui::PopFont();

    if (isRowHovered)
        fillRowWithColour(Color::IMGUI_COLOR_ORANGE);

    if (isRowClicked) {
        ECS::Entity ent{ GUIManager::GetSelectedEntity().GetRawEnttEntityID() };
        Scene::AddMesh(ent);
        SetIsComponentEdited(true);
        ImGui::CloseCurrentPopup();
    }
}
