#include <pch.h>
#include "BottomUpConstruct.h"
#include <BoundingVolumes/AABB.h>
#include <BoundingVolumes/BoundingSphere.h>
#include <glm/gtc/constants.hpp>

namespace BV
{

  BottomUpConstruct::BottomUpConstruct() : BVHierarchy(BVHierarchy::Type::BOTTOM_UP)
  {

  }

  void BottomUpConstruct::Construct(Node& node, ContainerType const& objects)
  {
    std::vector<Node> nodes{};
    nodes.reserve(objects.size());
    unsigned depth{};
    for (unsigned i{}; i < objects.size(); ++i)
    {
      nodes.emplace_back(std::make_shared<TreeNode>(TreeNode::NodeType::LEAF));
      nodes.back()->data = objects[i];
      nodes.back()->numObjects = 1;
      // copy over the object's BV
      if (Settings::bvType == BVType::AABB) {
        nodes.back()->bv = std::make_shared<AABB>(*std::static_pointer_cast<AABB>(objects[i]->collider));
      }
      else {
        nodes.back()->bv = std::make_shared<BoundingSphere>(*std::static_pointer_cast<BoundingSphere>(objects[i]->collider));
      }
    }

    while (nodes.size() > 1)
    {
      Node node1, node2;
      MergeNodes(nodes, node1, node2);
      auto parent{ std::make_shared<TreeNode>(node1, node2) };
      parent->ComputeBottomUpBV();
      nodes.emplace_back(std::move(parent));
    }

    node = nodes.front();  // assign root node of the tree
    node->SetDepthAndColor();
  }

  void BottomUpConstruct::MergeNodes(std::vector<Node>& nodes, Node& node1, Node& node2) const
  {
    std::map<std::pair<unsigned, unsigned>, int> scores;

    if (Settings::nearestNeighbor)
    {
      auto results1{ NearestNeighbor(nodes) };
      AddScores(scores, results1);
    }

    if (Settings::combinedVolume)
    {
      auto results2{ MinimumCombinedVolume(nodes) };
      AddScores(scores, results2);
    }

    if (Settings::relativeIncrease)
    {
      auto results3{ MinRelativeVolIncrease(nodes) };
      AddScores(scores, results3);
    }

    auto winner{ std::min_element(scores.cbegin(), scores.cend(),
      [](std::pair<std::pair<unsigned, unsigned>, int> const& lhs, 
        std::pair<std::pair<unsigned, unsigned>, int> const& rhs) { return lhs.second < rhs.second; }) };

    unsigned idx1{ winner->first.first }, idx2{ winner->first.second };
    node1 = nodes[idx1];
    node2 = nodes[idx2];

    // ensure idx1 < idx2
    if (idx1 > idx2) { std::swap(idx1, idx2); }
    nodes.erase(nodes.begin() + idx2);
    nodes.erase(nodes.begin() + idx1);
  }

  void BottomUpConstruct::AddScores(std::map<std::pair<unsigned, unsigned>, int>& scores, std::vector<NodePair> const& results) const
  {
    for (unsigned i{}; i < results.size(); ++i) {
      scores[std::pair<unsigned, unsigned>(results[i].idx1, results[i].idx2)] += i;
    }
  }

  std::vector<BottomUpConstruct::NodePair> BottomUpConstruct::NearestNeighbor(std::vector<Node>& nodes) const
  {
    std::priority_queue<NodePair, std::vector<NodePair>, std::greater<NodePair>> distances;

    if (Settings::bvType == BVType::AABB)
    {
      for (unsigned i{}; i < nodes.size(); ++i)
      {
        auto bv1{ std::static_pointer_cast<AABB>(nodes[i]->bv) };

        for (unsigned j{ i + 1 }; j < nodes.size(); ++j)
        {
          auto bv2{ std::static_pointer_cast<AABB>(nodes[j]->bv) };
          glm::vec3 const dist{ bv1->worldPos - bv2->worldPos };

          distances.emplace(i, j, glm::dot(dist, dist));
        }
      }
    }
    else
    {
      for (unsigned i{}; i < nodes.size(); ++i)
      {
        auto bv1{ std::static_pointer_cast<BoundingSphere>(nodes[i]->bv) };

        for (unsigned j{ i + 1 }; j < nodes.size(); ++j)
        {
          auto bv2{ std::static_pointer_cast<BoundingSphere>(nodes[j]->bv) };
          glm::vec3 const dist{ bv1->center - bv2->center };

          distances.emplace(i, j, glm::dot(dist, dist));
        }
      }
    }

    std::vector<NodePair> results;
    results.reserve(distances.size());
    unsigned i{};
    while (!distances.empty())
    {
      if (i >= RANKING_CUTOFF) { break; }

      results.emplace_back(distances.top());
      distances.pop();
      ++i;
    }

    return results;
  }

