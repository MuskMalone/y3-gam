#pragma once
#include <pch.h>

namespace Component {
  struct Tag {
    Tag() : tag{ "No Name" } {};
    Tag(std::string const& tagString) : tag{ tagString } {};

    inline void Clear() noexcept { tag = "No Name"; }

    std::string tag;
  };

  struct Transform {
    Transform(glm::vec3 const& _pos = {}, glm::vec3 const& _scale = { 1.f, 1.f, 1.f }, glm::vec3 const& _rot = {})
      : pos{ _pos }, scale{ 1.f, 1.f, 1.f }, rot{ _rot }, baseScale{ _scale }, worldPos{} {}

    glm::vec3 pos, scale, rot;
    glm::vec3 worldPos;

    inline glm::vec3 const& GetBaseScale() const noexcept { return baseScale; }
    inline void Clear() noexcept { pos = rot = {}; scale = glm::vec3(1.f); }

  private:
    glm::vec3 baseScale;
  };
};