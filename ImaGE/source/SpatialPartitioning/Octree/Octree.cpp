#include <pch.h>
#include "Octree.h"
#include <Intersection/IntersectionTests.h>
#include <glm/gtx/norm.hpp>
#include <BoundingVolumes/AABB.h>

namespace
{
  std::vector<Primitives::Triangle> IndicesToTriangles(std::vector<glm::vec3> const& vertices,
    std::vector<GLushort> const& indices, glm::mat3 const& xform);
}

// Spatial Partitioning
namespace SP
{

  void Octree::BuildTree(std::vector<std::shared_ptr<Object>> const& objects)
  {
    m_root = std::make_shared<OctNode>();
    auto const minMax = GetObjectsMinMax(objects);
    m_root->center = (minMax.first + minMax.second) * 0.5f;
    glm::vec3 const halfExtents{ minMax.second - m_root->center };
    m_root->halfWidth = std::max(halfExtents.x, std::max(halfExtents.y, halfExtents.z));
    m_root->ComputeBV();

    for (auto const& obj : objects)
    {
      auto proxyObj{ std::make_shared<ProxyObj>(obj) };
      auto const& vertices{ proxyObj->GetVertices() };
      auto const& indices{ proxyObj->GetIndices() };
      glm::mat3 const xform{ obj->mdlTransform };
#ifdef RECURSE_PER_TRIANGLE
      // recursively insert every triangle into tree
      for (unsigned i{}; i < indices.size(); i += 3)
      {
        InsertRecursive(m_root, proxyObj, { xform * vertices[indices[i]], xform * vertices[indices[i + 1]], xform * vertices[indices[i + 2]] });
      }
#else
      std::vector<Primitives::Triangle> triangles;
      triangles.reserve(objects.size() / 3);
      for (unsigned i{}; i < indices.size(); i += 3)
      {
        triangles.emplace_back(xform * vertices[indices[i]], xform * vertices[indices[i + 1]], xform * vertices[indices[i + 2]]);
      }
      InsertRecursive(m_root, proxyObj, triangles);
#endif
    }

    m_root->Initialize();
  }

