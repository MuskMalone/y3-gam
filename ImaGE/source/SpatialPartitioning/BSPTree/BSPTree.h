#pragma once
#include "BSPNode.h"
#include <string>

namespace Primitives { struct Triangle; struct Plane; }

// Spatial Partitioning
namespace SP
{

  class BSPTree
  {
  public:
    void BuildTree(std::vector<std::shared_ptr<Object>> const& objects);
    void Draw(Graphics::ShaderProgram& shaders);

    void LoadTree(std::string const& filepath);
    void SaveTree(std::string const& filepath);

  private:
    using NodePtr = BSPNode::NodePtr;
    using VertexList = std::vector<Primitives::Triangle>;

    static constexpr float STRADDLE_WEIGHT = 0.8f;

    enum RelationToPlane
    {
      COPLANAR,
      FRONT,
      BACK,
      STRADDLING
    };

    NodePtr m_root;

    NodePtr BuildTreeRecursive(VertexList&& triangles);
    Primitives::Plane ChooseSplitPlane(VertexList& triangles) const;
    void SplitTriangle(Primitives::Triangle const& triangle, Primitives::Plane const& plane, VertexList& frontList, VertexList& backList) const;

    RelationToPlane ClassifyPoint(glm::vec3 const& pt, Primitives::Plane plane) const;
    RelationToPlane ClassifyTriangle(Primitives::Triangle const& triangle, Primitives::Plane plane) const;

    Primitives::Plane AutoPartition(VertexList& triangles) const;
    void DeserializeNode(NodePtr& node, rapidjson::Value const& arr, rapidjson::SizeType idx = 0);
  };

} // namespace SP
