#include <pch.h>
#include "VolumeBounder.h"
#include <BVH/Construct/TopDownConstruct.h>
#include <BVH/Construct/BottomUpConstruct.h>

namespace BV
{

  VolumeBounder::VolumeBounder() : m_tree{}, m_method{} {}

  void VolumeBounder::Construct(BVHierarchy::ContainerType& objects, BV::BVHierarchy::Type type)
  {
    if (type == BV::BVHierarchy::Type::TOP_DOWN) {
      m_method = std::make_unique<TopDownConstruct>();
    }
    else {
      m_method = std::make_unique<BottomUpConstruct>();
    }
    m_tree.reset();
    m_method->Construct(m_tree, objects);
  }

  // should change to just update the tree instead of reconstructing
  void VolumeBounder::Reconstruct(BVHierarchy::ContainerType& objects)
  {
    m_tree.reset();
    m_method->Construct(m_tree, objects);
  }

  void VolumeBounder::Draw(Graphics::ShaderProgram& shader) const
  {
    m_tree->Draw(shader);
  }

} // namespace BV
