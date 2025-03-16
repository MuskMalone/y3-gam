/*!*********************************************************************
\file   PopupHelper.h
\author chengen.lau\@digipen.edu
\date   30-November-2024
\brief  Class encapsulating functions to faciliate general 
        engine-related popups such as remapping invalidated guids.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
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

    inline static constexpr char sGUIDPopupTitle[] = "Did you just merge??? Unlucky.";
    inline static constexpr char sGUIDCompletePopupTitle[] = "GUID Repair Complete";
    inline static constexpr char sWindowClosePopupTitle[] = "WARNING: Scene has been Modified!";

    std::vector<PopupFn> mPopupFunctions;

    EVENT_CALLBACK_DECL(OnGUIDInvalidated);
    EVENT_CALLBACK_DECL(OnGUIDRemap);
    EVENT_CALLBACK_DECL(OnQuitAppPopupTrigger);
    
    void InitGUIDPopup();
    void GUIDRemapPopup();
    void GUIDCompletePopup();

    void WindowCloseConfirmationPopup();
  };
} // namespace GUI
