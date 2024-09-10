#include <pch.h>
#include <BoundingVolumes/BoundingSphere.h>

namespace BVHelpers
{

  void MostSeparatedPointsOnAABB(int& min, int& max, std::vector<glm::vec3> const& points)
  {
    // First find most extreme points along principal axes
    int minx = 0, maxx = 0, miny = 0, maxy = 0, minz = 0, maxz = 0;
    for (unsigned i{}; i < points.size(); ++i) {
      if (points[i].x < points[minx].x) minx = i;
      if (points[i].x > points[maxx].x) maxx = i;
      if (points[i].y < points[miny].y) miny = i;
      if (points[i].y > points[maxy].y) maxy = i;
      if (points[i].z < points[minz].z) minz = i;
      if (points[i].z > points[maxz].z) maxz = i;
    }
    // Compute the squared distances for the three pairs of points
    float const dist2x = glm::dot(points[maxx] - points[minx], points[maxx] - points[minx]);
    float const dist2y = glm::dot(points[maxy] - points[miny], points[maxy] - points[miny]);
    float const dist2z = glm::dot(points[maxz] - points[minz], points[maxz] - points[minz]);
    // Pick the pair (min, max) of points most distant
    min = minx;
    max = maxx;
    if (dist2y > dist2x && dist2y > dist2z) {
      max = maxy;
      min = miny;
    }
    if (dist2z > dist2x && dist2z > dist2y) {
      max = maxz;
      min = minz;
    }
  }
  void SphereFromDistantPoints(BV::BoundingSphere& s, std::vector<glm::vec3> const& points)
  {
    // Find the most separated point pair defining the encompassing AABB
    int min, max;
    MostSeparatedPointsOnAABB(min, max, points);

    // Set up sphere to just encompass these two points
    s.center = (points[min] + points[max]) * 0.5f;
    s.radius = glm::dot(points[max] - s.center, points[max] - s.center);
    s.radius = sqrt(s.radius);
  }

  void SphereOfSphereAndPt(BV::BoundingSphere& s, glm::vec3 const& p)
  {
    // Compute squared distance between point and sphere center
    glm::vec3 const d{ p - s.center };
    float dist2 = glm::dot(d, d);
    // Only update s if point p is outside it
    if (dist2 > s.radius * s.radius) {
      float const dist = sqrt(dist2);
      float const newRadius = (s.radius + dist) * 0.5f;
      float const k = (newRadius - s.radius) / dist;
      s.center += d * k;
      s.radius = newRadius;
    }
  }

  // Compute variance of a set of 1D values
  float Variance(float x[], int n)
  {
    float u = 0.0f;
    for (int i{}; i < n;) {
      u += x[i];
    }

    u /= n;
    float s2 = 0.0f;
    for (int i{}; i < n; i++) {
      s2 += (x[i] - u) * (x[i] - u);
    }
    return s2 / n;
  }

  glm::mat3 CovarianceMtx(std::vector<glm::vec3> const& points)
  {
    float oon{ 1.f / static_cast<float>(points.size()) };
    glm::vec3 c{ 0.f, 0.f, 0.f };
    float e00{}, e11{}, e22{}, e01{}, e02{}, e12{};

    // compute centroid of pts
    for (auto const& pt : points) {
      c += pt;
    }
    c *= oon;

    // compute covariance elements
    for (auto const& pt : points)
    {
      // translate points so center of mass is at origin
      glm::vec3 p{ pt - c };

      // compute covariance of translated points
      e00 += p.x * p.x;
      e11 += p.y * p.y;
      e22 += p.z * p.z;
      e01 += p.x * p.y;
      e02 += p.x * p.z;
      e12 += p.y * p.z;
    }

    // fill in covariance mtx
    glm::mat3 ret;
    ret[0][0] = e00 * oon;
    ret[1][1] = e11 * oon;
    ret[2][2] = e22 * oon;
    ret[1][0] = ret[0][1] = e01 * oon;
    ret[2][0] = ret[0][2] = e02 * oon;
    ret[2][1] = ret[1][2] = e12 * oon;

    return ret;
  }

  void SymSchur2(glm::mat3& a, int p, int q, float& c, float& s)
  {
    if (abs(a[q][p]) > 0.0001f)
    {
      float r = (a[p][p] - a[q][q]) / (2.0f * a[q][p]);
      float t;
      if (r >= 0.0f) {
        t = 1.0f / (r + sqrt(1.0f + r * r));
      }
      else {
        t = -1.0f / (-r + sqrt(1.0f + r * r));
      }
      c = 1.0f / sqrt(1.0f + t * t);
      s = t * c;
    }
    else
    {
      c = 1.0f;
      s = 0.0f;
    }
  }

  void Jacobi(glm::mat3& a, glm::mat3& v)
  {
    int i, j, n, p, q;
    float prevoff{}, c, s;
    glm::mat3 J;

    // Initialize v to identify matrix
    v = glm::mat3(1.f);

    // Repeat for some maximum number of iterations
    const int MAXITERATIONS = 50;
    for (n = 0; n < MAXITERATIONS; ++n)
    {
      // Find largest off-diagonal absolute element a[q][p]
      p = 0; q = 1;
      for (i = 0; i < 3; ++i)
      {
        for (j = 0; j < 3; j++)
        {
          if (i == j) continue;
          if (abs(a[j][i]) > abs(a[q][p])) {
            p = i;
            q = j;
          }
        }
      }
    }

    // Compute the Jacobi rotation matrix J(p, q, theta)
    // (This code can be optimized for the three different cases of rotation)
    SymSchur2(a, p, q, c, s);
    for (i = 0; i < 3; i++)
    {
      J[0][i] = J[1][i] = J[2][i] = 0.0f;
      J[i][i] = 1.0f;
    }
    J[q][q] = c; J[q][p] = s;
    J[p][q] = -s; J[p][p] = c;

    // Cumulate rotations into what will contain the eigenvectors
    v = v * J;

    // Make 'a' more diagonal, until just eigenvalues remain on diagonal
    auto const jTranspose = glm::transpose(J);
    a = (jTranspose * a) * J;

    // Compute 'norm' of off-diagonal elements
    float off = 0.0f;
    for (i = 0; i < 3; ++i)
    {
      for (j = 0; j < 3; ++j)
      {
        if (i == j) { continue; }
        off += a[j][i] * a[j][i];
      }
    }
    //off = sqrt(off); //not needed for norm comparison

    // Stop when norm no longer decreasing
    if (n > 2 && off >= prevoff) { return; }

    prevoff = off;
  }

  void ExtremePointsAlongDirection(glm::vec3 dir, std::vector<glm::vec3> const& points, int* imin, int* imax)
  {
    float minproj = FLT_MAX, maxproj = -FLT_MAX;
    for (unsigned i{}; i < points.size(); ++i)
    {
      // Project vector from origin to point onto direction vector
      float proj = glm::dot(points[i], dir);
      // Keep track of least distant point along direction vector
      if (proj < minproj) {
        minproj = proj;
        *imin = i;
      }
      // Keep track of most distant point along direction vector
      if (proj > maxproj) {
        maxproj = proj;
        *imax = i;
      }
    }
  }

} // namespace BVHelpers
