#pragma once
#include <GUI/GUIWindow.h>
#include <vector>
#include <memory>

namespace GUI
{

  class Toolbar : public GUIWindow
  {
  public:
    Toolbar(std::string const& name, std::vector<std::unique_ptr<GUIWindow>> const& windowsRef);

    void Run();

  private:
    std::vector<std::unique_ptr<GUIWindow>> const& m_windowsRef;  // to manage active states
  };

} // namespace GUI
