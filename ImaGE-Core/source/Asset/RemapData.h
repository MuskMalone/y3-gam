#pragma once
#include <string>
#include <Asset/SmartPointer.h>
#include <Core/Entity.h>

namespace IGE::Assets {
  struct RemapData {
    RemapData() = default;
    RemapData(ECS::Entity _entity, IGE::Assets::GUID _guid, std::string _fileType) :
      entity{ _entity }, guid{ _guid }, fileType{ std::move(_fileType) } {}

    std::string fileType;
    IGE::Assets::GUID guid;
    ECS::Entity entity;
  };
}
