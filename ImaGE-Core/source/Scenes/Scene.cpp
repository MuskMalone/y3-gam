#include <pch.h>

#include "Scene.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <filesystem>
#include "Graphics/Renderer.h"
#include <Graphics/Mesh/MeshFactory.h>

#include "Graphics/RenderSystem.h"

#include "Core/Entity.h"
#include <Core/Components/Components.h>
#include <Graphics/Camera/EditorCamera.h>

#include <Physics/PhysicsSystem.h>
#include <Asset/IGEAssets.h>
Scene::Scene()
{
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
}

void Scene::Init()
{
   Graphics::RenderSystem::Init();
  Graphics::Renderer::Init();

  Graphics::EditorCamera editorCam(
      glm::vec3(0.0f, 5.0f, 10.0f),  // Position
      -90.0f,                        // Yaw
      -30.0f,                        // Pitch (look downwards slightly)
      60.0f,                         // FOV
      16.0f / 9.0f,                  // Aspect Ratio
      0.1f,                          // Near Clip
      100.0f                         // Far Clip
  );

 
  //mEcam = editorCam;
  //IGE::Assets::GUID cubeMeshSource = IGE_ASSETMGR.LoadRef<IGE::Assets::ModelAsset>("../Assets/Models/bunny_high_poly.imsh");//Graphics::MeshFactory::CreateModelFromImport("../Assets/Models/bunny_high_poly.imsh");
  //mesh0 = std::make_shared<Graphics::Mesh>(cubeMeshSource);

  //IGE::Assets::GUID pyrMeshSource = IGE_ASSETMGR.LoadRef<IGE::Assets::ModelAsset>("Cube");
  //mesh1 = std::make_shared<Graphics::Mesh>(pyrMeshSource);
//  
//  //=====================================================================================================================
//
//// Create 6 cube entities with different matIdx values
//  int numRows = 20; // Total number of rows
//  int numCols = 10; // Total number of columns
//  int matIdx = 0; // Start material index at 0
//
//  for (int i = 0; i < numRows; ++i) { // Loop through rows
//      for (int j = 0; j < numCols; ++j) { // Loop through columns
//          // Create a new entity
//          ECS::Entity entity = ECS::EntityManager::GetInstance().CreateEntity();
//
//          // Set up the TransformComponent with different offsets
//          auto& transformComponent = entity.GetComponent<Component::Transform>();
//          transformComponent.worldPos = glm::vec3(i * 2.0f, 0.0f, j * 2.0f); // Offset for position
//          transformComponent.worldScale = glm::vec3(1.0f);  // Default scale
//          transformComponent.worldRot = { 1.f, 0.f, 0.f, 0.f };  // No rotation
//          transformComponent.modified = true;
//
//          // Set up the MeshComponent
//          auto& renderComponent = entity.EmplaceComponent<Component::Mesh>();
//          renderComponent.mesh = mesh1; // Assign cube mesh
//
//          // Set up the Material
//          auto& Material = entity.EmplaceComponent<Component::Material>();
//          Material.matIdx = i; // Assign current matIdx based on the row number
//      }
//  }



}

void Scene::Update(float deltaTime)
{
    //mEcam.UpdateCamera(deltaTime);
  // update transforms
    //IGE::Physics::PhysicsSystem::GetInstance()->Update(deltaTime);
  //for (auto& obj : mObjects)
  //{
  //    obj->transform = obj->entity.GetComponent<Component::Transform>();
  //    obj->modified = true;
  //   obj->Update(deltaTime);
  //  
  //}
}

void Scene::Draw() const
{
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  //Graphics::Renderer::RenderSceneBegin(mtx);

  //// Imagine a 5x5 grid of cubes, each placed at different positions, scales, and colors
  //for (int x = -2; x <= 2; ++x) {
  //    for (int y = -2; y <= 2; ++y) {
  //        // Calculate the position offset based on grid coordinates
  //        glm::vec3 position = glm::vec3(x * 6.0f, y * 6.0f, 0.0f); // Spacing the cubes 6 units apart

  //        // Random or incremental scaling
  //        glm::vec3 scale = glm::vec3(1.0f + (x + 2) * 0.5f, 1.0f + (y + 2) * 0.5f, 1.0f);

  //        // Assign a unique color for each cube (using normalized RGB values)
  //        glm::vec4 color = glm::vec4(
  //            (x + 2) * 0.2f, // Ranges from 0.0 to 1.0 as x goes from -2 to 2
  //            (y + 2) * 0.2f, // Same for Y axis
  //            1.0f - (x + 2) * 0.2f, // Creates a variety in color between cubes
  //            1.0f);  // Alpha is always 1 (fully opaque)

  //        // Submit the cube with the calculated position, scale, and color
  //        Graphics::Renderer::SubmitMesh(mesh1, position, scale, color, 0.0f);
  //        Graphics::Renderer::SubmitMesh(mesh0, position, scale, color, 0.0f);
  //    }
  //}

  //Graphics::Renderer::RenderSceneEnd();
  
 
  glBindVertexArray(0);
}

//tch: i just added this to visually test physics
void Scene::AddMesh(ECS::Entity entity)
{
  /*  auto xfm{ entity.GetComponent<Component::Transform>() };
    mObjects.emplace_back(std::make_shared<Object>(
        "./assets/models/cube_low_poly.obj",
        xfm.worldPos, 
        xfm.worldScale));
    mObjects.back()->entity = entity;
    entity.EmplaceComponent<Component::Mesh>(Component::Mesh{});*/
}
