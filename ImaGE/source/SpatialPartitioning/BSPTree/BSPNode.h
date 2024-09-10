#pragma once
#include <glm/glm.hpp>
#include <SpatialPartitioning/ProxyObj.h>
#include <BoundingVolumes/AABB.h>
#include <rapidjson/document.h>

// forward declaration
struct Object;

// Spatial Partitioning
namespace SP
{

  struct BSPNode
  {
    using NodePtr = std::shared_ptr<BSPNode>;

    std::vector<glm::vec3> vertices;
    glm::vec4 drawClr;
    NodePtr leftChild, rightChild;

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

    BSPNode(NodePtr const& left = nullptr, NodePtr const& right = nullptr) :
      vertices{}, drawClr{drawColors[(clrIdx++) % 7]}, leftChild{left}, rightChild{right}, vao{} {}

    void Initialize();
    void Draw(Graphics::ShaderProgram& shaders) const;
    void Serialize(rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator) const;
    void Deserialize(rapidjson::Value const& arr, rapidjson::SizeType idx = 0);

  private:
    GLuint vao;
  };


} // namespace SP
