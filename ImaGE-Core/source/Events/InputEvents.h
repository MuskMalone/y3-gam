/*!*********************************************************************
\file   InputEvents.h
\author chengen.lau\@digipen.edu
\date   10-September-2025
\brief  Event subclasses for input. Currently defined for key triggers,
        holds, and releases. They are dispatched from the Input Manager
        class.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include "Event.h"
#include <string>
#include "Input/KeyCode.h"

namespace Events
{
  class KeyTriggeredEvent : public Event
  {
  public:
    KeyTriggeredEvent(KEY_CODE code) : Event(), mCode{ code } {}
    inline KEY_CODE GetKey() const noexcept { return mCode; }
    inline std::string GetName() const noexcept override { return "Key Triggered"; }

  private:
    KEY_CODE mCode;
  };

  class KeyHeldEvent : public Event
  {
  public:
    KeyHeldEvent(KEY_CODE code) : Event(), mCode{ code } {}
    inline KEY_CODE GetKey() const noexcept { return mCode; }
    inline std::string GetName() const noexcept override { return "Key Held"; }

  private:
    KEY_CODE mCode;
  };

  class KeyReleasedEvent : public Event
  {
  public:
    KeyReleasedEvent(KEY_CODE code) : Event(), mCode{ code } {}
    inline KEY_CODE GetKey() const noexcept { return mCode; }
    inline std::string GetName() const noexcept override { return "Key Released"; }

  private:
    KEY_CODE mCode;
  };

  class MouseTriggeredEvent : public Event
  {
  public:
    MouseTriggeredEvent(KEY_CODE code) : Event(), mCode{ code } {}
    inline KEY_CODE GetKey() const noexcept { return mCode; }
    inline std::string GetName() const noexcept override { return "Mouse Triggered"; }

  private:
    KEY_CODE mCode;
  };

  class MouseHeldEvent : public Event
  {
  public:
    MouseHeldEvent(KEY_CODE code) : Event(), mCode{ code } {}
    inline KEY_CODE GetKey() const noexcept { return mCode; }
    inline std::string GetName() const noexcept override { return "Mouse Held"; }

  private:
    KEY_CODE mCode;
  };

  class MouseReleasedEvent : public Event
  {
  public:
    MouseReleasedEvent(KEY_CODE code) : Event(), mCode{ code } {}
    inline KEY_CODE GetKey() const noexcept { return mCode; }
    inline std::string GetName() const noexcept override { return "Mouse Released"; }

  private:
    KEY_CODE mCode;
  };
}
