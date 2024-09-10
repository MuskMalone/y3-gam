#pragma once

namespace BV
{

  enum class BVType
  {
    AABB = 0,
    RITTER,
    LARSSON,
    PCA,
    OBB,
    ELLIPSOID,
    NUM_TYPES
  };

  constexpr const char* BV_TYPE_NAMES[]{
    "AABB", "RITTER'S", "LARSSON'S", "PCA", "OBB", "ELLIPSOID"
  };
    
} // namespace BV
