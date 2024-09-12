#include <pch.h>
#include "Entity.h"

Entity::Entity(EntityID entityID) : m_id{ entityID } {}

uint32_t Entity::GetEntityID() const {
  return static_cast<uint32_t>(m_id);
}

Entity::EntityID Entity::GetRawEnttEntityID() const {
  return m_id;
}

std::string Entity::GetTag() const {
  Component::Tag const& tagComponent{ this->GetComponent<Component::Tag>() };
  return tagComponent.tag;
}

Entity::operator bool() const {
  return m_id != entt::null;
}

bool Entity::operator==(const Entity& entity) const {
  return (*this).m_id == entity.m_id;
}

bool Entity::operator!=(const Entity& entity) const {
  return (*this).m_id != entity.m_id;
}