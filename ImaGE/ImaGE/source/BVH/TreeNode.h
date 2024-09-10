#pragma once
#include <vector>
#include <Core/Object.h>
#include <memory>
#include <BoundingVolumes/Collider.h>

namespace BV
{

  struct TreeNode
  {
    using Node = std::shared_ptr<TreeNode>;
    using ContainerType = std::vector<std::shared_ptr<Object>>;

    enum NodeType
    {
      INTERNAL,
      LEAF
    };

    std::shared_ptr<Object> data;
    Node leftChild, rightChild;
    std::shared_ptr<Collider> bv;
    unsigned numObjects, depth;
    NodeType type;

    static constexpr glm::vec4 rainbowColors[] {
      glm::vec4(1.f, 0.f, 0.f, 1.f),    // Red
      glm::vec4(1.f, 0.5f, 0.f, 1.f),   // Orange
      glm::vec4(1.f, 1.f, 0.f, 1.f),    // Yellow
      glm::vec4(0.f, 1.f, 0.f, 1.f),    // Green
      glm::vec4(0.f, 0.f, 1.f, 1.f),    // Blue
      glm::vec4(128.f / 255.f, 0.f, 222.f / 255.f, 1.f),  // Violet
      glm::vec4(1.f, 192.f/255.f, 203.f/255.f, 1.f),  // Pink
    };
    static constexpr glm::vec4 otherClr = glm::vec4(68.f / 255.f, 1.f, 210.f / 255.f, 1.f);

    TreeNode(unsigned depth = 0, NodeType _type = INTERNAL) : data{ nullptr }, leftChild{}, rightChild{},
      bv{}, numObjects{}, depth{ depth }, type{ _type } {}

    TreeNode(Node const& left, Node const& right) : data{}, leftChild{ left }, rightChild{ right },
      bv{}, numObjects{ left->numObjects + rightChild->numObjects }, depth{}, type{ INTERNAL } {}

    void ComputeTopDownBV(ContainerType::const_iterator begin, ContainerType::const_iterator end);
    void ComputeBottomUpBV();
    void Draw(Graphics::ShaderProgram& shader) const;
    void SetDepthAndColor(unsigned startDepth = 0);
    void SetColor();

  private:
    void TopDownAABB(ContainerType::const_iterator begin, ContainerType::const_iterator end);
    void TopDownSphere(ContainerType::const_iterator begin, ContainerType::const_iterator end);

    void BottomUpAABB();
    void BottomUpSphere();
  };

} // namespace BV