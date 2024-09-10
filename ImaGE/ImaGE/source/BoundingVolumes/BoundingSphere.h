#pragma once
#include <BoundingVolumes/Collider.h>
#include <BoundingVolumes/BVTypes.h>

namespace BV
{

  // (center - point)^2 - r^2 = 0
  struct BoundingSphere : Collider
  {
    BoundingSphere(glm::vec3 const& pos, float rad);
    BoundingSphere(glm::vec3 const& pos, glm::vec3 const& scale);

    void Update(Transform const& transform) override;
    void ComputeBV(Graphics::VertexAttribs const& vtxAttribs) override;
    void Reset() override {}

    float GetWorldVolume() const noexcept;
    void UpdateTransformWithWorld();

    void GenerateInitialPoints(std::vector<glm::vec3> const& vertices);
    

    enum class EPOS
    {
      EPOS_6 = 0,
      EPOS_14,
      EPOS_26,
      EPOS_98,
      NUM_EPOS
    };

    glm::vec3 center, worldCenter;
    float radius, worldRadius;;
    bool reRandomizePoints;

    static EPOS eposMode;
    static bool growInitial;

  private:
    static constexpr unsigned LARSSONS_INITIAL_POINT_COUNT = 1567;
    std::vector<unsigned> shuffledPoints;


    void RittersSphere(std::vector<glm::vec3> const& vertices);
    void LarssonsMethod(std::vector<glm::vec3> const& vertices);
    void PCA(std::vector<glm::vec3> const& vertices);
  };

} // namespace BV