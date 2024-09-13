#pragma once
#include <pch.h>

namespace Component {
  struct Tag {
    Tag() : tag{ "NoName" } {};
    Tag(std::string const& tagString) : tag{ tagString } {};

    inline void Clear() noexcept { tag = "NoName"; }

    std::string tag;
  };

  struct Layer {
    Layer() : layerName{ "NoLayer" } {};
    Layer(std::string const& layer) : layerName{ layer } {};

    std::string layerName;
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

  struct Collider {
      JPH::Vec3 scale;
      JPH::Vec3 position;
      JPH::Quat rotation;

      JPH::EShapeSubType type;
      JPH::BodyID bodyID;
      //~Collider() {
      //    if (shape.GetPtr() != nullptr) {
      //        delete shape;
      //    }
      //}
  };

  struct RigidBody {
        
  };
};