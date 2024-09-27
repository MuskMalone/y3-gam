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
  static Camera& GetMainCamera() { return m_cameras.front(); }
  void Draw();
  void DrawTopView();

  void DebugDraw(); //@todo TEMP

private:
	Graphics::EditorCamera mEcam;
  Graphics::ShaderProgram m_shaders, m_defaultShaders;
  Graphics::Light m_light;
  Graphics::Material m_material;
  static // tch to remove added for testing 
	  std::vector<Camera> m_cameras;

  //static //tch to remove, added for testing
	 // std::vector<std::shared_ptr<Object>> mObjects;

  bool m_leftClickHeld, m_leftClickTriggered;

  inline void StartPanning() noexcept { m_leftClickHeld = true; }
  inline void EndPanning() noexcept { m_leftClickHeld = false; m_leftClickTriggered = true; }
public:
	std::shared_ptr<Graphics::Mesh> mesh0, mesh1; //temp
};
