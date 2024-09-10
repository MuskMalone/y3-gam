#pragma once
#include <GUI/GUIWindow.h>
#include <Graphics/MeshType.h>

namespace GUI
{

  class ObjectManager : public GUIWindow
  {
  public:
    ObjectManager(std::string const& windowName, Scene& scene);

    void Run() override;

  private:
    int m_selectedType;

    void ListObjects(float height);
    void UpdateControls(float height);
  };
  
} // namespace GUI
