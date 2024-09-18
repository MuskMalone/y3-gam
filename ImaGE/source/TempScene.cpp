#include <pch.h>
#include "TempScene.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <filesystem>
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
  InputAssistant::RegisterKeyPressEvent(GLFW_KEY_W, std::bind(&Camera::Forward, &m_cameras.front()));
  InputAssistant::RegisterKeyPressEvent(GLFW_KEY_S, std::bind(&Camera::Backward, &m_cameras.front()));
  InputAssistant::RegisterKeyPressEvent(GLFW_KEY_D, std::bind(&Camera::Right, &m_cameras.front()));
  InputAssistant::RegisterKeyPressEvent(GLFW_KEY_A, std::bind(&Camera::Left, &m_cameras.front()));
  InputAssistant::RegisterKeyPressEvent(GLFW_MOUSE_BUTTON_1, std::bind(&Scene::StartPanning, this));
  InputAssistant::RegisterKeyReleaseEvent(GLFW_MOUSE_BUTTON_1, std::bind(&Scene::EndPanning, this));

  InputAssistant::RegisterCursorEvent([this](double x, double y) {
    static glm::dvec2 prevPos{ x, y };

    if (!m_leftClickHeld) {
      prevPos = { x, y }; // Update prevPos when the left click is not held
      return;
    }
    if (m_leftClickTriggered)
    {
      prevPos = { x, y };
      m_leftClickTriggered = false;
    }
    else
    {
      m_cameras.front().onCursor(x - prevPos.x, y - prevPos.y);
      prevPos = { x, y }; // Update prevPos after calling onCursor
    }
  });

  // OTHER MODELS
  //mObjects.emplace_back(std::make_shared<Object>("./assets/models/bunny_high_poly.obj", glm::vec3(-5.f, 0.f, 0.f), glm::vec3(30.f)));
  //mObjects.emplace_back(std::make_shared<Object>("./assets/models/horse_high_poly.obj", glm::vec3(5.f), glm::vec3(30.f)));
  //mObjects.emplace_back(std::make_shared<Object>("./assets/models/teapot_mid_poly.obj", glm::vec3(), glm::vec3(1.f)));
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
  m_cameras.front().SetUniforms(m_defaultShaders);
  m_defaultShaders.Unuse();

  glBindVertexArray(0);
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