  std::vector<BottomUpConstruct::NodePair> BottomUpConstruct::MinimumCombinedVolume(std::vector<Node>& nodes) const
  {
    std::priority_queue<NodePair, std::vector<NodePair>, std::greater<NodePair>> volumes;

    if (Settings::bvType == BVType::AABB)
    {
      for (unsigned i{}; i < nodes.size(); ++i)
      {
        auto bv1{ std::static_pointer_cast<AABB>(nodes[i]->bv) };
        glm::vec3 const bv1Min{ bv1->GetWorldMin() }, bv1Max{ bv1->GetWorldMax() };

        for (unsigned j{ i + 1 }; j < nodes.size(); ++j)
        {
          auto bv2{ std::static_pointer_cast<AABB>(nodes[j]->bv) };
          glm::vec3 const min{ glm::min(bv1Min, bv2->GetWorldMin()) }, max{ glm::max(bv1Max, bv2->GetWorldMax()) };
          glm::vec3 const extents{ max - min };

          volumes.emplace(i, j, extents.x * extents.y * extents.z);
        }
      }
    }
    else
    {
      for (unsigned i{}; i < nodes.size(); ++i)
      {
        auto bv1{ std::static_pointer_cast<BoundingSphere>(nodes[i]->bv) };

        for (unsigned j{ i + 1 }; j < nodes.size(); ++j)
        {
          auto bv2{ std::static_pointer_cast<BoundingSphere>(nodes[j]->bv) };
          float const combinedRad{ glm::distance(bv1->worldCenter, bv2->worldCenter) + bv1->worldRadius + bv2->worldRadius };

          volumes.emplace(i, j, combinedRad * combinedRad * glm::pi<float>());
        }
      }
    }

    std::vector<NodePair> results;
    results.reserve(volumes.size());
    while (!volumes.empty())
    {
      results.emplace_back(volumes.top());
      volumes.pop();
    }

    return results;
  }

  std::vector<BottomUpConstruct::NodePair> BottomUpConstruct::MinRelativeVolIncrease(std::vector<Node>& nodes) const
  {
    std::priority_queue<NodePair, std::vector<NodePair>, std::greater<NodePair>> volumes;

    if (Settings::bvType == BVType::AABB)
    {
      for (unsigned i{}; i < nodes.size(); ++i)
      {
        auto bv1{ std::static_pointer_cast<AABB>(nodes[i]->bv) };
        float const bv1Vol{ bv1->GetWorldVolume() };
        glm::vec3 const bv1Min{ bv1->GetWorldMin() }, bv1Max{ bv1->GetWorldMax() };

        for (unsigned j{ i + 1 }; j < nodes.size(); ++j)
        {
          auto bv2{ std::static_pointer_cast<AABB>(nodes[j]->bv) };
          // get current sum of indiv. vols
          float const sumOfVol{ bv1Vol + bv2->GetWorldVolume() };
          glm::vec3 const combinedMin{ glm::min(bv1Min, bv2->GetWorldMin()) }, combinedMax{ glm::max(bv1Max, bv2->GetWorldMax()) };
          glm::vec3 const combinedExtents{ combinedMax - combinedMin };
          float const newVol{ combinedExtents.x * combinedExtents.y * combinedExtents.z };
          // add entry with the difference in volumes
          volumes.emplace(i, j, newVol - sumOfVol);
        }
      }
    }
    else
    {
      for (unsigned i{}; i < nodes.size(); ++i)
      {
        auto bv1{ std::static_pointer_cast<BoundingSphere>(nodes[i]->bv) };
        float const bv1Vol{ bv1->GetWorldVolume() };
        for (unsigned j{ i + 1 }; j < nodes.size(); ++j)
        {
          auto bv2{ std::static_pointer_cast<BoundingSphere>(nodes[j]->bv) };
          // get current sum of indiv. vols
          float const sumOfVol{ bv1Vol + bv2->GetWorldVolume() };
          // get the new BV's vol
          float const combinedRad{ glm::distance(bv1->worldCenter, bv2->worldCenter) + bv1->worldRadius + bv2->worldRadius };
          float const newVol{ combinedRad * combinedRad * glm::pi<float>() };
          // add entry with the difference in volumes
          volumes.emplace(i, j, newVol - sumOfVol);
        }
      }
    }

    std::vector<NodePair> results;
    results.reserve(volumes.size());
    while (!volumes.empty())
    {
      results.emplace_back(volumes.top());
      volumes.pop();
    }

    return results;
  }

} // namespace BV
