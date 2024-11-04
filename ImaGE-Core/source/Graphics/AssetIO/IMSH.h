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
#include <Graphics/MeshSource.h>
#include <Graphics/Vertex.h>

// forward declarations
struct aiScene; struct aiNode;

namespace Graphics::AssetIO
{

  struct MeshImportFlags
  {
    MeshImportFlags() : boneWeights{ false }, animations{ false },
      lights{ false }, cameras{ false }, materials{ false } {}

    inline bool IsDefault() const noexcept { return boneWeights || animations || lights || cameras || materials; }
    int GetFlags() const;

    bool boneWeights, animations, lights, cameras, materials;
  };


  class IMSH
  {
  public:
    IMSH() : mVertexBuffer{}, mIndices{}, mSubmeshData{}, mStatus{ true } {}
    // conversions for use in editor only
    IMSH(std::string const& file, MeshImportFlags const& = {});

    /*!*********************************************************************
    \brief
      Writes the object to a binary file
    \param name
      The name of the output file
    ************************************************************************/
    void WriteToBinFile(std::string const& name) const;

    operator bool() const { return mStatus; } // check if object is valid
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
    // serialized as first 24 bytes
    struct Header {
      Header() = default;
      Header(uint64_t _vtxSize, uint64_t _idxSize, uint64_t _submeshSize)
        : vtxSize{ _vtxSize }, idxSize{ _idxSize }, submeshSize{ _submeshSize } {}
      uint64_t vtxSize, idxSize, submeshSize;
    };

    // used to read/write submesh data
    // we will copy the indices over from the base mesh during load
    struct SubmeshData
    {
      SubmeshData() = default;
      SubmeshData(uint32_t _baseVtx, uint32_t _baseIdx, uint32_t _vtxCount, uint32_t _idxCount, uint32_t _materialIdx)
        : baseVtx{ _baseVtx }, baseIdx{ _baseIdx }, vtxCount{ _vtxCount }, idxCount{ _idxCount }, materialIdx{ _materialIdx } {}

      uint32_t baseVtx, baseIdx, vtxCount, idxCount, materialIdx;
    };

    std::vector<Graphics::Vertex> mVertexBuffer;
    std::vector<uint32_t> mIndices;
    std::vector<SubmeshData> mSubmeshData;
    bool mStatus;

    static const unsigned sAssimpImportFlags, sMinimalAssimpImportFlags;

    void ProcessSubmeshes(aiNode* node, aiScene const* scene);
    void ProcessMeshes(aiNode* node, aiScene const* scene);
  };

} // namespace Graphics::AssetIO
