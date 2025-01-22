#pragma once
#include <GUI/GUIWindow.h>

namespace GUI {
  class KeyframeEditor : public GUIWindow
  {
  public:
    KeyframeEditor(const char* windowName);

    void Run() override;

  private:

  };
}