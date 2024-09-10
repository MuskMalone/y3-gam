#include <pch.h>
#include "TreeNode.h"
#include <BoundingVolumes/AABB.h>
#include <BoundingVolumes/BoundingSphere.h>
#include <glm/gtx/norm.hpp>
#include <Core/Random.h>

#define BS_DEBUG

namespace BV
{
  void TreeNode::ComputeTopDownBV(ContainerType::const_iterator begin, ContainerType::const_iterator end)
  {
    if (Settings::bvType == BVType::AABB) {
      TopDownAABB(begin, end);
    }
    else {
      TopDownSphere(begin, end);
    }
  }

  void TreeNode::ComputeBottomUpBV()
  {
    if (Settings::bvType == BVType::AABB) {
      BottomUpAABB();
    }
    else {
      BottomUpSphere();
    }
  }

  void TreeNode::SetColor()
  {
    if (depth < 7) {
      bv->drawClr = rainbowColors[depth];
    }
    else {
      bv->drawClr = otherClr;
    }
  }

  void TreeNode::Draw(Graphics::ShaderProgram& shader) const
  {
    if (depth < 7) {
      if (Settings::bvhLevelVisibility[depth]) {
        bv->Draw(shader);
      }
    }
    else if (Settings::bvhOtherLevelsVisible) {
      bv->Draw(shader);
    }

    if (leftChild) {
      leftChild->Draw(shader);
    }
    if (rightChild) {
      rightChild->Draw(shader);
    }
  }

  void TreeNode::SetDepthAndColor(unsigned startDepth)
  {
    depth = startDepth;
    SetColor();
    ++startDepth;
    if (leftChild) {
      leftChild->SetDepthAndColor(startDepth);
    }
    if (rightChild) {
      rightChild->SetDepthAndColor(startDepth);
    }
  }

  void TreeNode::TopDownAABB(ContainerType::const_iterator begin, ContainerType::const_iterator end)
  {
    // if only 1 object, copy construct bv
    if (begin + 1 == end) {
      bv = std::make_shared<AABB>(*std::static_pointer_cast<AABB>((*begin)->collider));
      return;
    }

    glm::vec3 min{ FLT_MAX }, max{ -FLT_MAX };
    while (begin != end)
    {
      auto currBV{ std::static_pointer_cast<AABB>((*begin)->collider) };
      glm::vec3 const bvMin{ currBV->GetWorldMin() }, bvMax{ currBV->GetWorldMax() };

      min = glm::min(min, bvMin);
      max = glm::max(max, bvMax);
      ++begin;
    }

    auto aabb{ std::make_shared<AABB>(glm::vec3(), glm::vec3()) };
    aabb->worldPos = (max + min) * 0.5f;
    aabb->worldHalfExtents = max - aabb->worldPos;
    aabb->UpdateTransformWithWorld();
    bv = aabb;
  }

  void TreeNode::TopDownSphere(ContainerType::const_iterator begin, ContainerType::const_iterator end)
  {
    // if only 1 object, copy construct bv
    if (begin + 1 == end) {
      bv = std::make_shared<BoundingSphere>(*std::static_pointer_cast<BoundingSphere>((*begin)->collider));
      return;
    }

    // begin with the first bounding sphere
    glm::vec3 currCenter;
    float currRad;

    {
      auto firstBV{ std::static_pointer_cast<BoundingSphere>((*begin)->collider) };
      currCenter = firstBV->worldCenter;
      currRad = firstBV->worldRadius;
      ++begin;
    }

    // expand sphere to include other points if necessary
    for (; begin != end; ++begin)
    {
      auto other{ std::static_pointer_cast<BoundingSphere>((*begin)->collider) };

      float const radiusDiff{ currRad - other->worldRadius };
      float const rDiffSquared{ radiusDiff * radiusDiff };
       
      // get normalized dir to other sphere
      glm::vec3 distToOther{ other->worldCenter - currCenter };
      float const distMag{ static_cast<float>(glm::length(distToOther)) };  // we need this later

      // if spheres already within each other, continue
      if (distMag * distMag <= rDiffSquared) {
        // set bv to the bigger sphere
        if (currRad < other->worldRadius) {
          currCenter = other->worldCenter;
          currRad = other->worldRadius;
        }
        continue;
      }

      // normalize distToOther
      if (distMag == 0.f) {
        distToOther = glm::vec3(0.f);
      }
      else {
        distToOther /= distMag;
      }

      // point outside found, expand the sphere
      glm::vec3 const extremeP1{ currCenter - distToOther * currRad },
        extremeP2{ other->worldCenter + distToOther * other->worldRadius };

      // update center and radius
      currCenter = (extremeP1 + extremeP2) * 0.5f;
      currRad = (currRad + other->worldRadius + distMag) * 0.5f;  // new diameter is distance between points + both radii
    }

    auto bs{ std::make_shared<BoundingSphere>(glm::vec3(), 0.f) };
    bs->worldCenter = currCenter;
    bs->worldRadius = currRad;
    bs->UpdateTransformWithWorld();
    bv = bs;
  }

  void TreeNode::BottomUpAABB()
  {
    auto aabb{ std::make_shared<AABB>(glm::vec3(), glm::vec3()) };
    auto bv1{ std::static_pointer_cast<AABB>(leftChild->bv) }, bv2{ std::static_pointer_cast<AABB>(rightChild->bv) };

    glm::vec3 const min{ glm::min(bv1->GetWorldMin(), bv2->GetWorldMin()) }, max{ glm::max(bv1->GetWorldMax(), bv2->GetWorldMax()) };

    aabb->worldPos = (min + max) * 0.5f;
    aabb->worldHalfExtents = max - aabb->worldPos;
    aabb->UpdateTransformWithWorld();
    bv = aabb;
  }

  void TreeNode::BottomUpSphere()
  {
    // start with the leftChild's bv
    auto bs{ std::make_shared<BoundingSphere>(*std::static_pointer_cast<BoundingSphere>(leftChild->bv)) };
    auto other{ std::static_pointer_cast<BoundingSphere>(rightChild->bv) };

    float const radiusDiff{ bs->worldRadius - other->worldRadius };
    float const rDiffSquared{ radiusDiff * radiusDiff };

    // get normalized dir to other sphere
    glm::vec3 distToOther{ other->worldCenter - bs->worldCenter };
    float const distMag{ static_cast<float>(glm::length(distToOther)) };  // we need this later
    if (distMag == 0.f) {
      distToOther = glm::vec3(0.f);
    }
    else {
      distToOther /= distMag;
    }

    // if spheres already within each other, return
    if (distMag * distMag <= rDiffSquared) {
      if (bs->worldRadius < other->worldRadius)
      {
        bs->worldRadius = other->worldRadius;
        bs->worldCenter = other->worldCenter;
      }
      bs->UpdateTransformWithWorld();
      bv = bs;
      return;
    }

    // point outside found, expand the sphere
    glm::vec3 const extremeP1{ bs->worldCenter - distToOther * bs->worldRadius },
      extremeP2{ other->worldCenter + distToOther * other->worldRadius };

    // update center and radius
    bs->worldCenter = (extremeP1 + extremeP2) * 0.5f;
    bs->worldRadius = (bs->worldRadius + distMag + other->worldRadius) * 0.5f;  // new diameter is distance between points + both radii
    bs->UpdateTransformWithWorld();
    bv = bs;
  }

} // namespace BV
