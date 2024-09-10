#pragma once
#include <memory>
#include <vector>
#include <BVH/TreeNode.h>

struct Object;

namespace BV
{
  
  class BVHierarchy
  {
  public:
    using Node = TreeNode::Node;
    using ContainerType = std::vector<std::shared_ptr<Object>>;

    enum class Type
    {
      TOP_DOWN,
      BOTTOM_UP,
      NUM_TYPES
    };

    BVHierarchy(Type type) : m_type{ type } {}

    virtual void Construct(std::shared_ptr<TreeNode>& node, ContainerType const& objects) = 0;
    inline Type GetType() const noexcept { return m_type; }

  private:
    Type m_type;
  };

} // namespace BV