  void Octree::Draw(Graphics::ShaderProgram& shader)
  {
    m_root->Draw(shader);
  }

#ifdef RECURSE_PER_TRIANGLE
  void Octree::InsertRecursive(NodePtr& tree, ProxyObj::Pointer const& obj, Primitives::Triangle const& triangle)
  {
    // stop if triangle count reached
    if (tree->GetTriangleCount() <= Settings::trianglesPerOctTreeCell) {
      //tree->AddObject(obj);
      // add vertices to the current node
      tree->vertices.insert(tree->vertices.end(), triangle.points, triangle.points + 3);
      return;
    }

    if (!tree->children[0]) {
      tree->CreateChildren();
    }

    auto const straddleResult{ DoesTriangleStraddle(triangle, tree) };
    // if straddling, split the triangle
    if (straddleResult.first)
    {
      std::array<std::vector<Primitives::Triangle>, 8> splitTriangles;
      // check intersection with planes
      SplitTriangle(tree, triangle, splitTriangles);

      for (unsigned i{}; i < 8; ++i)
      {
        if (splitTriangles[i].empty()) { continue; }

        for (auto const& splitTri : splitTriangles[i])
        {
          InsertRecursive(tree->children[i], obj, splitTri);
        }
      }
    }
    // if completely contained, continue into the subtree
    else
    {
      InsertRecursive(tree->children[straddleResult.second.x], obj, triangle);
    }
  }
#else
  void Octree::InsertRecursive(NodePtr& tree, ProxyObj::Pointer const& obj, std::vector<Primitives::Triangle> const& triangles)
  {
    // stop if triangle count reached
    if (tree->GetTriangleCount() <= Settings::trianglesPerOctTreeCell) {
      tree->AddObject(obj);
      // add triangles to the current node
      for (auto const& tri : triangles) {
        tree->vertices.insert(tree->vertices.end(), tri.points, tri.points + 3);
      }

      return;
    }

    if (!tree->children[0]) {
      tree->CreateChildren();
    }

    std::array<std::vector<Primitives::Triangle>, 8> splitTriangles;
    for (auto const& tri : triangles)
    {
      auto const straddleResult{ DoesTriangleStraddle(tri, tree) };

      if (straddleResult.first)
      {
        // if straddling, split the triangle
        SplitTriangle(tree, tri, splitTriangles);
      }
      // if completely contained, add the whole triangle into the subtree
      else
      {
        splitTriangles[straddleResult.second.x].emplace_back(tri);
      }
    }

    // recursively insert each octant's triangles
    for (unsigned i{}; i < 8; ++i)
    {
      if (splitTriangles[i].empty()) { continue; }

      InsertRecursive(tree->children[i], obj, splitTriangles[i]);
    }
  }
#endif

#ifdef RECURSE_PER_TRIANGLE
  void Octree::SplitTriangle(NodePtr& node, Primitives::Triangle const& triangle, std::array<std::vector<Primitives::Triangle>, 8>& splitTriangle) const
  {
    Primitives::Plane planes[6];

    // we check intersection between each ray and every child octant
    // (because idk how to optimize it)
    for (unsigned oct{}; oct < 8; ++oct)
    {
      node->children[oct]->SetNodePlanes(planes);
      std::vector<glm::vec3> currPolygon{ triangle.points, triangle.points + 3 };

      for (auto const& plane : planes)
      {
        std::vector<glm::vec3> newVertices;
        newVertices.reserve(currPolygon.size());
        std::vector<uint8_t> octCodes;
        octCodes.reserve(currPolygon.size());
        for (unsigned i{}; i < currPolygon.size(); ++i)
        {
          octCodes.emplace_back(GetOctCode(node, currPolygon[i]));
        }

        for (unsigned i{}; i < currPolygon.size(); ++i)
        {
          unsigned const v1Idx{ (i + 1) % currPolygon.size() };
          glm::vec3 const& v0{ currPolygon[i] }, & v1{ currPolygon[v1Idx] };

          // if both points lie in octant, accept it
          if (octCodes[i] == oct && octCodes[v1Idx] == oct) {
            newVertices.emplace_back(v0);
            newVertices.emplace_back(v1);
            continue;
          }

          Primitives::Ray const ray{ v0, v1 - v0 };
          float const d0{ plane.GetPointDist(v0) }, d1{ plane.GetPointDist(v1) };
          // if both points outside the plane, reject them
          if (d0 > 0.f && d1 > 0.f) {
            continue;
          }

          auto const result{ Intersection::RayToPlane(ray, plane) };
          if (!result) continue;

          // line intersects the plane, insert the intersection pt
          newVertices.emplace_back(result.interPt);
          // if ray goes outside -> inside, insert v1
          if (d0 > 0.f) {
            newVertices.emplace_back(v1);
          }
        }

        if (!newVertices.empty()) {
          currPolygon = std::move(newVertices);
        }
      }

      if (currPolygon.size() >= 3) {
        splitTriangle[oct].reserve((currPolygon.size() - 2) * 3);
        for (unsigned i{ 2 }; i < currPolygon.size(); ++i) {
          splitTriangle[oct].emplace_back(currPolygon[0], currPolygon[i - 1], currPolygon[i]);
        }
      }
    }
  }
#else
  void Octree::SplitTriangle(NodePtr& node, Primitives::Triangle const& triangle, std::array<std::vector<Primitives::Triangle>, 8>& splitTriangle) const
  {
    Primitives::Plane planes[6];

    // we check intersection between each ray and every child octant
    // (because idk how to optimize it)
    for (unsigned oct{}; oct < 8; ++oct)
    {
      node->children[oct]->SetNodePlanes(planes);
      std::vector<glm::vec3> currPolygon{ triangle.points, triangle.points + 3 };

      for (auto const& plane : planes)
      {
        std::vector<glm::vec3> newVertices;
        newVertices.reserve(currPolygon.size());
        std::vector<uint8_t> octCodes;
        octCodes.reserve(currPolygon.size());
        for (unsigned i{}; i < currPolygon.size(); ++i)
        {
          octCodes.emplace_back(GetOctCode(node, currPolygon[i]));
        }

        for (unsigned i{}; i < currPolygon.size(); ++i)
        {
          unsigned const v1Idx{ (i + 1) % currPolygon.size() };
          glm::vec3 const& v0{ currPolygon[i] }, & v1{ currPolygon[v1Idx] };

          // if both points lie in octant, accept it
          if (octCodes[i] == oct && octCodes[v1Idx] == oct) {
            newVertices.emplace_back(v0);
            newVertices.emplace_back(v1);
            continue;
          }

          float const d0{ plane.GetPointDist(v0) };
          // if both points outside the plane, reject them
          if (d0 > 0.f && plane.GetPointDist(v1) > 0.f) {
            continue;
          }

          Primitives::Ray const ray{ v0, v1 - v0 };
          auto const result{ Intersection::RayToPlane(ray, plane) };
          if (!result) { continue; }

          // line intersects the plane, insert the intersection pt
          newVertices.emplace_back(result.interPt);
          // if ray goes outside -> inside, insert v1
          if (d0 > 0.f) {
            newVertices.emplace_back(v1);
          }
        }

        if (!newVertices.empty()) {
          currPolygon = std::move(newVertices);
        }
      }

      if (currPolygon.size() >= 3) {
        for (unsigned i{ 2 }; i < currPolygon.size(); ++i) {
          splitTriangle[oct].emplace_back(currPolygon[0], currPolygon[i - 1], currPolygon[i]);
        }
      }
    }
  }
#endif

