#pragma once
#include <GUI/GUIWindow.h>
#include <Events/EventCallback.h>
#include <unordered_map>
#include <Asset/RemapData.h>

namespace GUI {
  class PopupHelper : public GUIWindow
  {
  public:
    PopupHelper(const char* windowName);

    void Run() override;

  private:
    using PopupFn = std::function<void()>;
    using GUIDDataMap = std::unordered_map<IGE::Assets::GUID, IGE::Assets::RemapData>;

    static inline constexpr char sGUIDPopupTitle[] = "Broken GUID Reference";

    GUIDDataMap mGUIDData;
    std::vector<PopupFn> mPopupFunctions;

    EVENT_CALLBACK_DECL(OnGUIDRemap);

    void GUIDRemapPopup();

  };
} // namespace GUI
