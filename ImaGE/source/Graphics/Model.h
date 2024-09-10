#pragma once
#include <glad/glad.h>
#include <stdint.h>
#include <Graphics/VertexAttribs.h>
#include <Graphics/MeshType.h>

struct aiNode; struct aiScene;

namespace Graphics
{
  class Model
  {
  public:
    size_t drawCount;
    GLenum primitiveType;
    bool isUsingIndices;
    std::string const name;

    Model(Graphics::MeshType _meshType, std::string _name);
    Model(std::string const& filePath, std::string _name); // load from obj file
    
    // accessors
    inline VertexAttribs const& GetVertexAttribs() const noexcept { return m_vtxAttribs; }
    inline GLuint GetVAO() const noexcept { return m_vao; }
    inline GLuint GetVBO() const noexcept { return m_vbo; }
    
    void Release();

  private:
    GLuint m_vbo, m_vao;
    VertexAttribs m_vtxAttribs;

    void BindModel();
    void ProcessNode(aiNode* node, aiScene const* scene);
  };

} // namespace Graphics