#include <pch.h>
#include "BoundingSphere.h"
#include <glm/gtx/norm.hpp>
#include <Core/Random.h>
#include "BVHelpers.h"

//#define BS_DEBUG
//#define ORIGINAL_RITTERS

namespace
{
  constexpr glm::vec3 EPOS_NORMALS[]
  {
      { 1.f,  0.f,  0.f }, { 0.f,  1.f,  0.f }, { 0.f,  0.f,  1.f },

      { 1.f,  1.f,  1.f }, { 1.f,  1.f, -1.f }, { 1.f, -1.f,  1.f },
      { 1.f, -1.f, -1.f },

      { 1.f,  1.f,  0.f }, { 1.f, -1.f,  0.f }, { 1.f,  0.f,  1.f },
      { 1.f,  0.f, -1.f }, { 0.f,  1.f,  1.f }, { 0.f,  1.f, -1.f },

      { 0.f,  1.f,  2.f }, { 0.f,  2.f,  1.f }, { 1.f,  0.f,  2.f },
      { 2.f,  0.f,  1.f }, { 1.f,  2.f,  0.f }, { 2.f,  1.f,  0.f },
      { 0.f,  1.f, -2.f }, { 0.f,  2.f, -1.f }, { 1.f,  0.f, -2.f },
      { 2.f,  0.f, -1.f }, { 1.f, -2.f,  0.f }, { 2.f, -1.f,  0.f },

      { 1.f,  1.f,  2.f }, { 2.f,  1.f,  1.f }, { 1.f,  2.f,  1.f },
      { 1.f, -1.f,  2.f }, { 1.f,  1.f, -2.f }, { 1.f, -1.f, -2.f },
      { 2.f, -1.f,  1.f }, { 2.f,  1.f, -1.f }, { 2.f, -1.f, -1.f },
      { 1.f, -2.f,  1.f }, { 1.f,  2.f, -1.f }, { 1.f, -2.f, -1.f },

      { 2.f,  2.f,  1.f }, { 1.f,  2.f,  2.f }, { 2.f,  1.f,  2.f },
      { 2.f, -2.f,  1.f }, { 2.f,  2.f, -1.f }, { 2.f, -2.f, -1.f },
      { 1.f, -2.f,  2.f }, { 1.f,  2.f, -2.f }, { 1.f, -2.f, -2.f },
      { 2.f, -1.f,  2.f }, { 2.f,  1.f, -2.f }, { 2.f, -1.f, -2.f }
  };
}

namespace BV
{
  BoundingSphere::EPOS BoundingSphere::eposMode{ EPOS::EPOS_6 };
  bool BoundingSphere::growInitial{ true };

  BoundingSphere::BoundingSphere(glm::vec3 const& pos, float rad)
    : Collider(), center{ pos }, worldCenter{}, radius{ rad }, worldRadius{},
    reRandomizePoints{ true }, shuffledPoints{}
  {
    SetModel(Graphics::MeshType::SPHERE);
  }

  BoundingSphere::BoundingSphere(glm::vec3 const& pos, glm::vec3 const& scale)
    : Collider(), center{ pos }, worldCenter{}, radius{ scale.x }, worldRadius{},
    reRandomizePoints{ true },shuffledPoints {}
  {
    SetModel(Graphics::MeshType::SPHERE);
  }

  void BoundingSphere::Update(Transform const& transform)
  {
    glm::vec3 const combinedScale{ transform.GetBaseScale() * transform.scale };
    
    // obj.transform * this->transform
    worldCenter = combinedScale * center + transform.pos;
    worldRadius = glm::max(combinedScale.x, glm::max(combinedScale.y, combinedScale.z)) * radius;

    transformMtx = glm::mat4(
      worldRadius, 0.f, 0.f, 0.f,
      0.f, worldRadius, 0.f, 0.f,
      0.f, 0.f, worldRadius, 0.f,
      worldCenter.x, worldCenter.y, worldCenter.z, 1.f
    );
  }

  float BoundingSphere::GetWorldVolume() const noexcept
  {
    return worldRadius * worldRadius * glm::pi<float>();
  }

  void BoundingSphere::UpdateTransformWithWorld()
  {
    transformMtx = glm::mat4(
      worldRadius, 0.f, 0.f, 0.f,
      0.f, worldRadius, 0.f, 0.f,
      0.f, 0.f, worldRadius, 0.f,
      worldCenter.x, worldCenter.y, worldCenter.z, 1.f
    );
  }

