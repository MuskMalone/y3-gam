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
#include <Singleton/ThreadSafeSingleton.h>
#include "Events.h"
#include <typeindex>
#include <functional>
#include <vector>
#include <queue>
#include <memory>

// forward declaration
namespace IGE { class Application; class EditorApplication; }

namespace Events
{

  class EventManager : public ThreadSafeSingleton<EventManager>
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
    template <typename T>
    void Subscribe(EventCallback const& callback) {
      mSubscribers[typeid(T)].emplace_back(callback);
    }

    /*!*********************************************************************
    \brief
      Adds an event to the queue. All respective listeners will be notified
      when the event is dispatched at the start of the game loop.
    \param event
      The event to queue
    ************************************************************************/
    template <typename T>
    void QueueEvent(EventPtr const& event) {
      mEventQueue.emplace(typeid(T), event);
    }

    template <typename T>
    void QueueEvent(EventPtr&& event) {
      mEventQueue.emplace(typeid(T), std::move(event));
    }

    /*!*********************************************************************
    \brief
      >>> For use only if event has to be handled immediately <<<
      Dispatches an immediately. All respective listeners will be notified
      when the event is dispatched at the start of the game loop.
    \param event
      The event to queue
    ************************************************************************/
    template <typename EventClass, typename... Args>
    void DispatchImmediateEvent(Args&& ...args);

  private:
    using SubscriberList = std::vector<EventCallback>;
    using SubscriberMap = std::unordered_map<std::type_index, SubscriberList>;

    // allow only application to invoke DispatchAll function
    friend class IGE::Application; 
    friend class IGE::EditorApplication;

    /*!*********************************************************************
    \brief
      Dispatches all events currently in the queue by invoking the
      respective callbacks subscribed
    ************************************************************************/
    void DispatchAll();

    // list of subscribers for each event
    // each event in the map will have its own list of subscribers
    SubscriberMap mSubscribers;
    std::queue<std::pair<std::type_index, EventPtr>> mEventQueue; // events to be dispatched
  };

  template <typename EventClass, typename... Args>
  void EventManager::DispatchImmediateEvent(Args&& ...args) {
    std::shared_ptr<EventClass> eventToDispatch{ std::make_shared<EventClass>(std::forward<Args>(args)...) };

    SubscriberList& subscribers{ mSubscribers[typeid(EventClass)]};
    for (auto& fn : subscribers) {
      fn(eventToDispatch);
    }
  }

#define IGE_EVENTMGR Events::EventManager::GetInstance()

  /*!*********************************************************************
    \brief
      Subscribe a class member function to an event
      e.g. SUBSCRIBE_CLASS_FUNC(Events::KeyTriggeredEvent, &MyClass::MyCallback, &MyClass.GetInstance());
    \param EventClass
      The class of the event
    \param callbackFn
      The class member function to be used as the callback
    \param instance
      The instance of the class
    ************************************************************************/
#define SUBSCRIBE_CLASS_FUNC(EventClass, callbackFn, instance) \
  Events::EventManager::GetInstance().Subscribe<EventClass>(std::bind(callbackFn, instance, std::placeholders::_1))

    // Subscribe a static function to an event
    /*!*********************************************************************
    \brief
      Subscribe a static function to an event
      e.g. SUBSCRIBE_STATIC_FUNC(Events::KeyTriggeredEvent, MyCallback);
    \param EventClass
      The class of the event
    \param callbackFn
      The function to be used as the callback
    ************************************************************************/
#define SUBSCRIBE_STATIC_FUNC(EventClass, callbackFn) \
  Events::EventManager::GetInstance().Subscribe<EventClass>(std::bind(callbackFn, std::placeholders::_1))

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
  Events::EventManager::GetInstance().QueueEvent<EventClass>(std::make_shared<EventClass>(__VA_ARGS__))

} // namespace Events