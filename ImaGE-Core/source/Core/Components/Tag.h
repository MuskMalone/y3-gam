/*!*********************************************************************
\file   Tag.h
\author 
\date   5-October-2024
\brief  Definition of the Tag component, which stores the name of an
        Entity.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <string>

namespace Component
{
  struct Tag
  {
    Tag() : tag{ "NoName" } {}
    Tag(std::string const& tagString) : tag{ tagString } {}

    /*!*********************************************************************
    \brief
      Resets the component to default values
    ************************************************************************/
    inline void Clear() noexcept { tag = "NoName"; }

    std::string tag;
  };

} // namespace Component