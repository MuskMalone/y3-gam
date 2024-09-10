#include <pch.h>
#include "TopDownConstruct.h"
#include <BoundingVolumes/AABB.h>
#include <BoundingVolumes/BoundingSphere.h>

#define HEURISTIC_DEBUG

namespace BV
{
  TopDownConstruct::SplitPlaneFunc TopDownConstruct::m_methods[SplitMethod::NUM_METHODS]{
    &TopDownConstruct::MedianOfCenters, &TopDownConstruct::MedianOfExtents, &TopDownConstruct::KEvenSplits
  };

  TopDownConstruct::TopDownConstruct() : BVHierarchy(BVHierarchy::Type::TOP_DOWN),
    m_chosenAxis{ ChosenAxis::X_AXIS }
  {
    
  }

  void TopDownConstruct::Construct(Node& node, ContainerType const& objects)
  {
    auto cpy{ objects };
    ConstructTopDownTree(node, cpy.begin(), static_cast<unsigned>(cpy.size()));
  }

  void TopDownConstruct::ConstructTopDownTree(Node& node,
    ContainerType::iterator object, unsigned objectCount, unsigned level)
  {
    node = std::make_shared<TreeNode>(level);
    node->ComputeTopDownBV(object, object + objectCount);
    node->SetColor();

    node->numObjects = objectCount;
    if (level++ >= MAX_HEIGHT || objectCount <= MIN_OBJECTS_AT_LEAF)
    {
      node->type = TreeNode::LEAF;
      node->data = *object;
    }
    else
    {
      node->type = TreeNode::INTERNAL;

      // use partioning strategy to arrange objects into 2 partitions
      auto grp2{ objectCount == 2 ? object + 1 : PartitionObjects(object, objectCount) };

      // recursively construct child nodes
      unsigned const grp1Count{ static_cast<unsigned>(grp2 - object) };
      ConstructTopDownTree(node->leftChild, object, grp1Count, level);
      ConstructTopDownTree(node->rightChild, grp2, objectCount - grp1Count, level);
    }
  }

  TopDownConstruct::ContainerType::iterator TopDownConstruct::PartitionObjects(ContainerType::iterator object, unsigned numObjects)
  {
    DetermineAxis(object, numObjects);
    return (this->*m_methods[Settings::splitMethod])(object, numObjects);
  }

  TopDownConstruct::ContainerType::iterator TopDownConstruct::MedianOfCenters(ContainerType::iterator object, unsigned numObjects)
  {
    unsigned const middleIdx{ numObjects / 2 };
    int const chosenAxis{ static_cast<int>(m_chosenAxis) };
    if (Settings::bvType == BVType::AABB)
    {
      std::nth_element(object, object + middleIdx, object + numObjects,
        [chosenAxis](std::shared_ptr<Object> const& lhs, std::shared_ptr<Object> const& rhs) { return std::static_pointer_cast<AABB const>(lhs->collider)->worldPos[chosenAxis]
          < std::static_pointer_cast<AABB const>(rhs->collider)->worldPos[chosenAxis]; });
    }
    else
    {
      // else if bounding sphere
      std::nth_element(object, object + middleIdx, object + numObjects,
        [chosenAxis](std::shared_ptr<Object> const& lhs, std::shared_ptr<Object> const& rhs) { return std::static_pointer_cast<BoundingSphere const>(lhs->collider)->worldCenter[chosenAxis]
        < std::static_pointer_cast<BoundingSphere const>(rhs->collider)->worldCenter[chosenAxis]; });
    }

    return object + middleIdx;
  }

