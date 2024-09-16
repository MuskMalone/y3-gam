#pragma once
#include <string>

namespace Component
{
  struct Layer
  {
    Layer() : layerName{ "NoLayer" } {};
    Layer(std::string const& layer) : layerName{ layer } {};

    inline void Clear() noexcept { layerName = "NoLayer"; }

    std::string layerName;
  };

} // namespace Component