  void BoundingSphere::ComputeBV(Graphics::VertexAttribs const& vtxAttribs)
  {
    if (reRandomizePoints) { GenerateInitialPoints(vtxAttribs.points); }

    switch (Settings::bvType)
    {
    case BVType::RITTER:
      RittersSphere(vtxAttribs.points);
      break;
    case BVType::LARSSON:
      LarssonsMethod(vtxAttribs.points);
      break;
    case BVType::PCA:
      PCA(vtxAttribs.points);
      break;
    }
  }

  void BoundingSphere::RittersSphere(std::vector<glm::vec3> const& vertices)
  {
#ifndef ORIGINAL_RITTERS
    std::vector<glm::vec3> initial;
    initial.reserve(shuffledPoints.size());
    for (auto const& idx : shuffledPoints)
      initial.emplace_back(vertices[idx]);
    BVHelpers::SphereFromDistantPoints(*this, initial);

    // Grow sphere to include all points
    for (unsigned i{}; i < vertices.size(); ++i)
      BVHelpers::SphereOfSphereAndPt(*this, vertices[i]);
#else
    // minX, maxX, minY, maxY, minZ, maxZ
    std::vector<glm::vec3> extremes;
    extremes.resize(6, vertices[shuffledPoints.front()]);

    // get extreme points for each axis
    for (auto const& idx : shuffledPoints)
    {
      auto const& vtx{ vertices[idx] };
      if (vtx.x < extremes[0].x) extremes[0] = vtx;
      if (vtx.x > extremes[1].x) extremes[1] = vtx;
      if (vtx.y < extremes[2].y) extremes[2] = vtx;
      if (vtx.y > extremes[3].y) extremes[3] = vtx;
      if (vtx.z < extremes[4].z) extremes[4] = vtx;
      if (vtx.z > extremes[5].z) extremes[5] = vtx;
    }

    // find pair with largest dist.
    std::vector<glm::vec3>::const_iterator p1{ extremes.cbegin() }, p2{ extremes.cbegin() + 1 };
    float maxDist{ -FLT_MAX };
    for (auto i{ extremes.cbegin() }; i != extremes.cend(); ++i)
    {
      for (auto j{ i + 1 }; j != extremes.cend(); ++j)
      {
        float const dist{ glm::distance2(*i, *j) };
        if (dist > maxDist) {
          maxDist = dist;
          p1 = i;
          p2 = j;
        }
      }
    }

    // construct initial sphere
    glm::vec3 currCenter{ (*p1 + *p2) * 0.5f };
    float squaredR{ glm::distance2(*p1, currCenter) };

    if (!growInitial)
    {
      center = std::move(currCenter);
      radius = sqrt(squaredR);
      return;
    }

    // check for any outliers
    for (auto const& vtx : vertices)
    {
      float const squaredDist{ glm::distance2(vtx, currCenter) };
      if (squaredDist <= squaredR) { continue; }

      // point found, expand the sphere
      glm::vec3 const dirToCenter{ glm::normalize(vtx - currCenter) };
      float oldRad{ sqrt(squaredR) };
      glm::vec3 oppositeEnd{ currCenter + dirToCenter * oldRad };
      // find the new center
      currCenter = (vtx + oppositeEnd) * 0.5f;
      squaredR = glm::distance2(currCenter, vtx);
    }

    // bounding sphere complete
    center = std::move(currCenter);
    radius = sqrt(squaredR);
#endif
#ifdef BS_DEBUG
    std::cout << "Position: " << center << ", radius: " << radius << "\n";
#endif
  }