  TopDownConstruct::ContainerType::iterator TopDownConstruct::MedianOfExtents(ContainerType::iterator object, unsigned numObjects)
  {
    unsigned const middleIdx{ numObjects / 2 };
    int const chosenAxis{ static_cast<int>(m_chosenAxis) };
    std::vector<Extent> extents;
    extents.reserve(numObjects * 2);
    if (Settings::bvType == BVType::AABB)
    {
      for (auto start{ object }, end{ object + numObjects }; start != end; ++start)
      {
        auto aabb{ std::static_pointer_cast<AABB>((*start)->collider) };
        float const maxExtent{ aabb->worldPos[chosenAxis] + aabb->worldHalfExtents[chosenAxis] };
        extents.emplace_back(maxExtent, start - object, true);
        // insert the max extents into a separate vector
        // we will sort the objects by their max extents in the end
        extents.emplace_back(aabb->worldPos[chosenAxis] - aabb->worldHalfExtents[chosenAxis], start - object, false);
      }

      size_t const extentsMid{ extents.size() / 2 };

      std::nth_element(extents.begin(), extents.begin() + extentsMid, extents.end());
      float const pivotCenter{ std::static_pointer_cast<AABB>((*(object + extents[extentsMid].index))->collider)->worldPos[chosenAxis] };

      auto returnIdx = std::partition(object, object + numObjects, [chosenAxis, pivotCenter](ContainerType::value_type const& obj) {
        auto aabb{ std::static_pointer_cast<AABB>(obj->collider) };
        return aabb->worldPos[chosenAxis] <= pivotCenter;
        });

      // if the last element was chosen, we partition as n-1, 1
      if (returnIdx == object + numObjects) { --returnIdx; }

      return returnIdx;
    }
    
    for (auto start{ object }, end{ object + numObjects }; start != end; ++start)
    {
      auto bs{ std::static_pointer_cast<BoundingSphere>((*start)->collider) };
      float const maxExtent{ bs->worldCenter[chosenAxis] + bs->worldRadius };
      extents.emplace_back(maxExtent, start - object, true);
      // insert the max extents into a separate vector
      // we will sort the objects by their max extents in the end
      extents.emplace_back(bs->worldCenter[chosenAxis] - bs->worldRadius, start - object, false);
    }

    size_t const extentsMid{ extents.size() / 2 };

    std::nth_element(extents.begin(), extents.begin() + extentsMid, extents.end());
    float const pivotCenter{ std::static_pointer_cast<BoundingSphere>((*(object + extents[extentsMid].index))->collider)->worldCenter[chosenAxis] };

    auto returnIdx = std::partition(object, object + numObjects, [chosenAxis, pivotCenter](ContainerType::value_type const& obj) {
      auto bs{ std::static_pointer_cast<BoundingSphere>(obj->collider) };
      return bs->worldCenter[chosenAxis] <= pivotCenter;
      });

    // if the last element was chosen, we partition as n-1, 1
    if (returnIdx == object + numObjects) { --returnIdx; }

    return returnIdx;
  }

  TopDownConstruct::ContainerType::iterator TopDownConstruct::KEvenSplits(ContainerType::iterator object, unsigned numObjects)
  {
    float min{ FLT_MAX }, max{ -FLT_MAX };
    int const chosenAxis{ m_chosenAxis };

    if (Settings::bvType == BVType::AABB)
    {
      ContainerType cpy{ object, object + numObjects };
      auto const idealPartition{ cpy.begin() + numObjects / 2 };
      for (auto start{ cpy.begin() }; start != cpy.end(); ++start)
      {
        auto bs{ std::static_pointer_cast<AABB>((*start)->collider) };

        float const minExtent{ bs->worldPos[chosenAxis] - bs->worldHalfExtents[chosenAxis] };
        float const maxExtent{ bs->worldPos[chosenAxis] + bs->worldHalfExtents[chosenAxis] };
        if (minExtent < min) {
          min = minExtent;
        }
        if (maxExtent > max) {
          max = maxExtent;
        }
      }

      auto lambd = [&min, chosenAxis](ContainerType::value_type& lhs) {
        return std::static_pointer_cast<AABB>(lhs->collider)->worldPos[chosenAxis] < min;
      };

      unsigned bestSoFar;
      unsigned differenceFromIdeal{ numObjects };
      for (float const incr{ (max - min) / static_cast<float>(EVEN_SPLITS) };
        min < max; min += incr)
      {
        auto partitionResult{ std::partition(cpy.begin(), cpy.end(), lambd) };

        if (partitionResult == idealPartition) {
          auto offset{ std::partition(object, object + numObjects, lambd) };
          return offset;
        }

        // we don't want partitions with 0 objects
        if (partitionResult == cpy.begin() || partitionResult == cpy.end()) { continue; }

        unsigned const diff{ static_cast<unsigned>(glm::abs(partitionResult - idealPartition)) };
        if (diff < differenceFromIdeal) {
          bestSoFar = static_cast<unsigned>(partitionResult - cpy.begin());
          differenceFromIdeal = diff;
        }
      }

      // objects lie within the same split section
      // default to split in the middle
      if (differenceFromIdeal == numObjects) { return object + numObjects / 2; }

      // ideal was not found, return the best option found
      for (auto dest{ object }, src{ cpy.begin() }; src != cpy.end(); ++dest, ++src)
      {
        *dest = *src;
      }
      return object + bestSoFar;
    }

    // else bounding sphere
    ContainerType cpy{ object, object + numObjects };
    auto const idealPartition{ cpy.begin() + numObjects / 2 };
    for (auto start{ cpy.begin()}; start != cpy.end(); ++start)
    {
      auto bs{ std::static_pointer_cast<BoundingSphere>((*start)->collider) };

      float const minExtent{ bs->worldCenter[chosenAxis] - bs->worldRadius };
      float const maxExtent{ bs->worldCenter[chosenAxis] + bs->worldRadius };
      if (minExtent < min) {
        min = minExtent;
      }
      if (maxExtent > max) {
        max = maxExtent;
      }
    }

    auto lambd = [&min, chosenAxis](ContainerType::value_type& lhs) {
      return std::static_pointer_cast<BoundingSphere>(lhs->collider)->worldCenter[chosenAxis] < min;
    };

    unsigned bestSoFar;
    unsigned differenceFromIdeal{ numObjects };
    for (float const incr{ (max - min) / static_cast<float>(EVEN_SPLITS) };
      min < max; min += incr)
    {
      auto partitionResult{ std::partition(cpy.begin(), cpy.end(), lambd) };

      if (partitionResult == idealPartition) {
        auto offset{ std::partition(object, object + numObjects, lambd) };
        return offset;
      }

      // we don't want partitions with 0 objects
      if (partitionResult == cpy.begin() || partitionResult == cpy.end()) { continue; }

      unsigned const diff{ static_cast<unsigned>(glm::abs(partitionResult - idealPartition)) };
      if (diff < differenceFromIdeal) {
        bestSoFar = static_cast<unsigned>(partitionResult - cpy.begin());
        differenceFromIdeal = diff;
      }
    }

    // objects lie within the same split section
    // default to split in the middle
    if (differenceFromIdeal == numObjects) { return object + numObjects / 2; }

    // ideal was not found, return the best option found
    for (auto dest{ object }, src{ cpy.begin() }; src != cpy.end(); ++dest, ++src)
    {
      *dest = *src;
    }
    return object + bestSoFar;
  }

