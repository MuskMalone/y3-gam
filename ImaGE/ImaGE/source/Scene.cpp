#include <pch.h>
#include "Scene.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <filesystem>
#include <BoundingVolumes/BoundingSphere.h>
#include <BVH/VolumeBounder.h>

Scene::Scene(const char* vtxShaderFile, const char* fragShaderFile, glm::vec4 const& clearClr)
  : m_shaders{}, m_defaultShaders{}, 
  m_light{ { 0.f, 25.f, 0.f }, { 0.4f, 0.4f, 0.4f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
  m_material{ glm::vec3(1.f), glm::vec3(1.f), glm::vec3(1.f), 100.f }, m_cameras{},
  m_objects{}, m_bvh{}, m_octree{}, m_bsp{},
  m_leftClickHeld{ false }, m_leftClickTriggered{ true }, m_bvhModified{ true }, m_reconstructTree{ false }
{
  Settings::Init(clearClr, 1.f);
  if (Settings::enableDepthBuffer) {
    glEnable(GL_DEPTH_TEST);
  }

  if (Settings::backfaceCulling) {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
  }

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  m_shaders.CompileShaderFile(vtxShaderFile, fragShaderFile);
  m_defaultShaders.CompileShaderFile("./shaders/Framework.vert.glsl", "./shaders/Framework.frag.glsl");
  m_cameras.emplace_back(WINDOW_WIDTH<int>, WINDOW_HEIGHT<int>, glm::vec3(3.f, 3.f, 15.f));
  m_cameras.emplace_back(WINDOW_WIDTH<int>, WINDOW_HEIGHT<int>, glm::vec3(0.f, 30.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(1.f, 0.f, 0.f), false);
}

Scene::~Scene()
{
  m_bsp.SaveTree("bsptree.json");
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

  // POWERPLANT MODELS
  //LoadMultiPartModel("./assets/Section4.txt", glm::vec3(0.0001f, 0.0001f, 0.0001f));
  //LoadMultiPartModel("./assets/Section5.txt", glm::vec3(0.0001f, 0.0001f, 0.0001f));
  //LoadMultiPartModel("./assets/Section6.txt", glm::vec3(0.0001f, 0.0001f, 0.0001f));

  // OTHER MODELS
  m_objects.emplace_back(std::make_shared<Object>("./assets/models/menger_sponge_level_1_low_poly.obj", BV::BVType::AABB, glm::vec3(1), glm::vec3(20.f)));
  m_objects.emplace_back(std::make_shared<Object>("./assets/models/cube_low_poly.obj", BV::BVType::AABB, glm::vec3(1), glm::vec3(20.f)));

  if (Settings::currDataStructure == Settings::DataStructure::BVH) {
    m_bvh.Construct(m_objects, BV::BVHierarchy::Type::TOP_DOWN);
  }

  m_octree.BuildTree(m_objects);
  m_bsp.BuildTree(m_objects);
  //m_bsp.LoadTree("bsptree.json");
}

void Scene::Update(float deltaTime)
{
  // update transforms
  for (auto& obj : m_objects)
  {
    obj->Update(deltaTime);
  }

  if (Settings::currDataStructure == Settings::DataStructure::BVH && Settings::showTree && m_bvhModified) {
    ReconstructBVH();
  }
  else if (m_reconstructTree)
  {
    if (Settings::currDataStructure == Settings::DataStructure::OCTREE) {
      m_octree.BuildTree(m_objects);
    }
    else {
      m_bsp.BuildTree(m_objects);
    }
    m_reconstructTree = false;
  }

  // update camera
  for (auto& cam : m_cameras)
  {
    cam.Update(deltaTime);
  }
}

void Scene::ReconstructBVH()
{
  m_bvh.Reconstruct(m_objects);
  m_bvhModified = false;
}

void Scene::ChangeConstructionMethod(BV::BVHierarchy::Type type)
{
  m_bvh.Construct(m_objects, type);
  m_bvhModified = false;
}

void Scene::Draw()
{
  glClear(GL_COLOR_BUFFER_BIT);
  if (Settings::enableDepthBuffer) {
    glClear(GL_DEPTH_BUFFER_BIT);
  }

  m_shaders.Use();

  m_cameras.front().SetUniforms(m_shaders);
  m_light.SetUniforms(m_shaders);
  m_material.SetUniforms(m_shaders);

  if (!Settings::showTree)
  {
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
  }
  m_shaders.Unuse();
  
  m_defaultShaders.Use();
  m_cameras.front().SetUniforms(m_defaultShaders);
  // draw bounding volumes with lines, then revert back to current mode
  auto const prevMode{ Settings::renderMode };
  Settings::SetRenderMode(Settings::RenderMode::WIREFRAME);
  if (Settings::showBoundingVol)
  {
    for (auto& obj : m_objects)
    {
      obj->collider->Draw(m_defaultShaders);
    }
  }

  if (Settings::showTree)
  {
    if (Settings::currDataStructure == Settings::DataStructure::BVH) {
      m_bvh.Draw(m_defaultShaders);
    }
    else if (Settings::currDataStructure == Settings::DataStructure::OCTREE) {
      m_octree.Draw(m_defaultShaders);
    }
    else if (Settings::currDataStructure == Settings::DataStructure::BSPTREE) {
      m_bsp.Draw(m_defaultShaders);
    }
  }
  Settings::SetRenderMode(prevMode);
  m_defaultShaders.Unuse();

  glBindVertexArray(0);
}

void Scene::DrawTopView()
{
  glClear(GL_COLOR_BUFFER_BIT);
  if (Settings::enableDepthBuffer) {
    glClear(GL_DEPTH_BUFFER_BIT);
  }

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
  // draw bounding volumes with lines, then revert back to current mode
  auto const prevMode{ Settings::renderMode };
  Settings::SetRenderMode(Settings::RenderMode::WIREFRAME);
  if (Settings::showBoundingVol)
  {
    for (auto& obj : m_objects)
    {
      obj->collider->Draw(m_defaultShaders);
    }
  }

  if (Settings::showTree)
  {
    if (Settings::currDataStructure == Settings::DataStructure::BVH) {
      m_bvh.Draw(m_defaultShaders);
    }
    else if (Settings::currDataStructure == Settings::DataStructure::OCTREE)
    {
      m_octree.Draw(m_defaultShaders);
    }
    else if (Settings::currDataStructure == Settings::DataStructure::BSPTREE)
    {
      m_bsp.Draw(m_defaultShaders);
    }
  }
  Settings::SetRenderMode(prevMode);
  m_defaultShaders.Unuse();

  glBindVertexArray(0);
}

void Scene::LoadMultiPartModel(const char* path, glm::vec3 const& scale, BV::BVType type)
{
  std::ifstream ifs{ path };
  if (!ifs) { throw std::runtime_error("Unable to read " + std::string(path)); }

  std::string file;
  std::filesystem::path currPath{ path };
  std::string const parentPath{ currPath.parent_path().string() };
  while (ifs)
  {
    ifs >> file;
    m_objects.emplace_back(std::make_shared<Object>(parentPath + "/" + file, type, glm::vec3(), scale));
  }
  ifs.close();
}

void Scene::RandomizeBVInitialPoints()
{
  for (auto& obj : m_objects) {
    auto bs{ std::dynamic_pointer_cast<BV::BoundingSphere>(obj->collider) };
    bs->reRandomizePoints = true;
    obj->modified = true;
  }
  RecomputeBVH();
}

void Scene::ChangeAllBVs(BV::BVType type)
{
  Settings::bvType = type;
  for (auto& obj : m_objects) { obj->ChangeBV(type); }
  RecomputeBVH();
}

void Scene::RecomputeAllBVs()
{
  for (auto const& obj : m_objects) { obj->modified = true; }
  RecomputeBVH();
}

void Scene::ResetCamera()
{
  m_cameras.front().Reset(glm::vec3(3.f, 3.f, 15.f), glm::vec3());
}
