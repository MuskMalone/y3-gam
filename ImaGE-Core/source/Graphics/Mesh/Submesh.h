/*!*********************************************************************
\file   Submesh.h
\author k.choa\@digipen.edu
\date   22/09/2024
\brief  The Submesh struct holds the data necessary to render a specific part of a mesh. It includes
        the starting vertex and index positions, the number of vertices and indices, and a transform
        matrix. It also supports storing the indices directly within the submesh for flexibility.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <glm/glm.hpp>
#include <cstdint>
#include <vector>

namespace Graphics{
  struct Submesh {
    uint32_t baseVtx;  // Start vertex in the vertex buffer
    uint32_t baseIdx;   // Start index in the index buffer !NOTE may be useless since im storing mIdices
    uint32_t vtxCount;
    uint32_t idxCount;  // Number of indices this submesh draws
    uint32_t materialIdx;  // Reference to the material to use
    glm::mat4 transform;  // Transform of the submesh, default to identity matrix
    std::vector<uint32_t> mIndices; // Indices for this submesh @TODO OPTIMIZE

    // Constructors
    Submesh() : baseVtx{}, baseIdx{}, vtxCount{}, idxCount{}, materialIdx{}, transform{ glm::mat4{ 1.f } }, mIndices{} {}
    Submesh(uint32_t vtx, uint32_t idx, uint32_t vtxCnt, uint32_t idxCnt, uint32_t matIdx, const glm::mat4& xform, const std::vector<uint32_t>& indices)
        : baseVtx{ vtx }, baseIdx{ idx }, vtxCount{ vtxCnt }, idxCount{ idxCnt }, materialIdx{ matIdx }, transform{ xform }, mIndices{indices} {}
  };
}