  void BoundingSphere::LarssonsMethod(std::vector<glm::vec3> const& vertices)
  {
    unsigned eposNormalCount;
    switch (eposMode)
    {
    case EPOS::EPOS_6:
      eposNormalCount = 3;
      break;
    case EPOS::EPOS_14:
      eposNormalCount = 7;
      break;
    case EPOS::EPOS_26:
      eposNormalCount = 13;
      break;
    case EPOS::EPOS_98:
      eposNormalCount = 49;
      break;
    }

    std::vector<glm::vec3> minMaxes;
    minMaxes.resize(eposNormalCount * 2, {});
    std::vector<float> distances; // min, max, min, max ...
    distances.resize(eposNormalCount * 2, -FLT_MAX);

    {
      // pick 3k initial points
      size_t const initialCount{ std::min<size_t>(3000, vertices.size()) };

      for (unsigned i{}; i < eposNormalCount * 2; i += 2) { distances[i] = FLT_MAX; }

      // for each support point, find the closest and farthest points
      for (unsigned i{}; i < eposNormalCount; ++i)
      {
        unsigned const currIdx{ i * 2 };
        for (auto const& idx : shuffledPoints)
        {
          glm::vec3 const& currVtx{ vertices[idx] };
          float const dist{ glm::dot(EPOS_NORMALS[i], currVtx) };

          if (dist < distances[currIdx])
          {
            minMaxes[currIdx] = currVtx;
            distances[currIdx] = dist;
          }
          if (dist > distances[currIdx + 1])
          {
            minMaxes[currIdx + 1] = currVtx;
            distances[currIdx + 1] = dist;
          }
        }
      }
    }

    // iterate through results to find pair furthest from each other
    glm::ivec2 furthestPair{};
    float greatestDist{ -FLT_MAX };
    for (unsigned i{}; i < eposNormalCount * 2; ++i)
    {
      for (unsigned j{ i + 1 }; j < eposNormalCount * 2; ++j)
      {
        float const sqDist{ glm::distance2(minMaxes[i], minMaxes[j]) };
        if (sqDist <= greatestDist) { continue; }

        greatestDist = sqDist;
        furthestPair = { i, j };
      }
    }

    // construct initial sphere
    glm::vec3 currCenter{ (minMaxes[furthestPair.x] + minMaxes[furthestPair.y]) * 0.5f };
    float squaredR{ glm::distance2(currCenter, minMaxes[furthestPair.x]) };

    if (!growInitial)
    {
      center = std::move(currCenter);
      radius = sqrt(squaredR);
#ifdef BS_DEBUG
      std::cout << "Position: " << center << ", radius: " << radius << "\n";
#endif
      return;
    }

    // expand sphere to include other points if necessary
    for (auto const& vtx : vertices)
    {
      float const squaredDist{ glm::distance2(vtx, currCenter) };
      if (squaredDist <= squaredR) { continue; }

      // point found, expand the sphere
      glm::vec3 const dirToCenter{ glm::normalize(currCenter - vtx) };
      glm::vec3 const oppositeEnd{ currCenter + dirToCenter * sqrt(squaredR) };
      // find the new center
      currCenter = (vtx + oppositeEnd) * 0.5f;
      squaredR = glm::distance2(currCenter, vtx);
    }

    // bounding sphere complete
    center = std::move(currCenter);
    radius = sqrt(squaredR);

#ifdef BS_DEBUG
    std::cout << "Position: " << center << ", radius: " << radius << "\n";
#endif
  }  

  void BoundingSphere::PCA(std::vector<glm::vec3> const& vertices)
  {
    glm::mat3 v;

    std::vector<glm::vec3> initialPoints;
    initialPoints.reserve(shuffledPoints.size());
    for (auto const& idx : shuffledPoints) { initialPoints.emplace_back(vertices[idx]); }

    // Compute the covariance matrix m
    glm::mat3 m{ BVHelpers::CovarianceMtx(initialPoints)};
    // Decompose it into eigenvectors (in v) and eigenvalues (in m)
    BVHelpers::Jacobi(m, v);

    // Find the component with largest magnitude eigenvalue (largest spread)
    glm::vec3 e;
    int maxc{};
    float maxf, maxe{ abs(m[0][0]) };
    if ((maxf = abs(m[1][1])) > maxe) {
      maxc = 1;
      maxe = maxf;
    }
    if ((maxf = abs(m[2][2])) > maxe) {
      maxc = 2;
      maxe = maxf;
    }
    e[0] = v[maxc][0];
    e[1] = v[maxc][1];
    e[2] = v[maxc][2];

    // Find the most extreme points along direction ‘e’
    int imin, imax;
    BVHelpers::ExtremePointsAlongDirection(e, initialPoints, &imin, &imax);
    glm::vec3 minpt = initialPoints[imin];
    glm::vec3 maxpt = initialPoints[imax];

    float dist = sqrt(glm::dot(maxpt - minpt, maxpt - minpt));
    center = (minpt + maxpt) * 0.5f;
    radius = dist * 0.5f;

    for (unsigned i{}; i < vertices.size(); ++i) {
      BVHelpers::SphereOfSphereAndPt(*this, vertices[i]);
    }
  }

  void BoundingSphere::GenerateInitialPoints(std::vector<glm::vec3> const& vertices)
  {
    // set to default size and unshuffled indexes
    size_t const count{ vertices.size() < LARSSONS_INITIAL_POINT_COUNT
      ? static_cast<unsigned>(static_cast<double>(vertices.size()) * 0.75) : LARSSONS_INITIAL_POINT_COUNT };
    {
      shuffledPoints.resize(vertices.size());
      unsigned i{};
      for (auto& val : shuffledPoints) { val = i; ++i; }
    }

    // shuffle
    auto randomized{ Random::RandGen::ShuffleVector(shuffledPoints) };
    shuffledPoints = { randomized.begin(), randomized.begin() + count };

    reRandomizePoints = false;
  }

} // namespace BV
