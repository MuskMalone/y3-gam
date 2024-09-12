/*!*********************************************************************
\file   EventManager.cpp
\author chengen.lau\@digipen.edu
\date   27-September-2023
\brief  The events system is managed by the EventManager singleton. It
        provides functions to subscribe listeners to events and is the
        source of dispatching events. The EventManager holds the map 
        of events to listeners and notifies all relevant parties 
        subscribed to a particular event when it is dispatched.
        Additionally, macros are defined for ease of use to simplify
        subscription and queuing of events.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "EventManager.h"

namespace Events
{

  void EventManager::Subscribe(EventType eventType, EventCallback const& callback)
  {
    mSubscribers[eventType].emplace_back(callback);
  }

  void EventManager::QueueEvent(EventPtr const& event)
  {
    mEventQueue.emplace(event);
  }

  void EventManager::QueueEvent(EventPtr&& event)
  {
    mEventQueue.emplace(std::move(event));
  }

  void EventManager::DispatchAll()
  {
    while (!mEventQueue.empty())
    {
      auto eventToDispatch{ std::move(mEventQueue.front()) };
      mEventQueue.pop();

      SubscriberList& subscribers{ mSubscribers[eventToDispatch->GetCategory()] };
      for (auto& fn : subscribers)
      {
        fn(eventToDispatch);
      }
    }
  }

} // namespace Events