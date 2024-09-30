#include <pch.h>
#include "TempScene.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <filesystem>
#include "Graphics/Renderer.h"
#include "Graphics/MeshFactory.h"

#include "Graphics/RenderSystem.h"

#include "Core/EntityManager.h"
#include "Core/Entity.h"
#include "Core/Component/Components.h"
#include "Graphics/EditorCamera.h"


#include <Physics/PhysicsSystem.h>
//std::vector<std::shared_ptr<Object>> Scene::mObjects;
Graphics::EditorCamera Scene::mEcam;
std::shared_ptr<Graphics::Mesh> Scene::mesh0, Scene::mesh1;
Scene::Scene(const char* vtxShaderFile, const char* fragShaderFile, glm::vec4 const& clearClr)
  : m_shaders{}, m_defaultShaders{}, 
  m_light{ { 0.f, 25.f, 0.f }, { 0.4f, 0.4f, 0.4f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
  m_material{ glm::vec3(1.f), glm::vec3(1.f), glm::vec3(1.f), 100.f },
  m_leftClickHeld{ false }, m_leftClickTriggered{ true }
{
  glClearColor(clearClr.r, clearClr.g, clearClr.b, clearClr.a);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  m_shaders.CompileShaderFile(vtxShaderFile, fragShaderFile);
  m_defaultShaders.CompileShaderFile("./Assets/Shaders/Framework.vert.glsl", "./Assets/Shaders/Framework.frag.glsl");
  m_cameras.emplace_back(WINDOW_WIDTH<int>, WINDOW_HEIGHT<int>, glm::vec3(3.f, 3.f, 15.f));
  m_cameras.emplace_back(WINDOW_WIDTH<int>, WINDOW_HEIGHT<int>, glm::vec3(0.f, 15.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(1.f, 0.f, 0.f), false);
}

void Scene::Init()
{
   Graphics::RenderSystem::Init();

  Graphics::EditorCamera editorCam(
      glm::vec3(0.0f, 5.0f, 10.0f),  // Position
      -90.0f,                        // Yaw
      -30.0f,                        // Pitch (look downwards slightly)
      60.0f,                         // FOV
      16.0f / 9.0f,                  // Aspect Ratio
      0.1f,                          // Near Clip
      100.0f                         // Far Clip
  );

 
  mEcam = editorCam;
  std::shared_ptr<Graphics::MeshSource> cubeMeshSource = Graphics::MeshFactory::CreateModelFromImport("./Assets/Models/bunny_high_poly.imsh");
  //std::shared_ptr<Graphics::MeshSource> cubeMeshSource = Graphics::MeshFactory::CreateCube();
  mesh0 = std::make_shared<Graphics::Mesh>(cubeMeshSource);

  std::shared_ptr<Graphics::MeshSource> pyrMeshSource = Graphics::MeshFactory::CreateCube();
  mesh1 = std::make_shared<Graphics::Mesh>(pyrMeshSource);
  //=====================================================================================================================

  //// Create a new entity for the plane
  //ECS::Entity planeEntity = ECS::EntityManager::GetInstance().CreateEntity();

  //// Set up the TransformComponent to create a flat plane
  //auto& planeTransformComponent = planeEntity.GetComponent<Component::Transform>();
  //planeTransformComponent.worldPos = glm::vec3(0.0f, -1.0f, 0.0f); // Place it slightly below the cubes (as ground)
  //planeTransformComponent.worldScale = glm::vec3(100.0f, 0.1f, 100.0f); // Scale it to make a large plane
  //planeTransformComponent.worldRot = {}; // No rotation

  //// Set up the MeshComponent for the plane
  //auto& planeMeshComponent = planeEntity.EmplaceComponent<Component::Mesh>();
  //planeMeshComponent.mesh = mesh0; // Use the cube mesh to create the plane

  //ECS::Entity entity = ECS::EntityManager::GetInstance().CreateEntity();

  // Set up the TransformComponent with different offsets
  ECS::Entity light = ECS::EntityManager::GetInstance().CreateEntity();
  auto& transformC = light.GetComponent<Component::Transform>();
  transformC.worldPos = glm::vec3(2.f, 2.0f, 2.f); // Offset for position
  transformC.worldScale = glm::vec3(1.f);  // Default scale
  transformC.worldRot = {};  // No rotation
  auto& meshC = light.EmplaceComponent<Component::Mesh>();
  meshC.mesh = nullptr;


  for (int i = 0; i < 5; ++i)
  {
      for (int j = 0; j < 0; ++j)
      {
          // Create a new entity
          ECS::Entity entity = ECS::EntityManager::GetInstance().CreateEntity();

          // Set up the TransformComponent with different offsets
          auto& transformComponent = entity.GetComponent<Component::Transform>();
          transformComponent.worldPos = glm::vec3(i * 2.0f, 0.0f, j * 2.0f); // Offset for position
          transformComponent.worldScale = glm::vec3(1.0f);  // Default scale
          transformComponent.worldRot = {};  // No rotation

          // Set up the MeshComponent
          auto& meshComponent = entity.EmplaceComponent<Component::Mesh>();

          // Assign alternating meshes between cube and pyramid
          if ((i + j) % 2 == 0) {
              meshComponent.mesh = mesh0; // Assign cube mesh
          }
          else {
              meshComponent.mesh = mesh1; // Assign pyramid mesh
          }
      }
  }
}

void Scene::Update(float deltaTime)
{
    //mEcam.UpdateCamera(deltaTime);
  // update transforms
    IGE::Physics::PhysicsSystem::GetInstance()->Update(deltaTime);
  //for (auto& obj : mObjects)
  //{
  //    obj->transform = obj->entity.GetComponent<Component::Transform>();
  //    obj->modified = true;
  //   obj->Update(deltaTime);
  //  
  //}


  // update camera
  for (auto& cam : m_cameras)
  {
    cam.Update(deltaTime);
  }

}

void Scene::Draw()
{
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);
  
  m_defaultShaders.Use();
  m_cameras.front().SetUniforms(m_defaultShaders);
  m_defaultShaders.Unuse();

  DebugDraw();
 
  glBindVertexArray(0);
}

void Scene::DebugDraw() {

    // Get the view and projection matrices from the camera
    glm::mat4 viewMatrix = m_cameras.front().GetViewMatrix();  // World to view space
    glm::mat4 projMatrix = m_cameras.front().GetProjMatrix();  // View to projection space

    glm::mat4 mtx = projMatrix * viewMatrix;
   // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    Graphics::RenderSystem::RenderEditorScene(mEcam);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Scene::DrawTopView()
{
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);

  m_shaders.Use();

  m_cameras[1].SetUniforms(m_shaders);
  m_light.SetUniforms(m_shaders);
  m_material.SetUniforms(m_shaders);

  //for (auto& obj : mObjects)
  //{
  //  m_shaders.SetUniform("uMdlTransform", obj->mdlTransform);
  //  m_shaders.SetUniform("uVtxClr", obj->clr);
  //  auto const& mdl{ *obj->meshRef };
  //  glBindVertexArray(mdl.GetVAO());

  //  // if primitive type is a point, render with GL_POINTS
  //  if (mdl.isUsingIndices)
  //  {
  //    glDrawElements(mdl.primitiveType, static_cast<GLsizei>(mdl.drawCount), GL_UNSIGNED_SHORT, NULL);
  //  }
  //  // else draw as per normal
  //  else {
  //    glDrawArrays(mdl.primitiveType, 0, static_cast<GLsizei>(mdl.drawCount));
  //  }
  //}
  m_shaders.Unuse();

  m_defaultShaders.Use();
  m_cameras[1].SetUniforms(m_defaultShaders);
  m_defaultShaders.Unuse();

  glBindVertexArray(0);
}

void Scene::ResetCamera()
{
  m_cameras.front().Reset(glm::vec3(3.f, 3.f, 15.f), glm::vec3());
}

//tch: i just added this to visually test physics
void Scene::AddMesh(ECS::Entity entity)
{
    auto xfm{ entity.GetComponent<Component::Transform>() };
    auto& meshComponent = entity.EmplaceComponent<Component::Mesh>();
    meshComponent.mesh = mesh0;
}