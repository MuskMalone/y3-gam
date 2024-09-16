#include <pch.h>
#include "TempScene.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <filesystem>
#include "Graphics/Renderer.h"
#include "Graphics/MeshSource.h"

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
  m_defaultShaders.CompileShaderFile("./shaders/Framework.vert.glsl", "./shaders/Framework.frag.glsl");
  m_cameras.emplace_back(WINDOW_WIDTH<int>, WINDOW_HEIGHT<int>, glm::vec3(3.f, 3.f, 15.f));
  m_cameras.emplace_back(WINDOW_WIDTH<int>, WINDOW_HEIGHT<int>, glm::vec3(0.f, 15.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(1.f, 0.f, 0.f), false);
}

void Scene::Init()
{
    Graphics::Renderer::Init();
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
  m_objects.emplace_back(std::make_shared<Object>("./assets/models/bunny_high_poly.obj", glm::vec3(-5.f, 0.f, 0.f), glm::vec3(30.f)));
  m_objects.emplace_back(std::make_shared<Object>("./assets/models/horse_high_poly.obj", glm::vec3(5.f), glm::vec3(30.f)));
  m_objects.emplace_back(std::make_shared<Object>("./assets/models/teapot_mid_poly.obj", glm::vec3(), glm::vec3(1.f)));

  //TEMP CODE===============================================================
  std::vector<Vertex> cubeVertices {
      // Front face
      { {-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}, {}, {}, {0.0f, 0.0f}},
      {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}, {}, {}, {1.0f, 0.0f}},
      {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f,  1.0f},{}, {}, {1.0f, 1.0f}},
      {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f,  1.0f},{}, {}, {0.0f, 1.0f}},

      // Back face
      {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {}, {},{1.0f, 0.0f}},
      {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},{}, {}, {0.0f, 0.0f}},
      {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {}, {},{0.0f, 1.0f}},
      {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},{}, {}, {1.0f, 1.0f}}
  };

  std::vector<uint32_t> cubeIndices = {
      // Front face
      0, 1, 2, 2, 3, 0,
      // Back face
      4, 5, 6, 6, 7, 4,
      // Left face
      4, 0, 3, 3, 7, 4,
      // Right face
      1, 5, 6, 6, 2, 1,
      // Top face
      3, 2, 6, 6, 7, 3,
      // Bottom face
      4, 5, 1, 1, 0, 4
  };

  using namespace Graphics;

  //Cubes
  auto vao = VertexArray::Create();
  auto vbo = VertexBuffer::Create(sizeof(cubeVertices));

  BufferLayout cubeLayout = {
      {AttributeType::VEC3, "a_Position"},
      {AttributeType::VEC3, "a_Normal"},
      {AttributeType::VEC2, "a_TexCoord"},
      {AttributeType::FLOAT, "a_TexIdx"},
      {AttributeType::VEC3, "a_Tangent"},
      {AttributeType::VEC3, "a_Bitangent"},
      {AttributeType::VEC4, "a_Color"},
  };

  vbo->SetLayout(cubeLayout);
  vao->AddVertexBuffer(vbo);

  std::shared_ptr<ElementBuffer> cubeEbo = ElementBuffer::Create(cubeIndices.data(), 36);
  vao->SetElementBuffer(cubeEbo);

  std::vector<Graphics::Submesh> submeshes;

  Graphics::Submesh cubeSubmesh{
    0,                                // baseVtx starts at 0
    0,                                // baseIdx starts at 0
    static_cast<uint32_t>(cubeVertices.size()),   // vtxCount = total number of vertices
    static_cast<uint32_t>(cubeIndices.size()),    // idxCount = total number of indices
    0,                                // materialIdx, assume 0 for now
    glm::mat4(1.0f)                   // identity matrix for submesh transform
  };

  submeshes.push_back(cubeSubmesh);
  auto cubeMeshSource = std::make_shared<MeshSource>(
      vao,                            // Vertex array object
      submeshes,                       // Submeshes
      cubeVertices,                    // Vertex data
      cubeIndices                      // Index data
  );

  mesh0 = Graphics::Mesh{ cubeMeshSource };
  //=====================================================================================================================

}

void Scene::Update(float deltaTime)
{
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

    Graphics::Renderer::DrawQuad({-1.f,0.f,0.f}, {0.5f,0.5f}, glm::vec4{0.9f,0.2f,0.8f,1.f});
    Graphics::Renderer::DrawQuad({ 1.f,0.f,0.f }, { 0.5f,0.5f }, glm::vec4{ 0.2f,1.f,0.8f,1.f });
    

    Graphics::Renderer::SubmitMesh( mesh0 , { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, 0.0f);

    //Graphics::Renderer::SubmitCube(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(5.0f, 5.0f, 5.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f);

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
    //        Graphics::Renderer::SubmitCube(position, scale, color, 0.0f);
    //    }
    //}

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