  uint8_t Octree::GetOctCode(NodePtr const& node, glm::vec3 const& pt) const
  {
    uint8_t ret{};
    for (unsigned axis{}; axis < 3; ++axis)
    {
      // for each vtx, compare the distance to the node's center
      if (pt[axis] - node->center[axis] > 0.f) {
        ret |= (1 << axis);
      }
    }

    return ret;
  }

  std::pair<bool, glm::u8vec3> Octree::DoesTriangleStraddle(Primitives::Triangle const& triangle, NodePtr const& node) const
  {
    glm::u8vec3 octant{ 0, 0, 0 };
    bool straddle{ false };

    // for each axis, check against each vertex
    for (unsigned axis{}; axis < 3; ++axis)
    {
      float const centerCoord{ node->center[axis] };
      // for each vtx, compare the distance to the node's center
      for (unsigned i{}; i < 3; ++i)
      {
        if (triangle[i][axis] - centerCoord > 0.f) {
          octant[i] |= (1 << axis);
        }
      }
    }

    // if points lie in different octants, triangle is straddling
    if (octant[0] != octant[1] || octant[0] != octant[2]) {
      straddle = true;
    }

    return { straddle, octant };
  }

  std::pair<glm::vec3, glm::vec3> Octree::GetObjectsMinMax(std::vector<std::shared_ptr<Object>> const& objects) const
  {
    glm::vec3 min{ FLT_MAX }, max{ -FLT_MAX };
    for (auto const& obj : objects)
    {
      auto aabb{ std::static_pointer_cast<BV::AABB>(obj->collider) };
      min = glm::min(aabb->GetWorldMin(), min);
      max = glm::max(aabb->GetWorldMax(), max);
    }

    return { min, max };
  }

} // namespace SP


namespace
{
  std::vector<Primitives::Triangle> IndicesToTriangles(std::vector<glm::vec3> const& vertices,
    std::vector<GLushort> const& indices, glm::mat3 const& xform)
  {
    std::vector<Primitives::Triangle> ret;
    ret.reserve(vertices.size() / 3);

    for (unsigned i{}; i < indices.size(); i += 3)
    {
      glm::vec3 const& v1{ vertices[indices[i]] }, & v2{ vertices[indices[i + 1]] }, & v3{ vertices[indices[i + 2]] };
      ret.emplace_back(xform * v1, xform * v2, xform * v3);
    }

    return ret;
  }
}