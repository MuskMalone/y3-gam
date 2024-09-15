#pragma once
#include <string>
#include <iostream>

namespace Component
{

  struct Layer
  {
    Layer() : layerName{ "NoLayer" } {}
    Layer(std::string const& layer) : layerName{ layer } { std::cout << "Ctor: " << layer << '\n'; }

    std::string layerName;
  };

} // namespace Component
