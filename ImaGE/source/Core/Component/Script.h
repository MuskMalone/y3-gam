#pragma once

#include <string>

namespace Component {
  struct Script {
    Script() : Script{ "None" } {}
    Script(std::string const& scriptName) : scriptName{ scriptName } {}

    inline void Clear() noexcept { scriptName = "None"; }

    std::string scriptName;
  };
} // namespace Component