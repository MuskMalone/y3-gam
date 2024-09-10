#include <pch.h>
#include "BSPTree.h"
#include <Core/Object.h>
#include <Intersection/IntersectionTests.h>
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>
//#define BSP_DEBUG

#ifdef BSP_DEBUG
#include <iostream>
#endif

// Spatial Partitioning
namespace SP
{

  void BSPTree::BuildTree(std::vector<std::shared_ptr<Object>> const& objects)
  {
    VertexList triangles;
    for (auto const& obj : objects)
    {
      auto proxyObj{ std::make_shared<ProxyObj>(obj) };
      auto const& vertices{ proxyObj->GetVertices() };
      auto const& indices{ proxyObj->GetIndices() };
      glm::mat3 const xform{ obj->mdlTransform };

      triangles.reserve(triangles.size() + indices.size() / 3);
      for (unsigned i{}; i < indices.size(); i += 3)
      {
        triangles.emplace_back(xform * vertices[indices[i]], xform * vertices[indices[i + 1]], xform * vertices[indices[i + 2]]);
      }
    }

     m_root = BuildTreeRecursive(std::move(triangles));
     m_root->Initialize();
  }

  void BSPTree::Draw(Graphics::ShaderProgram& shaders)
  {
    m_root->Draw(shaders);
  }

  BSPTree::NodePtr BSPTree::BuildTreeRecursive(VertexList&& triangles)
  {
#ifdef BSP_DEBUG
    std::cout << "SIZE: " << triangles.size() << "\n";
    int fCount{}, bCount{};
#endif
    // terminate when below triangle threshold
    if (triangles.size() <= Settings::trianglesPerBSPTreeCell) {
      auto newNode{ std::make_shared<BSPNode>() };
      newNode->vertices.reserve(triangles.size() * 3);
      for (auto const& tri : triangles)
      {
        newNode->vertices.insert(newNode->vertices.end(), tri.points, tri.points + 3);
      }
      return newNode;
    }

    auto const chosenPlane{ ChooseSplitPlane(triangles) };
    VertexList frontList, backList;

    for (auto& tri : triangles)
    {
      // classify into front / back
      switch (ClassifyTriangle(tri, chosenPlane))
      {
      case RelationToPlane::COPLANAR:
      case RelationToPlane::FRONT:
#ifdef BSP_DEBUG
        ++fCount;
#endif
        frontList.emplace_back(std::move(tri));
        break;
      case RelationToPlane::BACK:
#ifdef BSP_DEBUG
        ++bCount;
#endif
        backList.emplace_back(std::move(tri));
        break;
      case RelationToPlane::STRADDLING:
        SplitTriangle(tri, chosenPlane, frontList, backList);
        break;
      }
    }

#ifdef BSP_DEBUG
    std::cout << "Front: " << fCount << ", Back: " << bCount << "\n";
#endif

    triangles.clear();
    // recurse into subtrees, pushing polygon into leaves
    NodePtr frontNode{ BuildTreeRecursive(std::move(frontList)) };
    NodePtr backNode{ BuildTreeRecursive(std::move(backList)) };

    return std::make_shared<BSPNode>(frontNode, backNode);
  }

  void BSPTree::SplitTriangle(Primitives::Triangle const& triangle, Primitives::Plane const& plane, VertexList& frontList, VertexList& backList) const
  {
    std::vector<glm::vec3> frontVertices, backVertices;
    glm::vec3 a{ triangle[2] };
    RelationToPlane aSide{ ClassifyPoint(a, plane) };

    for (int i{}; i < 3; ++i)
    {
      glm::vec3 const& b{ triangle[i] };
      RelationToPlane const bSide{ ClassifyPoint(b, plane) };

      switch (bSide)
      {
      case RelationToPlane::FRONT:
        if (aSide == RelationToPlane::BACK)
        {
          // edge(a, b) straddles plane
          auto const interPt{ Intersection::LineSegmentToPlane(a, b, plane) };
          frontVertices.emplace_back(interPt);
          backVertices.emplace_back(interPt);
        }

        // output b to both sides in all cases
        frontVertices.emplace_back(b);
        break;
      case RelationToPlane::BACK:
        if (aSide == RelationToPlane::FRONT)
        {
          // edge(a, b) straddles plane
          auto const interPt{ Intersection::LineSegmentToPlane(a, b, plane) };
          frontVertices.emplace_back(interPt);
          backVertices.emplace_back(interPt);
        }
        else if (aSide == RelationToPlane::COPLANAR) {
          // output an edge if it goes from on to behind the plane
          backVertices.emplace_back(a);
        }
        // in all 3 cases, output b to back
        backVertices.emplace_back(b);
        break;
      case COPLANAR:
      default:
        // b is on the plane
        // in all 3 cases, output b to front
        frontVertices.emplace_back(b);

        // in one case, output b to back
        if (aSide == RelationToPlane::BACK) {
          backVertices.emplace_back(b);
        }
        break;
      }

      // keep b as starting point of next edge
      a = b;
      aSide = bSide;
    }

    // triangulate resulting points
    if (frontVertices.size() >= 3) {
      for (unsigned i{ 2 }; i < frontVertices.size(); ++i) {
        frontList.emplace_back(frontVertices[0], frontVertices[i - 1], frontVertices[i]);
      }
    }
    if (backVertices.size() >= 3) {
      for (unsigned i{ 2 }; i < backVertices.size(); ++i) {
        backList.emplace_back(backVertices[0], backVertices[i - 1], backVertices[i]);
      }
    }
  }

