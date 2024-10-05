#include <pch.h>
#include "MeshFactory.h"
#include "AssetIO/IMSH.h"

namespace Graphics {
  MeshFactory::MeshSourcePtr MeshFactory::CreateModelFromString(std::string const& meshName) {
    if (meshName == "Cube") {
      return CreateCube();
    }
    else if (meshName == "Plane") {
      return CreatePlane();
    }
    else if (meshName == "None") {
      return {};
    }

    Debug::DebugLogger::GetInstance().LogError("[MeshFactory] No such model exists: " + meshName);
    return {};
  }

  MeshFactory::MeshSourcePtr MeshFactory::CreateCube() {
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
    return std::make_shared<MeshSource>(vao, submeshes, cubeVertices, cubeIndices);
  }

  MeshFactory::MeshSourcePtr MeshFactory::CreatePlane() {
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
    return std::make_shared<MeshSource>(vao, submeshes, planeVertices, planeIndices);
  }

  MeshFactory::MeshSourcePtr MeshFactory::CreatePyramid() {
    std::vector<Vertex> pyramidVertices{
      // Base (square)
      {{-0.5f, 0.0f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {}, {}},  // Bottom-left
      {{ 0.5f, 0.0f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {}, {}},  // Bottom-right
      {{ 0.5f, 0.0f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {}, {}},  // Top-right
      {{-0.5f, 0.0f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {}, {}},  // Top-left

      // Apex
      {{ 0.0f,  0.75f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 1.0f}, {}, {}}  // Apex (top)
    };

    // Pyramid indices
    std::vector<uint32_t> pyramidIndices = {
      // Base (square)
      0, 1, 2, 2, 3, 0,       // Base face

      // Faces (triangles)
      0, 1, 4,   // Front face
      1, 2, 4,   // Right face
      2, 3, 4,   // Back face
      3, 0, 4    // Left face
    };

    // Create VAO and VBO
    auto vao = VertexArray::Create();
    auto vbo = VertexBuffer::Create(sizeof(pyramidVertices));

    BufferLayout pyramidLayout = {
        {AttributeType::VEC3, "a_Position"},
        {AttributeType::VEC3, "a_Normal"},
        {AttributeType::VEC2, "a_TexCoord"},
        {AttributeType::FLOAT, "a_TexIdx"},
        {AttributeType::VEC3, "a_Tangent"},
        {AttributeType::VEC3, "a_Bitangent"},
        {AttributeType::VEC4, "a_Color"},
    };

    vbo->SetLayout(pyramidLayout);
    vao->AddVertexBuffer(vbo);

    // Create and bind Element Buffer Object (EBO) for the indices
    std::shared_ptr<ElementBuffer> ebo = ElementBuffer::Create(pyramidIndices.data(), static_cast<uint32_t>(pyramidIndices.size()));
    vao->SetElementBuffer(ebo);

    // Set up submesh
    std::vector<Submesh> submeshes;
    Submesh pyramidSubmesh{
        0,                                 // baseVtx
        0,                                 // baseIdx
        static_cast<uint32_t>(pyramidVertices.size()),   // vtxCount
        static_cast<uint32_t>(pyramidIndices.size()),    // idxCount
        0,                                 // materialIdx
        glm::mat4(1.0f),                   // Identity matrix for transform
        pyramidIndices                     // Indices for the submesh
    };

    submeshes.push_back(pyramidSubmesh);

    // Create MeshSource with the generated data
    return std::make_shared<MeshSource>(vao, submeshes, pyramidVertices, pyramidIndices);
  }

  MeshFactory::MeshSourcePtr MeshFactory::CreateModelFromImport(std::string const& imshFile) {
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
