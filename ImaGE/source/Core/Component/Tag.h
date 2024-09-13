#pragma once
#include <string>

namespace Component
{
  struct Tag
  {
    Tag() : tag{ "NoName" } {};
    Tag(std::string const& tagString) : tag{ tagString } {};

    inline void Clear() noexcept { tag = "NoName"; }

    std::string tag;
  };

} // namespace Component
