#include <pch.h>
#include "MeshFactory.h"
#include "AssetIO/IMSH.h"

namespace Graphics {
  MeshSource MeshFactory::CreateModelFromString(std::string const& meshName) {
    if (meshName == "Cube") {
      return CreateCube();
    }
    else if (meshName == "Plane") {
      return CreatePlane();
    }
    else if (meshName == "None") {
        return MeshSource{ {},{},{},{} };
    }

    throw Debug::Exception<MeshFactory>(Debug::LVL_ERROR, Msg("[MeshFactory] No such model exists: " + meshName));
  }

  MeshSource MeshFactory::CreateCube() {
    std::vector<Vertex> cubeVertices{
      // Front face
      {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // Bottom-left
      {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // Bottom-right
      {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // Top-right
      {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // Top-left

      // Back face
      {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // Bottom-right
      {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // Bottom-left
      {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // Top-left
      {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // Top-right

      // Left face
      {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},  // Bottom-back
      {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},  // Bottom-front
      {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},  // Top-front
      {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},  // Top-back

      // Right face
      {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},  // Bottom-front
      {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},  // Bottom-back
      {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},  // Top-back
      {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},  // Top-front

      // Top face
      {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},  // Front-left
      {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},  // Front-right
      {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},  // Back-right
      {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},  // Back-left

      // Bottom face
      {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},  // Back-left
      {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},  // Back-right
      {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},  // Front-right
      {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}}   // Front-left
    };

    // Cube indices
    std::vector<uint32_t> cubeIndices = {
        0, 1, 2, 2, 3, 0,       // Front face
        4, 5, 6, 6, 7, 4,       // Back face
        8, 9, 10, 10, 11, 8,    // Left face
        12, 13, 14, 14, 15, 12, // Right face
        16, 17, 18, 18, 19, 16, // Top face
        20, 21, 22, 22, 23, 20  // Bottom face
    };

    // Create VAO and VBO
    auto vao = VertexArray::Create();
    auto vbo = VertexBuffer::Create(static_cast<unsigned>(cubeVertices.size() * sizeof(Vertex)));

    // Set vertex buffer data
    vbo->SetData(cubeVertices.data(), static_cast<unsigned>(cubeVertices.size() * sizeof(Vertex)));

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

    // Create and bind Element Buffer Object (EBO) for the indices
    std::shared_ptr<ElementBuffer> ebo = ElementBuffer::Create(cubeIndices.data(), static_cast<uint32_t>(cubeIndices.size()));
    vao->SetElementBuffer(ebo);

    // Set up submesh
    std::vector<Submesh> submeshes;
    Submesh cubeSubmesh{
        0,                                 // baseVtx
        0,                                 // baseIdx
        static_cast<uint32_t>(cubeVertices.size()),   // vtxCount
        static_cast<uint32_t>(cubeIndices.size()),    // idxCount
        0,                                 // materialIdx
        glm::mat4(1.0f),                   // Identity matrix for transform
        cubeIndices                        // Indices for the submesh
    };

    submeshes.push_back(cubeSubmesh);

    // Create MeshSource with the generated data
    return MeshSource(vao, submeshes, cubeVertices, cubeIndices);
  }

  MeshSource MeshFactory::CreatePlane() {
    std::vector<Vertex> planeVertices{
      // front
      {{-0.5f,  0.f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
      {{ 0.5f,  0.f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
      {{ 0.5f,  0.f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
      {{-0.5f,  0.f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},

      // back
      { { -0.5f, 0.f,  0.5f }, { 0.f, -1.f, 0.f }, { 0.f, 0.f }, { -1.f, 0.f, 0.f }, { 0.f, 0.f, -1.f } },
      { {  0.5f, 0.f,  0.5f }, { 0.f, -1.f, 0.f }, { 1.f, 0.f }, { -1.f, 0.f, 0.f }, { 0.f, 0.f, -1.f } },
      { {  0.5f, 0.f, -0.5f }, { 0.f, -1.f, 0.f }, { 1.f, 1.f }, { -1.f, 0.f, 0.f }, { 0.f, 0.f, -1.f } },
      { { -0.5f, 0.f, -0.5f }, { 0.f, -1.f, 0.f }, { 0.f, 1.f }, { -1.f, 0.f, 0.f }, { 0.f, 0.f, -1.f } }
    };

    std::vector<uint32_t> planeIndices = {
      // front
      0, 1, 2,
      0, 2, 3,

      // back
      4, 6, 5,
      4, 7, 6,
    };

    // Create VAO and VBO
    auto vao = VertexArray::Create();
    auto vbo = VertexBuffer::Create(static_cast<unsigned>(planeVertices.size() * sizeof(Vertex)));

    // Set vertex buffer data
    vbo->SetData(planeVertices.data(), static_cast<unsigned>(planeVertices.size() * sizeof(Vertex)));

    BufferLayout planeLayout = {
        {AttributeType::VEC3, "a_Position"},
        {AttributeType::VEC3, "a_Normal"},
        {AttributeType::VEC2, "a_TexCoord"},
        {AttributeType::FLOAT, "a_TexIdx"},
        {AttributeType::VEC3, "a_Tangent"},
        {AttributeType::VEC3, "a_Bitangent"},
        {AttributeType::VEC4, "a_Color"},
    };

    vbo->SetLayout(planeLayout);
    vao->AddVertexBuffer(vbo);

    // Create and bind Element Buffer Object (EBO) for the indices
    std::shared_ptr<ElementBuffer> ebo = ElementBuffer::Create(planeIndices.data(), static_cast<uint32_t>(planeIndices.size()));
    vao->SetElementBuffer(ebo);

    // Set up submesh
    std::vector<Submesh> submeshes;
    Submesh planeSubmesh{
        0,                                 // baseVtx
        0,                                 // baseIdx
        static_cast<uint32_t>(planeVertices.size()),   // vtxCount
        static_cast<uint32_t>(planeIndices.size()),    // idxCount
        0,                                 // materialIdx
        glm::mat4(1.0f),                   // Identity matrix for transform
        planeIndices                       // Indices for the submesh
    };

    submeshes.push_back(planeSubmesh);

    // Create MeshSource with the generated data
    return MeshSource(vao, submeshes, planeVertices, planeIndices);
  }

  MeshSource MeshFactory::CreateCapsule() {
      std::vector<Vertex> capsuleVertices;

      const int radialSegments = 16; // Number of segments for the rounded ends
      const int heightSegments = 8; // Number of segments along the height of the cylinder
      const float radius = 1.0f; // Radius of the unit capsule
      const float halfHeight = 0.5f; // Half of the total height (1 unit in total)

      // Generate the hemispherical caps
      for (int i = 0; i <= radialSegments; ++i) {
          float theta = i * glm::two_pi<float>() / radialSegments;
          float x = radius * cos(theta);
          float z = radius * sin(theta);

          // Top hemisphere vertices
          capsuleVertices.push_back({ {x, halfHeight, z}, {0, 1, 0}, {0, 0}, {1, 0, 0}, {0, 1, 0} });

          // Bottom hemisphere vertices
          capsuleVertices.push_back({ {x, -halfHeight, z}, {0, -1, 0}, {0, 1}, {1, 0, 0}, {0, 1, 0} });
      }

      // Generate the cylinder sides
      for (int j = 0; j <= heightSegments; ++j) {
          float y = -halfHeight + (j * (1.0f / heightSegments)); // Height range from -0.5 to 0.5
          for (int i = 0; i <= radialSegments; ++i) {
              float theta = i * glm::two_pi<float>() / radialSegments;
              float x = radius * cos(theta);
              float z = radius * sin(theta);
              capsuleVertices.push_back({ {x, y, z}, {0, 0, 1}, {static_cast<float>(i) / radialSegments, static_cast<float>(j) / heightSegments}, {1, 0, 0}, {0, 1, 0} });
          }

          // Create indices for the hemispherical caps
          std::vector<uint32_t> capsuleIndices;

          // Top hemisphere
          for (int i = 0; i < radialSegments; ++i) {
              capsuleIndices.push_back(i);
              capsuleIndices.push_back(i + 1);
              capsuleIndices.push_back(radialSegments + 1 + i);

              capsuleIndices.push_back(i + 1);
              capsuleIndices.push_back(radialSegments + 1 + i + 1);
              capsuleIndices.push_back(radialSegments + 1 + i);
          }

          // Cylinder sides
          for (int j = 0; j < heightSegments; ++j) {
              for (int i = 0; i < radialSegments; ++i) {
                  int first = (j * (radialSegments + 1)) + i;
                  int second = first + radialSegments + 1;

                  capsuleIndices.push_back(first);
                  capsuleIndices.push_back(second);
                  capsuleIndices.push_back(first + 1);

                  capsuleIndices.push_back(second);
                  capsuleIndices.push_back(second + 1);
                  capsuleIndices.push_back(first + 1);
              }
          }

          // Create VAO and VBO for the capsule
          auto vao = VertexArray::Create();
          auto vbo = VertexBuffer::Create(static_cast<unsigned>(capsuleVertices.size() * sizeof(Vertex)));

          // Set vertex buffer data
          vbo->SetData(capsuleVertices.data(), static_cast<unsigned>(capsuleVertices.size() * sizeof(Vertex)));

          BufferLayout capsuleLayout = {
              {AttributeType::VEC3, "a_Position"},
              {AttributeType::VEC3, "a_Normal"},
              {AttributeType::VEC2, "a_TexCoord"},
              {AttributeType::FLOAT, "a_TexIdx"},
              {AttributeType::VEC3, "a_Tangent"},
              {AttributeType::VEC3, "a_Bitangent"},
              {AttributeType::VEC4, "a_Color"},
          };

          vbo->SetLayout(capsuleLayout);
          vao->AddVertexBuffer(vbo);

          // Create and bind Element Buffer Object (EBO) for the indices
          auto ebo = ElementBuffer::Create(capsuleIndices.data(), static_cast<uint32_t>(capsuleIndices.size()));
          vao->SetElementBuffer(ebo);

          // Set up submesh
          std::vector<Submesh> submeshes;
          Submesh capsuleSubmesh{
              0,                                 // baseVtx
              0,                                 // baseIdx
              static_cast<uint32_t>(capsuleVertices.size()),   // vtxCount
              static_cast<uint32_t>(capsuleIndices.size()),    // idxCount
              0,                                 // materialIdx
              glm::mat4(1.0f),                   // Identity matrix for transform
              capsuleIndices                        // Indices for the submesh
          };

          submeshes.push_back(capsuleSubmesh);

          // Create MeshSource with the generated data
          return MeshSource(vao, submeshes, capsuleVertices, capsuleIndices);
      }
  }

  MeshSource MeshFactory::CreateModelFromImport(std::string const& imshFile) {
    AssetIO::IMSH imsh{};
    imsh.ReadFromBinFile(imshFile);
    if (!imsh) { Debug::DebugLogger::GetInstance().LogError("Unable to read binary file: " + imshFile); }

    // Create VAO and VBO
    auto vao = VertexArray::Create();
    auto vbo = VertexBuffer::Create(static_cast<unsigned>(imsh.GetVertexBuffer().size() * sizeof(Vertex)));

    // Set vertex buffer data
    auto vtxBufferCpy{ imsh.GetVertexBuffer() };

    vbo->SetData(vtxBufferCpy.data(), static_cast<unsigned>(vtxBufferCpy.size() * sizeof(Vertex)));

    BufferLayout modelLayout = {
        {AttributeType::VEC3, "a_Position"},
        {AttributeType::VEC3, "a_Normal"},
        {AttributeType::VEC2, "a_TexCoord"},
        {AttributeType::FLOAT, "a_TexIdx"},
        {AttributeType::VEC3, "a_Tangent"},
        {AttributeType::VEC3, "a_Bitangent"},
        {AttributeType::VEC4, "a_Color"},
    };

    vbo->SetLayout(modelLayout);
    vao->AddVertexBuffer(vbo);

    // Create and bind Element Buffer Object (EBO) for the indices
    std::shared_ptr<ElementBuffer> ebo = ElementBuffer::Create(imsh.GetIndices().data(), static_cast<uint32_t>(imsh.GetIndices().size()));
    vao->SetElementBuffer(ebo);

    return imsh.ToMeshSource(vao);
  }
}
