#pragma once
#include <vector>
#include <Core/Camera.h>
#include <variant>
#include <Graphics/Mesh.h>
#include <Graphics/EditorCamera.h>

#include "Core/Entity.h"

class Scene
{
public:
  Scene();

  void Init();
  void Update(float deltaTime);
  void Draw();

  static void AddMesh(ECS::Entity entity);
  static Camera& GetMainCamera() { return m_cameras.front(); }

private:
	Graphics::EditorCamera mEcam;
  //Graphics::Material m_material;
  static std::vector<Camera> m_cameras;// tch to remove added for testing 

public:
	std::shared_ptr<Graphics::Mesh> mesh0, mesh1; //temp
};
