#pragma once
#include <GUI/GUIWindow.h>
#include <BVH/VolumeBounder.h>

namespace GUI
{

  class TreeConfig : public GUIWindow
  {
  public:
    TreeConfig(std::string const& windowName, Scene& scene, BV::VolumeBounder& bvh);

    void Run() override;

  private:
    BV::VolumeBounder& m_bvh;
  };

} // namespace GUI
