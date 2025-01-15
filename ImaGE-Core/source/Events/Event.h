/*!*********************************************************************
\file   Event.h
\author chengen.lau\@digipen.edu
\date   10-September-2024
\brief  Base event class for events/messaging system. All events inherit
        from base class Event and will by default contain a name and a
        category. Each individual event will hold its own set of
        variables that will be passed to the listener when handling
        the event. (E.g. input events hold the key code for the key
        that was pressed)

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <typeindex>
#include <string>

namespace Events
{
  class Event
  {
  public:
    Event() = default;

    /*!*********************************************************************
      \brief
        Returns the name of the current event
      \return
        The name of the current event
      ************************************************************************/
    virtual inline std::string GetName() const noexcept { return "Base Event"; }

    /*!*********************************************************************
    \brief
      Returns the status of the current event
    \return
      True if the event has already been handled and false otherwise
    ************************************************************************/
    //inline bool IsHandled() const noexcept { return mHandled; }

    /*!*********************************************************************
    \brief
      Virtual destructor.
    ************************************************************************/
    virtual ~Event() {}

  protected:
    //bool mHandled = false;
  };

} // namespace Events