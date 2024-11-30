/*!*********************************************************************
\file   RemapData.h
\author chengen.lau\@digipen.edu
\date   30-November-2024
\brief  Struct used by the editor to store info if invalidated GUIDs
        to let the user remap them through a popup.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <string>
#include <vector>
#include <Asset/SmartPointer.h>
#include <Core/Entity.h>

namespace IGE::Assets {
  struct RemapData {
    RemapData() = default;
    RemapData(IGE::Assets::GUID _guid, std::string _fileType) :
      fileType{ std::move(_fileType) }, guid{ _guid }, entities{} {}

    std::string fileType; // the name of the asset registered to rttr
    IGE::Assets::GUID guid; // the original guid of the asset
    std::vector<ECS::Entity> entities;
  };
}
