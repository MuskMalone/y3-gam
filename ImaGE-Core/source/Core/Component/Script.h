#pragma once
#include <string>

namespace Component {
  struct Script {
    Script() : Script{ "None" } {}
    Script(std::string const& scriptName) : name{ scriptName } {}

    inline void Clear() noexcept { name = "None"; }

    std::string name;
  };
} // namespace Component