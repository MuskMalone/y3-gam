#pragma once
#include <GUI/GUIWindow.h>
#include <Events/EventCallback.h>

namespace GUI {
  class PopupHelper : public GUIWindow
  {
  public:
    PopupHelper(const char* windowName);

    void Run() override;

  private:
    using PopupFn = std::function<void()>;

    static inline constexpr char sGUIDPopupTitle[] = "Broken GUID Reference";

    std::vector<PopupFn> mPopupFunctions;

    EVENT_CALLBACK_DECL(TriggerGUIDRemap);

    void GUIDRemapPopup();

  };
} // namespace GUI
