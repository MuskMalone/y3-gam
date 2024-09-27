/*!*********************************************************************
\file   EventManager.h
\author chengen.lau\@digipen.edu
\date   10-September-2024
\brief  The events system is managed by the EventManager singleton. It
        provides functions to subscribe listeners to events and is the
        source of dispatching events. The EventManager holds the map 
        of events to listeners and notifies all relevant parties 
        subscribed to a particular event when it is dispatched.
        Additionally, macros are defined for ease of use to simplify
        subscription and queuing of events.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <Singleton.h>
#include "Events.h"
#include <functional>
#include <vector>
//#include <typeindex>  // for std::type_index
#include <queue>
#include <memory>

namespace Events
{

  class EventManager : public Singleton<EventManager>
  {
  public:
    using EventPtr = std::shared_ptr<Event>;
    using EventCallback = std::function<void(EventPtr const&)>;

    /*!*********************************************************************
    \brief
      Subscribes a callback function to an event category
    \param eventType
      The type of the event
    \param callback
      The callback function to subscribe to the event
    ************************************************************************/
    void Subscribe(EventType eventType, EventCallback const& callback);

    /*!*********************************************************************
    \brief
      Adds an event to the queue. All respective listeners will be notified
      when the event is dispatched at the start of the game loop.
    \param event
      The event to queue
    ************************************************************************/
    void QueueEvent(EventPtr const& event);
    void QueueEvent(EventPtr&& event);

  private:
    using SubscriberList = std::vector<EventCallback>;
    using SubscriberMap = std::unordered_map<EventType, SubscriberList>;

    friend class Application; // allow only application to invoke DispatchAll function
    friend class EditorApplication;

    /*!*********************************************************************
    \brief
      Dispatches all events currently in the queue by invoking the
      respective callbacks subscribed
    ************************************************************************/
    void DispatchAll();

    // list of subscribers for each event
    // each event in the map will have its own list of subscribers
    SubscriberMap mSubscribers;
    std::queue<EventPtr> mEventQueue; // events to be dispatched
  };


  /*!*********************************************************************
    \brief
      Subscribe a class member function to an event
      e.g. SUBSCRIBE_CLASS_FUNC(Events::EventType::KEY_TRIGGERED, &MyClass::MyCallback, &MyClass.GetInstance());
    \param eventType
      The type of the event
    \param callbackFn
      The class member function to be used as the callback
    \param instance
      The instance of the class
    ************************************************************************/
#define SUBSCRIBE_CLASS_FUNC(eventType, callbackFn, instance) \
  Events::EventManager::GetInstance().Subscribe(eventType, std::bind(callbackFn, instance, std::placeholders::_1))

    // Subscribe a static function to an event
    /*!*********************************************************************
    \brief
      Subscribe a static function to an event
      e.g. SUBSCRIBE_STATIC_FUNC(Events::EventType::KEY_TRIGGERED, MyCallback);
    \param eventType
      The type of the event
    \param callbackFn
      The function to be used as the callback
    ************************************************************************/
#define SUBSCRIBE_STATIC_FUNC(eventType, callbackFn) \
  Events::EventManager::GetInstance().Subscribe(eventType, std::bind(callbackFn, std::placeholders::_1))

    /*!*********************************************************************
    \brief
      Adds an event to the queue given the class and the arguments
      e.g. QUEUE_EVENT(Events::KeyTriggeredEvent, KEY_K);
    \param EventClass
      The class type of the event (like you would use in a template argument <>)
    \param ...
      Arguments to construct the class (variadic)
    ************************************************************************/
#define QUEUE_EVENT(EventClass, ...) \
  Events::EventManager::GetInstance().QueueEvent(std::make_shared<EventClass>(__VA_ARGS__))

} // namespace Events