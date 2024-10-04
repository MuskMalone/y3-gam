/*!*********************************************************************
\file   Layer.h
\author 
\date   5-October-2024
\brief  Definition of Layer component, which is used by the layer
        system to determine which entities can interact with one
        another
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <string>

namespace Component {
  struct Layer {
    Layer() : name{ "Default" } {}
    Layer(std::string const& layer) : name{ layer } {}

    /*!*********************************************************************
    \brief
      Resets the object to default values
    ************************************************************************/
    inline void Clear() noexcept { name = "Default"; }

    std::string name;
  };
} // namespace Component
