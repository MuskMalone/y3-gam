#include <pch.h>
#include "MeshGen.h"
#include <filesystem>

namespace Graphics
{

  std::unordered_map<std::string, std::shared_ptr<Model>> MeshGen::m_models;

  std::shared_ptr<Model> MeshGen::GetMesh(Graphics::MeshType type)
  {
    std::string const meshToLoad{ Graphics::MESH_NAMES[static_cast<int>(type)] };

    // check if mesh has already been created
    auto result{ m_models.find(meshToLoad) };
    if (result != m_models.end()) { return result->second; }

    // if not, load it and return the model
    m_models.emplace(meshToLoad, std::make_shared<Model>(type, MESH_NAMES[static_cast<int>(type)]));

    return m_models[meshToLoad];
  }

  std::shared_ptr<Model> MeshGen::GetMesh(std::string const& mesh)
  {
    // check if mesh has already been created
    auto result{ m_models.find(mesh) };
    if (result != m_models.end()) { return result->second; }

    // if not, load it and return the model
    m_models.emplace(mesh, std::make_shared<Model>(mesh, std::filesystem::path(mesh).stem().string()));

    return m_models[mesh];
  }

  void MeshGen::ReleaseMesh(std::string const& mesh)
  {
    auto modelToErase{ m_models.find(mesh) };
    if (modelToErase == m_models.end()) { throw std::runtime_error("No such model \"" + mesh + "\" exists!"); }

    modelToErase->second->Release();
    m_models.erase(modelToErase);
  }

} // namespace Graphics
