#pragma once
#include <Graphics/ShaderProgram.h>
#include <vector>
#include <Core/Object.h>
#include <glm/glm.hpp>
#include <Core/Settings.h>
#include <Core/Camera.h>
#include <Graphics/ShaderStructs.h>
#include <BVH/VolumeBounder.h>
#include <SpatialPartitioning/Octree/Octree.h>
#include <SpatialPartitioning/BSPTree/BSPTree.h>
#include <variant>

// forward declaration
namespace GUI { class GUIWindow; class GUIManager; }

class Scene
{
public:
  Scene(const char* vtxShaderFile, const char* fragShaderFile, glm::vec4 const& clearClr = {});
  ~Scene();

  void Init();
  void Update(float deltaTime);
  inline void RecomputeBVH() noexcept { m_bvhModified = true; }
  inline void ReconstructTree() noexcept { m_reconstructTree = true; }
  void ResetCamera();

  void Draw();
  void DrawTopView();

  void RandomizeBVInitialPoints();
  void ChangeAllBVs(BV::BVType type);
  void RecomputeAllBVs();
  void ChangeConstructionMethod(BV::BVHierarchy::Type type);

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
  std::vector<std::shared_ptr<Object>> m_objects;

  BV::VolumeBounder m_bvh;
  SP::Octree m_octree;  // spatial partitioning tree
  SP::BSPTree m_bsp;

  bool m_leftClickHeld, m_leftClickTriggered;
  bool m_bvhModified, m_reconstructTree;

  inline void StartPanning() noexcept { m_leftClickHeld = true; }
  inline void EndPanning() noexcept { m_leftClickHeld = false; m_leftClickTriggered = true; }

  void LoadMultiPartModel(const char* path, glm::vec3 const& scale = { 1.f, 1.f, 1.f }, BV::BVType type = BV::BVType::AABB);
  void ReconstructBVH();
};
