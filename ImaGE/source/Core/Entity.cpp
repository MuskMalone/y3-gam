#include <pch.h>
#include "Entity.h"
#include "Component/Tag.h"

namespace ECS
{

  Entity::Entity(EntityID entityID) : mId{ entityID } {}

  uint32_t Entity::GetEntityID() const {
    return static_cast<uint32_t>(mId);
  }

  Entity::EntityID Entity::GetRawEnttEntityID() const {
    return mId;
  }

  std::string const& Entity::GetTag() const {
    Component::Tag const& tagComponent{ this->GetComponent<Component::Tag>() };
    return tagComponent.tag;
  }

  Entity::operator bool() const {
    return mId != entt::null;
  }

  bool Entity::operator==(const Entity& entity) const {
    return (*this).mId == entity.mId;
  }

  bool Entity::operator!=(const Entity& entity) const {
    return (*this).mId != entity.mId;
  }

} // namespace ECS
