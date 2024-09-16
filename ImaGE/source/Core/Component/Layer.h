#pragma once
#include <string>

namespace Component
{

  struct Layer
  {
    Layer() : layerName{ "NoLayer" } {};
    Layer(std::string const& layer) : layerName{ layer } {};

    std::string layerName;
  };

} // namespace Component