  Primitives::Plane BSPTree::ChooseSplitPlane(VertexList& triangles) const
  {
    return AutoPartition(triangles);
  }

  Primitives::Plane BSPTree::AutoPartition(VertexList& triangles) const
  {
    Primitives::Plane bestPlane{};
    float bestScore{ FLT_MAX };

    for (unsigned i{}; i < triangles.size(); ++i)
    {
      int frontCount{}, backCount{}, straddleCount{};
      Primitives::Plane currPlane{ triangles[i] };

      for (unsigned j{}; j < triangles.size(); ++j)
      {
        if (i == j) { continue; }

        switch (ClassifyTriangle(triangles[j], currPlane))
        {
        case COPLANAR:
        case FRONT:
          ++frontCount;
          break;
        case BACK:
          ++backCount;
          break;
        case STRADDLING:
          ++straddleCount;
          break;
        }
      }

      float const score{ STRADDLE_WEIGHT * straddleCount + (1.f - STRADDLE_WEIGHT) * abs(frontCount - backCount) };
      if (score < bestScore) {
        bestPlane = std::move(currPlane);
        bestScore = score;
      }
    }

    return bestPlane;
  }

  BSPTree::RelationToPlane BSPTree::ClassifyPoint(glm::vec3 const& pt, Primitives::Plane plane) const
  {
    float const dist{ plane.GetPointDist(pt) };
    if (dist > EPSILON<float>) {
      return RelationToPlane::FRONT;
    }
    else if (dist < -EPSILON<float>) {
      return RelationToPlane::BACK;
    }

    return RelationToPlane::COPLANAR;
  }

  BSPTree::RelationToPlane BSPTree::ClassifyTriangle(Primitives::Triangle const& triangle, Primitives::Plane plane) const
  {
    int frontCount{}, backCount{};
    for (unsigned i{}; i < 3; ++i)
    {
      auto const result{ ClassifyPoint(triangle[i], plane) };
      if (result == RelationToPlane::FRONT) {
        ++frontCount;
      }
      else if (result == RelationToPlane::BACK) {
        ++backCount;
      }
    }

    // if vertices are on both sides of plane, it is straddling
    if (frontCount != 0 && backCount != 0) {
      return RelationToPlane::STRADDLING;
    }

    // else if >= 1 vertices in front and no vertices behind,
    // triangle lies in front
    if (frontCount != 0) { return RelationToPlane::FRONT; }
    // same for back
    else if (backCount != 0) { return RelationToPlane::BACK; }

    // here, all vertices lie on the plane
    return RelationToPlane::COPLANAR;
  }

  void BSPTree::LoadTree(std::string const& filepath)
  {
    rapidjson::Document document{};
    std::ifstream ifs{ filepath };
    if (!ifs)
    {
#ifdef _DEBUG
      std::cout << "Unable to read " << filepath << "\n";
#endif
      throw std::runtime_error("Unable to read " + filepath);
    }
    // parse into document object
    rapidjson::IStreamWrapper isw{ ifs };

    if (document.ParseStream(isw).HasParseError())
    {
      ifs.close();
#ifdef _DEBUG
      std::cout << "Unable to parse " + filepath << "\n";
#endif
      throw std::runtime_error("Unable to parse " + filepath);
    }

    m_root = std::make_shared<BSPNode>();
    m_root->Deserialize(document.GetArray());
    m_root->Initialize();

    ifs.close();
  }

  void BSPTree::SaveTree(std::string const& filepath)
  {
    rapidjson::Document document{ rapidjson::kArrayType };
    std::ofstream ofs{ filepath };
    if (!ofs) {
#ifdef _DEBUG
      std::cout << "Unable to read " << filepath << "\n";
#endif
      throw std::runtime_error("Unable to read " + filepath);
    }

    m_root->Serialize(document, document.GetAllocator());

    rapidjson::OStreamWrapper osw{ ofs };
    rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
    document.Accept(writer);

    ofs.close();
  }

} // namespace SP
