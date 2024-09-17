#pragma once
#include <Graphics/ShaderProgram.h>
#include <vector>
#include <Core/TempObject.h>
#include <glm/glm.hpp>
#include <Core/Camera.h>
#include <Graphics/ShaderStructs.h>
#include <variant>
#include "Core/Entity.h"
// forward declaration
namespace GUI { class GUIWindow; class GUIManager; }

class Scene
{
public:
  Scene(const char* vtxShaderFile, const char* fragShaderFile, glm::vec4 const& clearClr = {});

  void Init();
  void Update(float deltaTime);
  inline void RecomputeBVH() noexcept { m_bvhModified = true; }
  inline void ReconstructTree() noexcept { m_reconstructTree = true; }
  void ResetCamera();
  static void AddMesh(ECS::Entity entity);
  void Draw();
  void DrawTopView();

  // allow ObjectEditor to access objects
  friend class GUI::GUIWindow;
  friend class GUI::GUIManager;

private:
  Graphics::ShaderProgram m_shaders, m_defaultShaders;
  Graphics::Light m_light;
  Graphics::Material m_material;
  std::vector<Camera> m_cameras;

  // can encapsulate in a struct if more members are added
  // so that GUIWindow can allow access to relevant members
  // to derived classes
  static //tch to remove, added for testing
	  std::vector<std::shared_ptr<Object>> mObjects;

  bool m_leftClickHeld, m_leftClickTriggered;
  bool m_bvhModified, m_reconstructTree;

  inline void StartPanning() noexcept { m_leftClickHeld = true; }
  inline void EndPanning() noexcept { m_leftClickHeld = false; m_leftClickTriggered = true; }
};
