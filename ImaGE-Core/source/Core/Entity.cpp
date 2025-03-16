#include <pch.h>
#include "Entity.h"
#include <Core/Components/Tag.h>
#include <Core/Components/Layer.h>

#include <Events/EventManager.h>
#include <Events/EventCallback.h>

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
    if (!this->HasComponent<Component::Tag>()) {
      /*Debug::DebugLogger::GetInstance().LogWarning("[Entity] Entity does not have Tag Component!");
      return std::string();*/
      throw Debug::Exception<Entity>(Debug::LVL_CRITICAL, Msg("Entity " + std::to_string(GetEntityID()) + " does not have Tag Component!"));
    }

    Component::Tag const& tagComponent{ this->GetComponent<Component::Tag>() };
    return tagComponent.tag;
  }

  void Entity::SetTag(std::string const& tag) {
    if (!this->HasComponent<Component::Tag>()) {
      Debug::DebugLogger::GetInstance().LogWarning("[Entity] Entity " + std::to_string(GetEntityID()) + " does not have Tag Component!");
      return;
    }

    Component::Tag& tagRef{ this->GetComponent<Component::Tag>() };
    tagRef.tag = tag;
  }

  bool Entity::IsActive() const {
    if (!this->HasComponent<Component::Tag>()) {
      Debug::DebugLogger::GetInstance().LogWarning("[Entity] Entity " + std::to_string(GetEntityID()) + " does not have Tag Component!");
      return false;
    }

    return this->GetComponent<Component::Tag>().isActive;
  }

  void Entity::SetIsActive(bool isActiveFlag) {
    if (!this->HasComponent<Component::Tag>()) {
      Debug::DebugLogger::GetInstance().LogWarning("[Entity] Entity " + std::to_string(GetEntityID()) + " does not have Tag Component!");
      return;
    }

    this->GetComponent<Component::Tag>().isActive = isActiveFlag;
  }

  void Entity::SetLayer(std::string layerName) {
    if (!this->HasComponent<Component::Layer>()) {
      Debug::DebugLogger::GetInstance().LogWarning("[Entity] Entity " + std::to_string(GetEntityID()) + " does not have Layer Component!");
      return;
    }

    std::string oldLayer = this->GetComponent<Component::Layer>().name;
    this->GetComponent<Component::Layer>().name = layerName;
    QUEUE_EVENT(Events::EntityLayerModified, *this, oldLayer);
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
      IGE_EVENTMGR.DispatchImmediateEvent<Events::RemoveComponentEvent>(*this, type);
    }
  }

} // namespace ECS
