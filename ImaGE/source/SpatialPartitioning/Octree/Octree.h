#pragma once
#include "OctNode.h"
#include <glm/glm.hpp>
#include <vector>
#include <array>

//#define RECURSE_PER_TRIANGLE

// forward declaration
namespace Primitives { struct Triangle; }

// Spatial Partitioning
namespace SP
{

  class Octree
  {
  public:
    void BuildTree(std::vector<std::shared_ptr<Object>> const& objects);
    void Draw(Graphics::ShaderProgram& shader);

  private:
    using NodePtr = OctNode::NodePtr;

    NodePtr m_root;

    std::pair<glm::vec3, glm::vec3> GetObjectsMinMax(std::vector<std::shared_ptr<Object>> const& objects) const;
#ifdef RECURSE_PER_TRIANGLE
    void InsertRecursive(NodePtr& tree, ProxyObj::Pointer const& obj, Primitives::Triangle const& triangle);
#else
    void InsertRecursive(NodePtr& tree, ProxyObj::Pointer const& obj, std::vector<Primitives::Triangle> const& triangles);
#endif

    uint8_t GetOctCode(NodePtr const& node, glm::vec3 const& pt) const;
    std::pair<bool, glm::u8vec3> DoesTriangleStraddle(Primitives::Triangle const& triangle, NodePtr const& node) const;
    void SplitTriangle(NodePtr& node, Primitives::Triangle const& triangle, std::array<std::vector<Primitives::Triangle>, 8>& splitTriangle) const;
  };

} // namespace SP
