#include <pch.h>
#include "MeshFactory.h"

namespace Graphics {
	std::shared_ptr<MeshSource> MeshFactory::CreateCube() {
        std::vector<Vertex> cubeVertices{
            // Front face
            {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}, {0.0f, 0.0f}, {}, {}},  // Bottom-left
            {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}, {1.0f, 0.0f}, {}, {}},  // Bottom-right
            {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}, {1.0f, 1.0f}, {}, {}},  // Top-right
            {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}, {0.0f, 1.0f}, {}, {}},  // Top-left

            // Back face
            {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, {}, {}},  // Bottom-right
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, {}, {}},  // Bottom-left
            {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, {}, {}},  // Top-left
            {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, {}, {}},  // Top-right

            // Left face
            {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {}, {}},  // Bottom-back
            {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {}, {}},  // Bottom-front
            {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {}, {}},  // Top-front
            {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {}, {}},  // Top-back

            // Right face
            {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {}, {}},  // Bottom-front
            {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {}, {}},  // Bottom-back
            {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {}, {}},  // Top-back
            {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {}, {}},  // Top-front

            // Top face
            {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {}, {}},  // Front-left
            {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {}, {}},  // Front-right
            {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {}, {}},  // Back-right
            {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {}, {}},  // Back-left

            // Bottom face
            {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {}, {}},  // Back-left
            {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {}, {}},  // Back-right
            {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {}, {}},  // Front-right
            {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {}, {}}   // Front-left
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
}