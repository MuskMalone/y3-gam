/*!*********************************************************************
\file   MeshConverter.h
\author chengen.lau\@digipen.edu
\date   20-September-2024
\brief  
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#ifndef IMGUI_DISABLE
#include <memory>
#include <Graphics/MeshSource.h>

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


  class MeshConverter
  {
  public:
    MeshConverter(std::string const& file, MeshImportFlags const& = {});
    operator bool() const { return mStatus; } // check if object is valid

    std::shared_ptr<MeshSource> GetMeshSource() const;

  private:
    std::vector<Graphics::Vertex> mVertexBuffer;
    std::vector<uint32_t> mIndices;
    std::vector<Submesh> mSubmeshes;
    bool mStatus;

    static unsigned sAssimpImportFlags;
  };

} // namespace Graphics::AssetIO

#endif  // IMGUI_DISABLE
