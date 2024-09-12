/*!*********************************************************************
\file   EventCallback.h
\author chengen.lau\@digipen.edu
\date   10-September-2024
\brief  Macros to simplify event callback declaration and definition.

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
