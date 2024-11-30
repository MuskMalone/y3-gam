#pragma once
#include <vector>
#include <variant>
#include <Graphics/Mesh/Mesh.h>
#include <Graphics/Camera/EditorCamera.h>

#include "Core/Entity.h"

class Scene
{
public:
	Scene();

  void Init();
  void Update(float deltaTime);
  void Draw() const;

  inline Graphics::EditorCamera& GetEditorCamera() noexcept { return mEcam; }

  static void AddMesh(ECS::Entity entity);

private:
	Graphics::EditorCamera mEcam;
  //Graphics::Material m_material;

public:
	std::shared_ptr<Graphics::Mesh> mesh0, mesh1; //temp
};