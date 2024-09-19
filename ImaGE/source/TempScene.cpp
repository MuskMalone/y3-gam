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

#include <Physics/PhysicsSystem.h>
std::vector<std::shared_ptr<Object>> Scene::mObjects;
std::vector<Camera> Scene::m_cameras;

Scene::Scene(const char* vtxShaderFile, const char* fragShaderFile, glm::vec4 const& clearClr)
  : m_shaders{}, m_defaultShaders{}, 
  m_light{ { 0.f, 25.f, 0.f }, { 0.4f, 0.4f, 0.4f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
  m_material{ glm::vec3(1.f), glm::vec3(1.f), glm::vec3(1.f), 100.f },
  //mObjects{},
  m_leftClickHeld{ false }, m_leftClickTriggered{ true }, m_bvhModified{ true }, m_reconstructTree{ false }
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
  Graphics::Renderer::Init();



  std::shared_ptr<Graphics::MeshSource> cubeMeshSource = Graphics::MeshFactory::CreateCube();
  mesh0 = std::make_shared<Graphics::Mesh>(cubeMeshSource);

  std::shared_ptr<Graphics::MeshSource> pyrMeshSource = Graphics::MeshFactory::CreatePyramid();
  mesh1 = std::make_shared<Graphics::Mesh>(pyrMeshSource);
  //=====================================================================================================================

}

void Scene::Update(float deltaTime)
{
  // update transforms
    IGE::Physics::PhysicsSystem::GetInstance()->Update(deltaTime);
  for (auto& obj : mObjects)
  {
      obj->transform = obj->entity.GetComponent<Component::Transform>();
      obj->modified = true;
     obj->Update(deltaTime);
    
  }

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

  m_shaders.Use();

  m_cameras.front().SetUniforms(m_shaders);
  m_light.SetUniforms(m_shaders);
  m_material.SetUniforms(m_shaders);

  //for (auto& obj : m_objects)
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
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    Graphics::Renderer::RenderSceneBegin(mtx);

    // Imagine a 5x5 grid of cubes, each placed at different positions, scales, and colors
    for (int x = -2; x <= 2; ++x) {
        for (int y = -2; y <= 2; ++y) {
            // Calculate the position offset based on grid coordinates
            glm::vec3 position = glm::vec3(x * 6.0f, y * 6.0f, 0.0f); // Spacing the cubes 6 units apart

            // Random or incremental scaling
            glm::vec3 scale = glm::vec3(1.0f + (x + 2) * 0.5f, 1.0f + (y + 2) * 0.5f, 1.0f);

            // Assign a unique color for each cube (using normalized RGB values)
            glm::vec4 color = glm::vec4(
                (x + 2) * 0.2f, // Ranges from 0.0 to 1.0 as x goes from -2 to 2
                (y + 2) * 0.2f, // Same for Y axis
                1.0f - (x + 2) * 0.2f, // Creates a variety in color between cubes
                1.0f);  // Alpha is always 1 (fully opaque)

            // Submit the cube with the calculated position, scale, and color
            Graphics::Renderer::SubmitMesh(mesh1, position, scale, color, 0.0f);
            Graphics::Renderer::SubmitMesh(mesh0, position, scale, color, 0.0f);
        }
    }

    Graphics::Renderer::RenderSceneEnd();
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

  for (auto& obj : mObjects)
  {
    m_shaders.SetUniform("uMdlTransform", obj->mdlTransform);
    m_shaders.SetUniform("uVtxClr", obj->clr);
    auto const& mdl{ *obj->meshRef };
    glBindVertexArray(mdl.GetVAO());

    // if primitive type is a point, render with GL_POINTS
    if (mdl.isUsingIndices)
    {
      glDrawElements(mdl.primitiveType, static_cast<GLsizei>(mdl.drawCount), GL_UNSIGNED_SHORT, NULL);
    }
    // else draw as per normal
    else {
      glDrawArrays(mdl.primitiveType, 0, static_cast<GLsizei>(mdl.drawCount));
    }
  }
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
    mObjects.emplace_back(std::make_shared<Object>(
        "./assets/models/cube_low_poly.obj",
        xfm.worldPos, 
        xfm.worldScale));
    mObjects.back()->entity = entity;
    entity.EmplaceComponent<Component::Mesh>(Component::Mesh{});
}