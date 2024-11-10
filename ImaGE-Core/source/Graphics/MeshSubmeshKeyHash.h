#pragma once
// MeshSubmeshKeyHash.h

#include <utility>      // For std::pair
#include <functional>   // For std::hash
#include "Asset/AssetManager.h"

// Define MeshSubmeshKey for convenience
using MeshSubmeshKey = std::pair<IGE::Assets::GUID, uint32_t>;

namespace std {
    template <>
    struct hash<MeshSubmeshKey> {
        size_t operator()(const MeshSubmeshKey& key) const noexcept {
            size_t hash1 = std::hash<IGE::Assets::GUID>()(key.first);
            size_t hash2 = std::hash<uint32_t>()(key.second);
            return hash1 ^ (hash2 << 1); // Combine the two hashes
        }
    };
}
