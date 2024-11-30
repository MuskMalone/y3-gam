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

    inline static bool sOpenPopup;
    inline static std::string sCurrentPopup;

    inline static constexpr char sGUIDPopupTitle[] = "Did you just merge???";
    inline static constexpr char sGUIDCompletePopupTitle[] = "GUID Repair Complete";

    std::vector<PopupFn> mPopupFunctions;

    EVENT_CALLBACK_DECL(OnGUIDInvalidated);
    EVENT_CALLBACK_DECL(OnGUIDRemap);
    
    void GUIDRemapPopup();
    void GUIDCompletePopup();

  };
} // namespace GUI
