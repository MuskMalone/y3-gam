#pragma once
#include <BVH/TreeNode.h>
#include <BVH/Construct/BVHierarchy.h>

struct Object;

namespace BV
{
  class VolumeBounder
  {
  public:
    VolumeBounder();

    void Construct(BVHierarchy::ContainerType& objects, BV::BVHierarchy::Type type);
    void Reconstruct(BVHierarchy::ContainerType& obejcts);
    void Draw(Graphics::ShaderProgram& shader) const;
    inline BVHierarchy::Type GetCurrentType() const noexcept { return m_method->GetType(); }

  private:
    TreeNode::Node m_tree;
    std::unique_ptr<BVHierarchy> m_method;
  };

} // namespace BV
