/*!*********************************************************************
\file   EventCallback.h
\author chengen.lau\@digipen.edu
\date   10-September-2024
\brief  Macros to simplify event callback declaration and definition.
        Example usage:
          // in ur constructor / init
          SUBSCRIBE_CLASS_FUNC(Events::EventType::EDIT_PREFAB, &MyClass::MyCallback, this);
          SUBSCRIBE_CLASS_FUNC(Events::EventType::EDIT_PREFAB, &MyClass::MyCallback2, this);

          // in .h
          EVENT_CALLBACK_DECL(MyCallback);

          // in .cpp
          EVENT_CALLBACK_DEF(PrefabEditor, MyCallback)
          {
            auto editPrefabEvent{ CAST_TO_EVENT(Events::EditPrefabEvent) };
            // access data members and do ur stuff
            // ...
          }

          EVENT_CALLBACK_DEF(PrefabEditor, MyCallback2)
          {
            auto keyTriggeredEvent{ CAST_TO_EVENT(Events::EditPrefabEvent) };
            // ...
          }

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <Events/Event.h>
#include <memory>

/*!*********************************************************************
\brief
  Macro for callback function declaration of signature
  void (std::shared_ptr<Events::Event> const& event
  Note that The argument is named "event"
\param FnName
  The name of the function
************************************************************************/
#define EVENT_CALLBACK_DECL(FnName) \
  void FnName(std::shared_ptr<Events::Event> const& event)

/*!*********************************************************************
\brief
  Macro for callback function definition of signature
  void (std::shared_ptr<Events::Event> const& event). Note that The 
  argument is named "event". Used for defining outside of class definition
\param ClassScope
  The name of the class
\param FnName
  The name of the function
************************************************************************/
#define EVENT_CALLBACK_DEF(ClassScope, FnName) \
  void ClassScope::FnName(std::shared_ptr<Events::Event> const& event)

// macro to cast a shared_ptr to an event class
#define CAST_TO_EVENT(eventClass) std::static_pointer_cast<eventClass>(event)
