#pragma once
#include <glm/glm.hpp>
#include <SpatialPartitioning/ProxyObj.h>
#include <BoundingVolumes/AABB.h>
#include <memory>

// forward declaration
namespace Primitives { struct Plane; }

// Spatial Partitioning
namespace SP
{

  struct OctNode
  {
    using NodePtr = std::shared_ptr<OctNode>;

  public:
    glm::mat4 transformMtx;
    NodePtr children[8];
    BV::AABB bv;
    std::vector<glm::vec3> vertices;
    ProxyObj::Pointer objects;
    glm::vec4 drawClr; 
    glm::vec3 center;
    float halfWidth;

    static constexpr glm::vec4 drawColors[]
    {
      glm::vec4(1.f, 0.f, 0.f, 1.f),    // Red
      glm::vec4(1.f, 0.5f, 0.f, 1.f),   // Orange
      glm::vec4(1.f, 1.f, 0.f, 1.f),    // Yellow
      glm::vec4(0.f, 1.f, 0.f, 1.f),    // Green
      glm::vec4(0.f, 0.f, 1.f, 1.f),    // Blue
      glm::vec4(128.f / 255.f, 0.f, 222.f / 255.f, 1.f),  // Violet
      glm::vec4(1.f, 192.f / 255.f, 203.f / 255.f, 1.f),  // Pink
    };
    static unsigned clrIdx;

    OctNode() : transformMtx{}, children{}, bv{}, vertices{}, objects{}, drawClr{ drawColors[(clrIdx++) % 7] }, center{}, halfWidth{}, vao{} {}
    OctNode(glm::vec3 const& _center, float _halfWidth) : transformMtx{}, children{}, bv{ _center, glm::vec3(_halfWidth) }, 
      vertices{}, objects{ nullptr }, drawClr{ drawColors[(clrIdx++) % 7] }, center{ _center }, halfWidth{ _halfWidth }, vao{} {}

    void Initialize();
    void CreateChildren();
    void ComputeBV();
    void Draw(Graphics::ShaderProgram& shader);
    inline size_t GetTriangleCount() const { return vertices.size() / 3; }
    void SetNodePlanes(Primitives::Plane (&planes)[6]);
    void AddObject(ProxyObj::Pointer const& obj);
    inline bool Empty() const noexcept { return !objects; }

  private:
    GLuint vao;
  };


} // namespace SP
