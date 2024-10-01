#pragma once
#include <string>

namespace Component {
  struct Layer {
    Layer() : name{ "Default" } {}
    Layer(std::string const& layer) : name{ layer } {}

    inline void Clear() noexcept { name = "Default"; }

    std::string name;
  };
} // namespace Component
