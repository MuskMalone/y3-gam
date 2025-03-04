/*!*********************************************************************
\file   IMSH.h
\author chengen.lau\@digipen.edu
\date   20-September-2024
\brief  Class acting as the intermediary step between a n external
        model file and the engine's custom format. It reads the
        contents with Assimp and then converts it in to an .imsh
        object, which can then be used as a MeshSource.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <memory>
#include <string>
#include <vector>
#include <Graphics/Mesh/MeshSource.h>

// forward declarations
struct aiScene; struct aiNode;
template <typename T> class aiMatrix4x4t;
typedef aiMatrix4x4t<float> aiMatrix4x4;

namespace Graphics::AssetIO
{
  struct ImportSettings {
    bool staticMesh = false;
    bool recenterMesh = true, normalizeScale = true, flipUVs = false;
  };

  class IMSH
  {
  public:
    IMSH() : mVertexBuffer{}, mIndices{}, mSubmeshData{}, mStatus{ true }, mIsStatic{ true } {}
    // conversions for use in editor only
    IMSH(std::string const& file, ImportSettings const& = {});

    /*!*********************************************************************
    \brief
      Writes the object to a binary file
    \param path
      The path of the output file
    ************************************************************************/
    void WriteToBinFile(std::string const& path) const;

    operator bool() const noexcept { return mStatus; } // check if object is valid
    inline std::vector<Graphics::Vertex> const& GetVertexBuffer() const noexcept { return mVertexBuffer; }
    inline std::vector<uint32_t> const& GetIndices() const noexcept { return mIndices; }
    
    /*!*********************************************************************
    \brief
      Reads a binary file into the object
    \param file
      The binary file to read from
    ************************************************************************/
    void ReadFromBinFile(std::string const& file);
    
    // NOTE: This converts the object into a mesh source. Data will be MOVED into
    // the MeshSource and accessing contents afterwards is undefined
    MeshSource ToMeshSource(std::shared_ptr<VertexArray> vao);

  private:
    // serialized as first 32 bytes
    struct Header {
      Header() = default;
      Header(uint64_t _vtxSize, uint64_t _idxSize, uint64_t _submeshSize, uint64_t _nameSize)
        : vtxSize{ _vtxSize }, idxSize{ _idxSize }, submeshSize{ _submeshSize }, nameSize{ _nameSize } {}
      uint64_t vtxSize, idxSize, submeshSize, nameSize;
    };

    // used to read/write submesh data
    // we will copy the indices over from the base mesh during load
    struct SubmeshData
    {
      SubmeshData() = default;
      SubmeshData(uint32_t _baseVtx, uint32_t _baseIdx, uint32_t _vtxCount, uint32_t _idxCount)
        : baseVtx{ _baseVtx }, baseIdx{ _baseIdx }, vtxCount{ _vtxCount }, idxCount{ _idxCount } {}

      uint32_t baseVtx, baseIdx, vtxCount, idxCount;
    };

    std::vector<Graphics::Vertex> mVertexBuffer;
    std::vector<uint32_t> mIndices;
    std::vector<SubmeshData> mSubmeshData;
    std::vector<std::string> mMeshNames;
    glm::vec3 mMin, mMax;
    bool mStatus, mIsStatic;

    static const unsigned sAssimpImportFlags, sMinimalAssimpImportFlags;
    static const unsigned sMeshImportFlags;

    void ComputeBV();
    void RecenterMesh();
    void NormalizeScale(bool isMeshRecentered);

    void ProcessSubmeshes(aiNode* node, aiScene const* scene, aiMatrix4x4 const& parentMtx);
    void ProcessMeshes(aiNode* node, aiScene const* scene);
  };

} // namespace Graphics::AssetIO


// don't think we'll ever need this
#ifdef OLD_STUFF
struct MeshImportFlags
{
  MeshImportFlags() : boneWeights{ false }, animations{ false },
    lights{ false }, cameras{ false }, materials{ false } {}

  inline bool IsDefault() const noexcept { return !(boneWeights || animations || lights || cameras || materials); }
  int GetFlags() const {
    int ret{};
    if (!animations) { ret |= aiComponent_ANIMATIONS; }
    if (!boneWeights) { ret |= aiComponent_BONEWEIGHTS; }
    if (!cameras) { ret |= aiComponent_CAMERAS; }
    if (!materials) { ret |= aiComponent_MATERIALS; }
    if (!lights) { ret |= aiComponent_LIGHTS; }

    return ret;
  }

  bool boneWeights, animations, lights, cameras, materials;
};
#endif

