#include <pch.h>
#include "IMSH.h"

#ifndef IMGUI_DISABLE
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

namespace {
  glm::vec2 ToGLMVec2(aiVector3D const& vec) { return { vec.x, vec.y }; }
  glm::vec3 ToGLMVec3(aiVector3D const& vec) { return { vec.x, vec.y, vec.z }; }
  glm::vec4 ToGLMVec4(aiColor4D const& vec) { return { vec.r, vec.g, vec.b, vec.a }; }

  void AddVertices(std::vector<Graphics::Vertex>& vertexBuffer, aiMesh const* mesh);
  void AddIndices(std::vector<uint32_t>& indices, aiMesh const* mesh);
}

namespace Graphics::AssetIO
{
  // erm i read through all the flags and these seemed like the best setup? Idk needs some testing
  unsigned IMSH::sAssimpImportFlags = aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate
    | aiProcess_RemoveComponent | aiProcess_GenSmoothNormals | aiProcess_ImproveCacheLocality | aiProcess_FixInfacingNormals
    | aiProcess_FindDegenerates | aiProcess_FindInvalidData | aiProcess_SortByPType | aiProcess_OptimizeMeshes | aiProcess_FlipUVs;


  int MeshImportFlags::GetFlags() const {
    int ret{};
    if (!animations) { ret |= aiComponent_ANIMATIONS; }
    if (!boneWeights) { ret |= aiComponent_BONEWEIGHTS; }
    if (!cameras) { ret |= aiComponent_CAMERAS; }
    if (!materials) { ret |= aiComponent_MATERIALS; }
    if (!lights) { ret |= aiComponent_LIGHTS; }

    return ret;
  }

  IMSH::IMSH(std::string const& file, MeshImportFlags const& importFlags) : mVertexBuffer{}, mIndices{}, mSubmeshData{}, mStatus { true } {
    Assimp::Importer importer;

    importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, importFlags.GetFlags());

    aiScene const* aiScn{ importer.ReadFile(file, sAssimpImportFlags) };
    if (!aiScn || aiScn->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiScn->mRootNode) {
      mStatus = false;
      Debug::DebugLogger::GetInstance().LogError("Unable to import model " + file);
      return;
    }

    ProcessMeshes(aiScn->mRootNode, aiScn);
  }

  void IMSH::ProcessSubmeshes(aiNode* node, aiScene const* scene)
  {
    // now add the submeshes
    for (unsigned i{}; i < node->mNumMeshes; ++i) {
      aiMesh const* mesh{ scene->mMeshes[node->mMeshes[i]] };
      uint32_t const vtxOffset{ static_cast<uint32_t>(mVertexBuffer.size()) }, idxOffset{ static_cast<uint32_t>(mIndices.size()) };
      AddVertices(mVertexBuffer, mesh);
      AddIndices(mIndices, mesh);
      mSubmeshData.emplace_back(vtxOffset, idxOffset, mesh->mNumVertices, mIndices.size() - idxOffset, 0);
    }

    for (unsigned i{}; i < node->mNumChildren; ++i) {
      ProcessSubmeshes(node->mChildren[i], scene);
    }
  }

  void IMSH::ProcessMeshes(aiNode* node, aiScene const* scene) {
    // process base meshes
    for (unsigned i{}; i < node->mNumMeshes; ++i) {
      aiMesh const* mesh{ scene->mMeshes[node->mMeshes[i]] };
      AddVertices(mVertexBuffer, mesh);
      AddIndices(mIndices, mesh);
    }

    mSubmeshData.reserve(node->mNumChildren);
    for (unsigned i{}; i < node->mNumChildren; ++i) {
      ProcessSubmeshes(node->mChildren[i], scene);
    }
  }

  void IMSH::WriteToBinFile(std::string const& name, std::string const& path) const {
    std::string const outputFile{ gMeshOutputDir + name + gMeshFileExt };
    std::ofstream ofs{ outputFile, std::ios::binary};
    if (!ofs) { throw Debug::Exception<IMSH>(Debug::LVL_ERROR, Msg("Unable to create binary file: " + outputFile)); }

    Header const header{ mVertexBuffer.size(), mIndices.size(), mSubmeshData.size()};

    ofs.write(reinterpret_cast<char const*>(&header), sizeof(Header));
    ofs.write(reinterpret_cast<char const*>(mVertexBuffer.data()), header.vtxSize * sizeof(Graphics::Vertex));
    ofs.write(reinterpret_cast<char const*>(mIndices.data()), header.idxSize * sizeof(uint32_t));
    ofs.write(reinterpret_cast<char const*>(mSubmeshData.data()), header.submeshSize * sizeof(SubmeshData));

    ofs.close();
  }
