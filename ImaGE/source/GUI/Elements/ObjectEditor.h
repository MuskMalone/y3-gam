#pragma once
#include <GUI/GUIWindow.h>

namespace GUI
{

  class ObjectEditor : public GUIWindow
  {
  public:
    ObjectEditor(std::string const& windowName, Scene& scene);

    void Run() override;

  private:
    void EditObjects();
  };
  
} // namespace GUI
