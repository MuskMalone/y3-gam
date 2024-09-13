#pragma once
#include <GUI/GUIWindow.h>

namespace GUI
{

  class Inspector : public GUIWindow
  {
  public:
    Inspector(std::string const& name);

    void Run() override;

  private:
    
  };

} // namespace GUI
