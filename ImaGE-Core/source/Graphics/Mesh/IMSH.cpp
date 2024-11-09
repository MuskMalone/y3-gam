/*!*********************************************************************
\file   IMSH.cpp
\author chengen.lau\@digipen.edu
\date   20-September-2024
\brief  Class acting as the intermediary step between an external
        model file and the engine's custom format. It reads the
        contents with Assimp and then converts it in to an .imsh
        object, which can then be used as a MeshSource.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include "pch.h"
#include "IMSH.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <glm/gtx/transform.hpp>

namespace {
  static bool sFirstMaterialEncountered{ false };

  glm::vec2 ToGLMVec2(aiVector3D const& vec) { return { vec.x, vec.y }; }
  glm::vec3 ToGLMVec3(aiVector3D const& vec) { return { vec.x, vec.y, vec.z }; }
  glm::vec3 ToGLMVec3(aiColor3D const& col) { return { col.r, col.g, col.b }; }
  glm::vec4 ToGLMVec4(aiColor4D const& vec) { return { vec.r, vec.g, vec.b, vec.a }; }

  void AddVertices(std::vector<Graphics::Vertex>& vertexBuffer, aiMesh const* mesh);
  void AddVertices(std::vector<Graphics::Vertex>& vertexBuffer, aiMesh const* mesh, aiMatrix4x4 const& transMtx);
  void AddIndices(std::vector<uint32_t>& indices, aiMesh const* mesh, unsigned offset = 0);
  //Graphics::MeshMatValues GetMaterial(aiMaterial* material);
}

namespace Graphics::AssetIO
{
  // erm i read through all the flags and these seemed like the best setup? Idk needs some testing
  unsigned const IMSH::sMinimalAssimpImportFlags = aiProcess_ValidateDataStructure | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices
    | aiProcess_GenSmoothNormals | aiProcess_SortByPType;
  unsigned const IMSH::sAssimpImportFlags = aiProcess_ValidateDataStructure | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate
    | aiProcess_RemoveComponent | aiProcess_GenSmoothNormals | aiProcess_ImproveCacheLocality | aiProcess_FixInfacingNormals
    | aiProcess_FindInvalidData | aiProcess_SortByPType | aiProcess_OptimizeMeshes | aiProcess_FlipUVs | aiProcess_RemoveRedundantMaterials
    | aiProcess_SplitLargeMeshes | aiProcess_TransformUVCoords;


  int MeshImportFlags::GetFlags() const {
    int ret{};
    if (!animations) { ret |= aiComponent_ANIMATIONS; }
    if (!boneWeights) { ret |= aiComponent_BONEWEIGHTS; }
    if (!cameras) { ret |= aiComponent_CAMERAS; }
    if (!materials) { ret |= aiComponent_MATERIALS; }
    if (!lights) { ret |= aiComponent_LIGHTS; }

    return ret;
  }

  IMSH::IMSH(std::string const& file, MeshImportFlags const& importFlags) : mVertexBuffer{}, mIndices{}, mSubmeshData{},
    mStatus{ true }, mIsStatic{ sStaticMeshConversion } {
    Assimp::Importer importer;

    importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, importFlags.GetFlags());

    aiScene const* aiScn{ importer.ReadFile(file, sAssimpImportFlags) };
    if (!aiScn || aiScn->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiScn->mRootNode) {
      mStatus = false;
      Debug::DebugLogger::GetInstance().LogError("Unable to import model " + file);
      return;
    }

    sFirstMaterialEncountered = false;
    ProcessMeshes(aiScn->mRootNode, aiScn);

    if (sRecenterMesh) {
      RecenterMesh();
    }
    if (sNormalizeScale) {
      NormalizeScale();
    }
  }

  void IMSH::ProcessSubmeshes(aiNode* node, aiScene const* scene, aiMatrix4x4 const& parentMtx)
  {
    aiMatrix4x4 const transMtx{ parentMtx * node->mTransformation };

    // now add the submeshes
    unsigned const vtxOffset{ static_cast<unsigned>(mVertexBuffer.size()) }, idxOffset{ static_cast<unsigned>(mIndices.size()) };
    for (unsigned i{}; i < node->mNumMeshes; ++i) {
      unsigned const meshIdx{ node->mMeshes[i] };
      aiMesh const* mesh{ scene->mMeshes[meshIdx] };

      AddVertices(mVertexBuffer, mesh, transMtx);
      AddIndices(mIndices, mesh, vtxOffset);

      //if (meshIdx < scene->mNumMaterials) {
      //  mMatValues.emplace_back(GetMaterial(scene->mMaterials[meshIdx]));
      //}
      
      // add a submesh entry if needed
      if (!mIsStatic) {
        mSubmeshData.emplace_back(vtxOffset, idxOffset, mesh->mNumVertices, mIndices.size() - idxOffset);
      }
    }

    if (!mIsStatic) {
      mSubmeshData.reserve(mSubmeshData.size() + node->mNumChildren);
    }
    for (unsigned i{}; i < node->mNumChildren; ++i) {
      ProcessSubmeshes(node->mChildren[i], scene, transMtx);
    }
  }

  void IMSH::ProcessMeshes(aiNode* node, aiScene const* scene) {
    // process base meshes
    for (unsigned i{}; i < node->mNumMeshes; ++i) {
      unsigned const meshIdx{ node->mMeshes[i] };
      aiMesh const* mesh{ scene->mMeshes[meshIdx] };

      AddVertices(mVertexBuffer, mesh, node->mTransformation);
      AddIndices(mIndices, mesh, 0);

      //if (scene->HasMaterials()) {
      //  mMatValues.emplace_back(GetMaterial(scene->mMaterials[meshIdx]));
      //}

      // since the first mesh is also rendered as a submesh
      if (!mIsStatic) {
        mSubmeshData.emplace_back(0, 0, mesh->mNumVertices, mIndices.size());
      }
    }

    if (!mIsStatic) {
      mSubmeshData.reserve(node->mNumChildren);
    }
    for (unsigned i{}; i < node->mNumChildren; ++i) {
      ProcessSubmeshes(node->mChildren[i], scene, node->mTransformation);
    }
  }

  void IMSH::WriteToBinFile(std::string const& path) const {
      //tch: i added a compiled folder for compilation
      std::string const outputFile{ path };
      std::ofstream ofs{ outputFile, std::ios::binary };
      if (!ofs) { throw Debug::Exception<IMSH>(Debug::LVL_ERROR, Msg("Unable to create binary file: " + outputFile)); }

      Header const header{ mVertexBuffer.size(), mIndices.size(), mSubmeshData.size() };

      ofs.write(reinterpret_cast<char const*>(&header), sizeof(Header));
      ofs.write(reinterpret_cast<char const*>(mVertexBuffer.data()), header.vtxSize * sizeof(Graphics::Vertex));
      ofs.write(reinterpret_cast<char const*>(mIndices.data()), header.idxSize * sizeof(uint32_t));
      ofs.write(reinterpret_cast<char const*>(mSubmeshData.data()), header.submeshSize * sizeof(SubmeshData));
      ofs.write(reinterpret_cast<char const*>(&mIsStatic), sizeof(bool));

      ofs.close();
  }

  MeshSource IMSH::ToMeshSource(std::shared_ptr<VertexArray> vao) {
    mStatus = false;

    std::vector<Submesh> submeshes;
    if (!mIsStatic) {
      submeshes.reserve(mSubmeshData.size());
      for (auto const& data : mSubmeshData) {
        submeshes.emplace_back(data.baseVtx, data.baseIdx, data.vtxCount, data.idxCount,
          0, glm::identity<glm::mat4>(), std::vector<uint32_t>());
      }
    }

    return MeshSource(std::move(vao), std::move(submeshes), std::move(mVertexBuffer), std::move(mIndices));
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
    ifs.read(reinterpret_cast<char*>(&mIsStatic), sizeof(bool));

    ifs.close();
  }

  void IMSH::RecenterMesh() {
    glm::vec3 min{ FLT_MAX }, max{ -FLT_MAX };

    // find the min and max AABB
    for (Graphics::Vertex const& vtx : mVertexBuffer) {
      min = glm::min(vtx.position, min);
      max = glm::max(vtx.position, max);
    }

    // get the center of the AABB
    glm::vec3 const center{ (min + max) * 0.5f };
    // if already centered, return
    if (glm::all(glm::lessThan(glm::abs(center), glm::vec3(glm::epsilon<float>())))) { return; }

    // map center to { 0, 0, 0 }
    for (Graphics::Vertex& vtx : mVertexBuffer) {
      vtx.position -= center;
    }
  }

  void IMSH::NormalizeScale() {
    glm::vec3 min{ FLT_MAX }, max{ -FLT_MAX };

    // find the min and max AABB
    for (Graphics::Vertex const& vtx : mVertexBuffer) {
      min = glm::min(vtx.position, min);
      max = glm::max(vtx.position, max);
    }

    glm::vec3 const center{ sRecenterMesh ? glm::vec3() : (min + max) * 0.5f };
    glm::vec3 const scale{ max - min };
    float const maxScale{ std::min(scale.x, std::min(scale.y, scale.z)) };
    glm::mat4 transform{
      1.f / maxScale, 0.f, 0.f, 0.f,
      0.f, 1.f / maxScale, 0.f, 0.f,
      0.f, 0.f, 1.f / maxScale, 0.f,
      center.x, center.y, center.z, 1.f
    };

    // if mesh wasn't centered at origin, move it to origin before scaling
    if (!sRecenterMesh) {
      transform = transform * glm::translate(glm::identity<glm::mat4>(), -center);
    }

    // transform all vertices
    for (Graphics::Vertex& vtx : mVertexBuffer) {
      vtx.position = transform * glm::vec4(vtx.position, 1.f);
    }
  }

} // namespace Graphics::AssetIO


namespace {
  void AddVertices(std::vector<Graphics::Vertex>& vertexBuffer, aiMesh const* mesh)
  {
    vertexBuffer.reserve(vertexBuffer.size() + mesh->mNumVertices);
    std::vector<Graphics::Vertex> temp(mesh->mNumVertices);
    for (unsigned i{}; i < mesh->mNumVertices; ++i) {
      temp[i].position = ToGLMVec3(mesh->mVertices[i]);

      // should be automatically generated by assimp if not present
      temp[i].normal = ToGLMVec3(mesh->mNormals[i]);
    }
    
    if (mesh->HasTextureCoords(0)) {
      for (unsigned i{}; i < mesh->mNumVertices; ++i) {
        temp[i].texcoord = ToGLMVec2(mesh->mTextureCoords[0][i]);
        //vertexBuffer[i].tangent = ToGLMVec3(mesh->mTangents[i]);
        //vertexBuffer[i].bitangent = ToGLMVec3(mesh->mBitangents[i]);
      }
    }

    if (mesh->HasVertexColors(0)) {
      for (unsigned i{}; i < mesh->mNumVertices; ++i) {
        temp[i].color = ToGLMVec4(mesh->mColors[0][i]);
      }
    }

    vertexBuffer.insert(vertexBuffer.end(), temp.begin(), temp.end());
  }

  void AddVertices(std::vector<Graphics::Vertex>& vertexBuffer, aiMesh const* mesh, aiMatrix4x4 const& transMtx) {
    vertexBuffer.reserve(vertexBuffer.size() + mesh->mNumVertices);
    std::vector<Graphics::Vertex> temp(mesh->mNumVertices);
    for (unsigned i{}; i < mesh->mNumVertices; ++i) {
      temp[i].position = ToGLMVec3(transMtx * mesh->mVertices[i]);

      // should be automatically generated by assimp if not present
      temp[i].normal = ToGLMVec3(mesh->mNormals[i]);
    }

    if (mesh->HasTextureCoords(0)) {
      for (unsigned i{}; i < mesh->mNumVertices; ++i) {
        temp[i].texcoord = ToGLMVec2(mesh->mTextureCoords[0][i]);
        //vertexBuffer[i].tangent = ToGLMVec3(mesh->mTangents[i]);
        //vertexBuffer[i].bitangent = ToGLMVec3(mesh->mBitangents[i]);
      }
    }

    if (mesh->HasVertexColors(0)) {
      for (unsigned i{}; i < mesh->mNumVertices; ++i) {
        temp[i].color = ToGLMVec4(mesh->mColors[0][i]);
      }
    }

    vertexBuffer.insert(vertexBuffer.end(), temp.begin(), temp.end());
  }

  void AddIndices(std::vector<uint32_t>& indices, aiMesh const* mesh, unsigned offset) {
    for (size_t i{}, totalCount{ indices.size() }; i < mesh->mNumFaces; ++i) {
      auto const& face{ mesh->mFaces[i] };
      totalCount += face.mNumIndices;
      indices.reserve(totalCount);
      for (unsigned j{}; j < face.mNumIndices; ++j) {
        indices.emplace_back(face.mIndices[j] + offset);
      }
    }
  }

 /* Graphics::MeshMatValues GetMaterial(aiMaterial* material) {
    if (!material) { return {}; }

    Graphics::MeshMatValues ret;

    {
      aiColor3D col{};
      if (material->Get(AI_MATKEY_COLOR_DIFFUSE, col) == aiReturn_SUCCESS) {
        ret.diffuseCol = ToGLMVec3(col);
      }
      if (material->Get(AI_MATKEY_COLOR_AMBIENT, col) == aiReturn_SUCCESS) {
        ret.ambientCol = ToGLMVec3(col);
      }
      if (material->Get(AI_MATKEY_COLOR_SPECULAR, col) == aiReturn_SUCCESS) {
        ret.specularCol = ToGLMVec3(col);
      }
      if (material->Get(AI_MATKEY_COLOR_EMISSIVE, col) == aiReturn_SUCCESS) {
        ret.emissiveCol = ToGLMVec3(col);
      }
    }

    float val;
    if (material->Get(AI_MATKEY_OPACITY, val) == aiReturn_SUCCESS) {
      ret.opacity = val;
    }
    if (material->Get(AI_MATKEY_SHININESS, val) == aiReturn_SUCCESS) {
      ret.shininess = val;
    }
    if (material->Get(AI_MATKEY_REFRACTI, val) == aiReturn_SUCCESS) {
      ret.refractionIndex = val;
    }

    return ret;
  }*/
}
