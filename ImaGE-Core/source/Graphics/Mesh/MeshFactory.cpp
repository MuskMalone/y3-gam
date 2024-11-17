#include <pch.h>
#include "MeshFactory.h"
#include <Graphics/Mesh/IMSH.h>

namespace Graphics {
  MeshSource MeshFactory::CreateModelFromString(std::string const& meshName) {
    if (meshName == "Cube") {
      return CreateCube();
    }
    else if (meshName == "Plane") {
      return CreatePlane();
    }
    else if (meshName == "Quad") {
        return CreateQuad();
    }
    else if (meshName == "Sphere") {
        return CreateSphere(); //default 32 slices and stacks
    }
    else if (meshName == "Capsule") {
        return CreateCapsule();
    }
    else if (meshName == "None") {
        return MeshSource{ {},{},{},{} };
    }

    throw Debug::Exception<MeshFactory>(Debug::LVL_ERROR, Msg("[MeshFactory] No such model exists: " + meshName));
  }

  MeshSource MeshFactory::CreateQuad() {
      std::vector<Vertex2D> quadVertices{
          {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},  // Bottom-left
          {{ 0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},  // Bottom-right
          {{ 0.5f,  0.5f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},  // Top-right
          {{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}   // Top-left
      };

      std::vector<uint32_t> quadIndices = {
          0, 1, 2,  // First triangle
          2, 3, 0   // Second triangle
      };

      // Create VAO and VBO
      auto vao = VertexArray::Create();
      auto vbo = VertexBuffer::Create(static_cast<unsigned>(quadVertices.size() * sizeof(Vertex2D)));

      // Set vertex buffer data to quadVertices2D
      vbo->SetData(quadVertices.data(), static_cast<unsigned>(quadVertices.size() * sizeof(Vertex2D)));

      // Define the layout only for position, texcoord, and color
      BufferLayout quadLayout = {
          {AttributeType::VEC3, "a_Position"},
          {AttributeType::VEC2, "a_TexCoord"},
          {AttributeType::VEC4, "a_Color"},
      };

      vbo->SetLayout(quadLayout);
      vao->AddVertexBuffer(vbo);

      // Create and bind Element Buffer Object (EBO) for the indices
      auto ebo = ElementBuffer::Create(quadIndices.data(), static_cast<uint32_t>(quadIndices.size()));
      vao->SetElementBuffer(ebo);

      // Set up submesh
      std::vector<Submesh> submeshes;
      Submesh quadSubmesh{
          0,                                 // baseVtx
          0,                                 // baseIdx
          static_cast<uint32_t>(quadVertices.size()),   // vtxCount
          static_cast<uint32_t>(quadIndices.size()),    // idxCount
          0,                                 // materialIdx
          glm::mat4(1.0f),                   // Identity matrix for transform
          quadIndices                        // Indices for the submesh
      };

      submeshes.push_back(quadSubmesh);

      // Create MeshSource with the generated data
      return MeshSource(vao, submeshes, {}, quadIndices);  // Empty vertices list, since we use Vertex2D locally
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

  MeshSource MeshFactory::CreateSphere(uint32_t stacks, uint32_t slices){
      std::vector<Vertex> sphereVertices;
      std::vector<uint32_t> sphereIndices;

      // Generate vertices
      for (uint32_t stack = 0; stack <= stacks; ++stack) {
          float phi = glm::pi<float>() * stack / stacks; // Latitude angle
          float y = glm::cos(phi);                      // Y-coordinate
          float radius = glm::sin(phi);                 // Radius of the current ring

          for (uint32_t slice = 0; slice <= slices; ++slice) {
              float theta = glm::two_pi<float>() * slice / slices; // Longitude angle
              float x = radius * glm::cos(theta);
              float z = radius * glm::sin(theta);

              glm::vec3 position = { x, y, z };
              glm::vec3 normal = glm::normalize(position);
              glm::vec2 texCoord = { static_cast<float>(slice) / slices, static_cast<float>(stack) / stacks };

              // Add vertex
              sphereVertices.push_back({ position, normal, texCoord, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} });
          }
      }

      // Generate indices
      for (uint32_t stack = 0; stack < stacks; ++stack) {
          for (uint32_t slice = 0; slice < slices; ++slice) {
              uint32_t first = stack * (slices + 1) + slice;
              uint32_t second = first + slices + 1;

              // Triangle 1
              sphereIndices.push_back(first + 1);
              sphereIndices.push_back(second);
              sphereIndices.push_back(first);

              // Triangle 2
              sphereIndices.push_back(first + 1);
              sphereIndices.push_back(second + 1);
              sphereIndices.push_back(second);

          }
      }

      // Create VAO and VBO
      auto vao = VertexArray::Create();
      auto vbo = VertexBuffer::Create(static_cast<unsigned>(sphereVertices.size() * sizeof(Vertex)));

      // Set vertex buffer data
      vbo->SetData(sphereVertices.data(), static_cast<unsigned>(sphereVertices.size() * sizeof(Vertex)));

      BufferLayout sphereLayout = {
          {AttributeType::VEC3, "a_Position"},
          {AttributeType::VEC3, "a_Normal"},
          {AttributeType::VEC2, "a_TexCoord"},
          {AttributeType::FLOAT, "a_TexIdx"},
          {AttributeType::VEC3, "a_Tangent"},
          {AttributeType::VEC3, "a_Bitangent"},
          {AttributeType::VEC4, "a_Color"},
      };

      vbo->SetLayout(sphereLayout);
      vao->AddVertexBuffer(vbo);

      // Create and bind Element Buffer Object (EBO) for the indices
      std::shared_ptr<ElementBuffer> ebo = ElementBuffer::Create(sphereIndices.data(), static_cast<uint32_t>(sphereIndices.size()));
      vao->SetElementBuffer(ebo);

      // Set up submesh
      std::vector<Submesh> submeshes;
      Submesh sphereSubmesh{
          0,                                      // baseVtx
          0,                                      // baseIdx
          static_cast<uint32_t>(sphereVertices.size()), // vtxCount
          static_cast<uint32_t>(sphereIndices.size()),  // idxCount
          0,                                      // materialIdx
          glm::mat4(1.0f),                        // Identity matrix for transform
          sphereIndices                           // Indices for the submesh
      };

      submeshes.push_back(sphereSubmesh);
      return MeshSource(vao, submeshes, sphereVertices, sphereIndices);
  }

  MeshSource MeshFactory::CreateCapsule(float radius, float height, int stacks, int slices) {
      std::vector<Vertex> vertices;
      std::vector<uint32_t> indices;

      // Top hemisphere
      for (int i = 0; i <= stacks; ++i) {
          float phi = glm::pi<float>() / 2 * (1.0f - float(i) / stacks); // From π/2 to 0
          float y = radius * sin(phi) + height / 2.0f; // Offset by half height
          float r = radius * cos(phi); // Radius of the circle at this stack

          for (int j = 0; j <= slices; ++j) {
              float theta = glm::two_pi<float>() * j / slices; // Angle around the circle
              float x = r * cos(theta);
              float z = r * sin(theta);

              vertices.push_back({
                  {x, y, z},                                 // Position
                  glm::normalize(glm::vec3(x, y - height / 2.0f, z)), // Normal (corrected reference point)
                  {float(j) / slices, 1.0f - float(i) / stacks},      // UV (v-flipped for consistency)
                  {1.0f, 0.0f, 0.0f},                        // Tangent
                  {0.0f, 1.0f, 0.0f},                        // Bitangent
                  {1.0f, 1.0f, 1.0f, 1.0f}                   // Color
                  });
          }
      }

      // Cylinder
      for (int i = 0; i <= stacks; ++i) {
          float y = height / 2.0f - float(i) / stacks * height; // Linear interpolation from top to bottom
      
          for (int j = 0; j <= slices; ++j) {
              float theta = glm::two_pi<float>() * j / slices;
              float x = radius * cos(theta);
              float z = radius * sin(theta);
      
              vertices.push_back({
                  {x, y, z},                        // Position
                  glm::normalize(glm::vec3(x, 0.0f, z)), // Normal (aligned to the radius)
                  {float(j) / slices, float(i) / stacks}, // UV (consistent with top/bottom)
                  {1.0f, 0.0f, 0.0f},               // Tangent
                  {0.0f, 1.0f, 0.0f},               // Bitangent
                  {1.0f, 1.0f, 1.0f, 1.0f}          // Color
              });
          }
      }

      // Bottom hemisphere
      for (int i = 0; i <= stacks; ++i) {
          float phi = glm::pi<float>() / 2 * ((float)i / stacks - 1.0f); // From 0 to -π/2
          float y = radius * sin(phi) - height / 2.0f; // Offset by half height
          float r = radius * cos(phi);

          for (int j = 0; j <= slices; ++j) {
              float theta = glm::two_pi<float>() * j / slices;
              float x = r * cos(theta);
              float z = r * sin(theta);

              vertices.push_back({
                  {x, y, z},                 // Position
                  glm::normalize(glm::vec3(x, y + height / 2.0f, z)), // Normal
                  {float(j) / slices, float(i) / stacks}, // UV
                  {1.0f, 0.0f, 0.0f},        // Tangent
                  {0.0f, 1.0f, 0.0f},        // Bitangent
                  {1.0f, 1.0f, 1.0f, 1.0f}   // Color
                  });
          }
      }

      // Generate indices for top hemisphere
      for (int i = 0; i < stacks; ++i) {
          for (int j = 0; j < slices; ++j) {
              uint32_t curr = i * (slices + 1) + j;
              uint32_t next = curr + slices + 1;

              indices.push_back(curr);
              indices.push_back(curr + 1);
              indices.push_back(next);

              indices.push_back(curr + 1);
              indices.push_back(next + 1);
              indices.push_back(next);
          }
      }

      // Generate indices for cylinder
      int offset = (stacks + 1) * (slices + 1);
      for (int i = 0; i < stacks; ++i) {
          for (int j = 0; j < slices; ++j) {
              uint32_t curr = offset + i * (slices + 1) + j;
              uint32_t next = curr + slices + 1;

              indices.push_back(curr + 1);
              indices.push_back(next);
              indices.push_back(curr);

              indices.push_back(next + 1);
              indices.push_back(next);
              indices.push_back(curr + 1);
          }
      }

      // Generate indices for bottom hemisphere
      offset += (stacks + 1) * (slices + 1);
      for (int i = 0; i < stacks; ++i) {
          for (int j = 0; j < slices; ++j) {
              uint32_t curr = offset + i * (slices + 1) + j;
              uint32_t next = curr + slices + 1;

              indices.push_back(curr);
              indices.push_back(next);
              indices.push_back(curr + 1);

              indices.push_back(curr + 1);
              indices.push_back(next);
              indices.push_back(next + 1);
          }
      }

      // Create VAO and VBO
      auto vao = VertexArray::Create();
      auto vbo = VertexBuffer::Create(static_cast<unsigned>(vertices.size() * sizeof(Vertex)));
      vbo->SetData(vertices.data(), static_cast<unsigned>(vertices.size() * sizeof(Vertex)));

      BufferLayout layout = {
          {AttributeType::VEC3, "a_Position"},
          {AttributeType::VEC3, "a_Normal"},
          {AttributeType::VEC2, "a_TexCoord"},
          {AttributeType::FLOAT, "a_TexIdx"},
          {AttributeType::VEC3, "a_Tangent"},
          {AttributeType::VEC3, "a_Bitangent"},
          {AttributeType::VEC4, "a_Color"},
      };

      vbo->SetLayout(layout);
      vao->AddVertexBuffer(vbo);

      auto ebo = ElementBuffer::Create(indices.data(), static_cast<uint32_t>(indices.size()));
      vao->SetElementBuffer(ebo);

      std::vector<Submesh> submeshes;
      Submesh capsuleSubmesh{
          0,                                 // baseVtx
          0,                                 // baseIdx
          static_cast<uint32_t>(vertices.size()),   // vtxCount
          static_cast<uint32_t>(indices.size()),    // idxCount
          0,                                 // materialIdx
          glm::mat4(1.0f),                   // Identity matrix for transform
          indices                            // Indices for the submesh
      };

      submeshes.push_back(capsuleSubmesh);

      return MeshSource(vao, submeshes, vertices, indices);
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
