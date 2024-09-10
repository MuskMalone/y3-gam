#include <pch.h>
#include "Model.h"
#include <Graphics/VertexAttribs.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Graphics
{

  Model::Model(Graphics::MeshType _meshType, std::string _name) : name{ std::move(_name) }
  {
    switch (_meshType)
    {
    case Graphics::MeshType::POINT:
      m_vtxAttribs = Graphics::GeneratePoint();
      primitiveType = GL_POINTS;
      break;
    case Graphics::MeshType::LINE:
      m_vtxAttribs = Graphics::GenerateLine();
      primitiveType = GL_LINES;
      break;
    case Graphics::MeshType::CUBE:
      m_vtxAttribs = Graphics::GenerateCube();
      primitiveType = GL_TRIANGLES;
      break;
    case Graphics::MeshType::SPHERE:
      m_vtxAttribs = Graphics::GenerateSphere();
      primitiveType = GL_TRIANGLES;
      break;
    case Graphics::MeshType::TRIANGLE:
      m_vtxAttribs = Graphics::GenerateTriangle();
      primitiveType = GL_TRIANGLES;
      break;
    case Graphics::MeshType::PLANE:
      m_vtxAttribs = Graphics::GeneratePlane();
      primitiveType = GL_TRIANGLE_FAN;
      break;
    default:
      throw std::runtime_error("Invalid MeshType read!");
      break;
    }
    
    BindModel();
  }

  Model::Model(std::string const& filePath, std::string _name) : primitiveType{ GL_TRIANGLES }, name{ std::move(_name) }
  {
    Assimp::Importer importer;
    aiScene const* scene{ importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs) };
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
      throw std::runtime_error(importer.GetErrorString());
    }

    ProcessNode(scene->mRootNode, scene);
    BindModel();
  }

  void Model::BindModel()
  {
    glCreateBuffers(1, &m_vbo);
    
    glNamedBufferStorage(m_vbo, sizeof(glm::vec3) * (m_vtxAttribs.points.size() + m_vtxAttribs.normals.size()),
      nullptr, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferSubData(m_vbo, 0, sizeof(glm::vec3) * m_vtxAttribs.points.size(), m_vtxAttribs.points.data());
    if (!m_vtxAttribs.normals.empty())
    {
      glNamedBufferSubData(m_vbo, sizeof(glm::vec3) * m_vtxAttribs.points.size(),
        sizeof(glm::vec3) * m_vtxAttribs.normals.size(), m_vtxAttribs.normals.data());
    }

    // vtx pos arr
    glCreateVertexArrays(1, &m_vao);

    glEnableVertexArrayAttrib(m_vao, 0);
    glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, sizeof(glm::vec3));
    glVertexArrayAttribFormat(m_vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(m_vao, 0, 0);

    // vtx normals
    if (!m_vtxAttribs.normals.empty())
    {
      glEnableVertexArrayAttrib(m_vao, 1);
      glVertexArrayVertexBuffer(m_vao, 1, m_vbo,
        sizeof(glm::vec3) * m_vtxAttribs.points.size(), sizeof(glm::vec3));
      glVertexArrayAttribFormat(m_vao, 1, 3, GL_FLOAT, GL_FALSE, 0);
      glVertexArrayAttribBinding(m_vao, 1, 1);
    }

    // indices
    isUsingIndices = !m_vtxAttribs.indices.empty();
    if (isUsingIndices)
    {
      GLuint eboHdl;
      glCreateBuffers(1, &eboHdl);
      glNamedBufferStorage(eboHdl, sizeof(GLushort) * m_vtxAttribs.indices.size(),
        m_vtxAttribs.indices.data(), GL_DYNAMIC_STORAGE_BIT);
      glVertexArrayElementBuffer(m_vao, eboHdl);
    }

    glBindVertexArray(0); // unbind VAO

    drawCount = m_vtxAttribs.indices.empty() ? m_vtxAttribs.points.size() / 3 : m_vtxAttribs.indices.size();
  }

  void Model::Release()
  {
    glInvalidateBufferData(m_vao);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    //glDeleteTextures();
  }

  void Model::ProcessNode(aiNode* node, aiScene const* scene)
  {
    // process meshes
    for (unsigned i{}; i < node->mNumMeshes; ++i) {
      m_vtxAttribs.AddMeshes(scene->mMeshes[node->mMeshes[i]]);
    }

    // recursively do the same for all children
    for (unsigned i{}; i < node->mNumChildren; ++i) {
      ProcessNode(node->mChildren[i], scene);
    }
  }

} // namespace Graphics
