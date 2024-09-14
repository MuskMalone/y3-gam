#pragma once

namespace GUI
{
  // class that defines common interface every GUI window should have
  class GUIWindow
  {
  public:
    inline void Toggle() noexcept { mActive = !mActive; }
    inline bool IsActive() const noexcept { return mActive; }
    inline std::string const& GetName() const noexcept { return mWindowName; }

    //virtual void Init() {}
    virtual void Run() = 0;

  protected:
    GUIWindow(std::string windowName) : mWindowName{ std::move(windowName) }, mActive{ true } {}

    std::string const mWindowName;
    bool mActive;
  };

};  // namespace GUI
