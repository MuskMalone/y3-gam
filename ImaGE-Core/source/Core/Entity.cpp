#include <pch.h>
#include "Entity.h"
#include <Core/Components/Tag.h>
#include <Events/EventManager.h>

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

  void Entity::SetTag(std::string const& tag) {
    Component::Tag& tagRef{ this->GetComponent<Component::Tag>() };
    tagRef.tag = tag;
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

  void Entity::DispatchRemoveComponentEvent(std::initializer_list<rttr::type> types) {
    for (rttr::type const& type : types) {
      IGE_EVENT_MGR.DispatchImmediateEvent<Events::RemoveComponentEvent>(*this, type);
    }
  }

} // namespace ECS