  void TopDownConstruct::DetermineAxis(ContainerType::iterator object, unsigned numObjects)
  {
    glm::vec3 min{ FLT_MAX }, max{ -FLT_MAX };
    if (Settings::bvType == BVType::AABB)
    {
      for (auto start{ object }, end{ object + numObjects }; start != end; ++start)
      {
        auto const& bv{ *std::static_pointer_cast<AABB>((*start)->collider) };
        glm::vec3 const currMin{ bv.GetMin() }, currMax{ bv.GetMax() };

        if (currMin.x < min.x) { min.x = currMin.x; }
        if (currMax.x > max.x) { max.x = currMax.x; }
        if (currMin.y < min.y) { min.y = currMin.y; }
        if (currMax.y > max.y) { max.y = currMax.y; }
        if (currMin.z < min.z) { min.z = currMin.z; }
        if (currMax.z > max.z) { max.z = currMax.z; }
      }

      glm::vec3 const spread{ max - min };
      if (spread.x < spread.y && spread.y < spread.z) {
        m_chosenAxis = ChosenAxis::Z_AXIS;
      }
      else {
        m_chosenAxis = spread.x < spread.z ? ChosenAxis::X_AXIS : ChosenAxis::Y_AXIS;
      }

      return;
    }

    // if bounding sphere
    for (auto start{ object }, end{ object + numObjects }; start != end; ++start)
    {
      auto const& bv{ *std::static_pointer_cast<BoundingSphere>((*start)->collider) };
      glm::vec3 const currMin{ bv.center - bv.radius }, currMax{ bv.center + bv.radius };

      if (currMin.x < min.x) { min.x = currMin.x; }
      if (currMax.x > max.x) { max.x = currMax.x; }
      if (currMin.y < min.y) { min.y = currMin.y; }
      if (currMax.y > max.y) { max.y = currMax.y; }
      if (currMin.z < min.z) { min.z = currMin.z; }
      if (currMax.z > max.z) { max.z = currMax.z; }
    }

    glm::vec3 const spread{ max - min };
    if (spread.x > spread.y && spread.x > spread.z) {
      m_chosenAxis = ChosenAxis::X_AXIS;
    }
    else {
      m_chosenAxis = spread.y > spread.z ? ChosenAxis::Y_AXIS : ChosenAxis::Z_AXIS;
    }
  }

} // namespace BV