#else
namespace Graphics::AssetIO
{
#endif  // IMGUI_DISABLE

  std::shared_ptr<MeshSource> IMSH::ToMeshSource(std::shared_ptr<VertexArray> vao) {
    mStatus = false;

    std::vector<Submesh> submeshes;
    submeshes.reserve(mSubmeshData.size());
    for (auto const& data : mSubmeshData) {
      submeshes.emplace_back(data.baseVtx, data.baseIdx, data.vtxCount, data.idxCount, data.materialIdx,
        glm::mat4(1.f), std::vector<uint32_t>(mIndices.begin() + data.baseIdx, mIndices.begin() + data.idxCount));
    }

    return std::make_shared<MeshSource>(std::move(vao), std::move(submeshes), std::move(mVertexBuffer), std::move(mIndices));
  }

  void IMSH::ReadFromBinFile(std::string const& file) {
    std::ifstream ifs{ file, std::ios::binary };
    if (!ifs) {
      mStatus = false; return;
    }

    Header header{};
    ifs.read(reinterpret_cast<char*>(&header), sizeof(Header));

    mVertexBuffer.resize(header.vtxSize);
    ifs.read(reinterpret_cast<char*>(mVertexBuffer.data()), header.vtxSize * sizeof(Graphics::Vertex));
    mIndices.resize(header.idxSize);
    ifs.read(reinterpret_cast<char*>(mIndices.data()), header.idxSize * sizeof(uint32_t));
    mSubmeshData.resize(header.submeshSize);
    ifs.read(reinterpret_cast<char*>(mSubmeshData.data()), header.submeshSize * sizeof(SubmeshData));

    ifs.close();
  }
} // namespace Graphics::AssetIO

#ifndef IMGUI_DISABLE

namespace {
  void AddVertices(std::vector<Graphics::Vertex>& vertexBuffer, aiMesh const* mesh)
  {
    vertexBuffer.resize(vertexBuffer.size() + mesh->mNumVertices);
    for (unsigned i{}; i < mesh->mNumVertices; ++i) {
      vertexBuffer[i].position = ToGLMVec3(mesh->mVertices[i]);

      // should be automatically generated by assimp if not present
      vertexBuffer[i].normal = ToGLMVec3(mesh->mNormals[i]);
    }

    if (mesh->HasTextureCoords(0)) {
      for (unsigned i{}; i < mesh->mNumVertices; ++i) {
        vertexBuffer[i].texcoord = ToGLMVec2(mesh->mTextureCoords[0][i]);
        vertexBuffer[i].tangent = ToGLMVec3(mesh->mTangents[i]);
        vertexBuffer[i].bitangent = ToGLMVec3(mesh->mBitangents[i]);
      }
    }

    if (mesh->HasVertexColors(0)) {
      for (unsigned i{}; i < mesh->mNumVertices; ++i) {
        vertexBuffer[i].texcoord = ToGLMVec4(mesh->mColors[0][i]);
      }
    }
  }

  void AddIndices(std::vector<uint32_t>& indices, aiMesh const* mesh) {
    for (size_t i{}, totalCount{ indices.size() }; i < mesh->mNumFaces; ++i) {
      auto const& face{ mesh->mFaces[i] };
      totalCount += face.mNumIndices;
      indices.reserve(totalCount);
      for (unsigned j{}; j < face.mNumIndices; ++j) {
        indices.emplace_back(face.mIndices[j]);
      }
    }
  }
}
#endif
