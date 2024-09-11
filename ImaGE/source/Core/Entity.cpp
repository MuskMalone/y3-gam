#include <pch.h>
#include "Entity.h"

Entity::Entity(EntityID entID) : m_id{ entID } {}

uint32_t Entity::GetEntityID() const {
  return static_cast<uint32_t>(m_id);
}

Entity::EntityID Entity::GetRawEnttEntityID() const {
  return m_id;
}

std::string Entity::GetTag() const {
  return std::string();
}
