#pragma once
#include <unordered_map>
#include <Graphics/Model.h>
#include <memory>

namespace Graphics
{

  class MeshGen
  {
  public:
    static std::shared_ptr<Model> GetMesh(Graphics::MeshType type);
    static std::shared_ptr<Model> GetMesh(std::string const& mesh);
    static void ReleaseMesh(std::string const& mesh);

  private:
    static std::unordered_map<std::string, std::shared_ptr<Model>> m_models;
  };

} // namespace Graphics
