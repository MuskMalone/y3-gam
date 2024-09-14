#include <pch.h>
#include "TempScene.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <filesystem>
#include <Physics/PhysicsSystem.h>
Scene::Scene(const char* vtxShaderFile, const char* fragShaderFile, glm::vec4 const& clearClr)
  : m_shaders{}, m_defaultShaders{}, 
  m_light{ { 0.f, 25.f, 0.f }, { 0.4f, 0.4f, 0.4f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
  m_material{ glm::vec3(1.f), glm::vec3(1.f), glm::vec3(1.f), 100.f }, m_cameras{},
  m_objects{},
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
  //m_objects.emplace_back(std::make_shared<Object>("./assets/models/bunny_high_poly.obj", glm::vec3(-5.f, 0.f, 0.f), glm::vec3(20.f)));
  //m_objects.emplace_back(std::make_shared<Object>("./assets/models/horse_high_poly.obj", glm::vec3(5.f), glm::vec3(10.f, 10.f, 10.f)));
  //m_objects.emplace_back(std::make_shared<Object>("./assets/models/teapot_mid_poly.obj", glm::vec3()));
  m_objects.emplace_back(std::make_shared<Object>("./assets/models/bunny_high_poly.obj", glm::vec3(-5.f, 0.f, 0.f), glm::vec3(20.f)));
  m_objects.emplace_back(std::make_shared<Object>("./assets/models/bunny_high_poly.obj", glm::vec3(5.f), glm::vec3(10.f, 10.f, 10.f)));
  m_objects.emplace_back(std::make_shared<Object>("./assets/models/bunny_high_poly.obj", glm::vec3()));


}


//IGE::Physics::PhysicsSystem ps{};
void Scene::Update(float deltaTime)
{
    {//this is for testing
        using namespace IGE;
        using namespace Physics;
        static bool firsttime = true;
        if (firsttime) {
            Physics::PhysicsSystem::InitAllocator();
            Physics::PhysicsSystem::GetInstance()->Init();
            firsttime = !firsttime;
        }
        Physics::PhysicsSystem::GetInstance()->Update(deltaTime, m_objects);
    }
  // update transforms
  for (auto& obj : m_objects)
  {
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

  for (auto& obj : m_objects)
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

  for (auto& obj : m_objects)
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
