#pragma once

namespace Graphics
{

  enum class MeshType
  {
    POINT = 0,
    LINE,
    CUBE,
    SPHERE,
    PLANE,
    TRIANGLE,
    NUM_TYPES
  };

  constexpr const char* MESH_NAMES[]{
    "Point3D", "Ray", "AABB", "Bounding Sphere", "Plane", "Triangle"
  };

} // namespace Graphics