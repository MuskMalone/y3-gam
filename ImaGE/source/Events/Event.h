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

namespace Events
{
  enum class EventType
  {
    KEY_TRIGGERED = 0,
    KEY_HELD,
    KEY_RELEASED,
    MOUSE_TRIGGERED,
    MOUSE_HELD,
    MOUSE_RELEASED,
    WINDOW_LOSE_FOCUS,
    WINDOW_GAIN_FOCUS,
    WINDOW_MINIMIZED,
    TOGGLE_FULLSCREEN,
    QUIT_GAME
#ifdef GAM200_EVENTS
#ifndef IMGUI_DISABLE
    NEW_SCENE,
    START_SCENE,
    PAUSE_SCENE,
    STOP_SCENE,
    PREFAB_SAVED,
    PREFAB_INSTANCES_UPDATED,
    EDIT_PREFAB,
    DELETE_ASSET,
    DELETE_PREFAB,
    DELETE_PREFAB_CHILD,
    RELOAD_SCRIPT,
#endif
#endif
  };

  class Event
  {
  public:
    Event(EventType type) : m_category{ type } {}

    /*!*********************************************************************
      \brief
        Returns the name of the current event
      \return
        The name of the current event
      ************************************************************************/
    virtual inline std::string GetName() const noexcept { return "Base Event"; }

    /*!*********************************************************************
    \brief
      Returns the category of the current event
    \return
      The category of the current event
    ************************************************************************/
    inline EventType GetCategory() const noexcept { return m_category; }

    /*!*********************************************************************
    \brief
      Returns the status of the current event
    \return
      True if the event has already been handled and false otherwise
    ************************************************************************/
    //inline bool IsHandled() const noexcept { return m_handled; }

    /*!*********************************************************************
    \brief
      Virtual destructor.
    ************************************************************************/
    virtual ~Event() {}

  protected:
    EventType m_category;
    //bool m_handled = false;
  };

} // namespace Events