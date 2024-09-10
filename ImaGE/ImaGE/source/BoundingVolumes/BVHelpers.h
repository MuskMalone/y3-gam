#pragma once
#include <vector>
#include <glm/glm.hpp>

namespace BV { struct BoundingSphere; }

namespace BVHelpers
{

  void MostSeparatedPointsOnAABB(int& min, int& max, std::vector<glm::vec3> const& points);
  void SphereFromDistantPoints(BV::BoundingSphere& s, std::vector<glm::vec3> const& points);
  void SphereOfSphereAndPt(BV::BoundingSphere& s, glm::vec3 const& p);
  float Variance(float x[], int n);
  glm::mat3 CovarianceMtx(std::vector<glm::vec3> const& points);
  void SymSchur2(glm::mat3& a, int p, int q, float& c, float& s);
  void Jacobi(glm::mat3& a, glm::mat3& v);
  void ExtremePointsAlongDirection(glm::vec3 dir, std::vector<glm::vec3> const& points, int* imin, int* imax);

} // namespace BVHelpers
