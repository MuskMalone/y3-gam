/*!*********************************************************************
\file   Inspector.cpp
\author
\date   5-October-2024
\brief  Class encapsulating functions to run the inspector / property
        window of the editor. Displays and allows modification of
        components for the currently selected file.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "Inspector.h"
#include <Asset/IGEAssets.h>
#include <Graphics/MaterialData.h>

namespace GUI {

  void Inspector::RunFileInspector() {
    std::filesystem::path const selectedFile{ GUIVault::GetSelectedFile() };
    if (selectedFile.empty() || selectedFile.extension() != gMaterialFileExt) { return; }

    IGE::Assets::AssetManager& am{ IGE_ASSETMGR };
    auto& selectedMaterial{ am.GetAsset<IGE::Assets::MaterialAsset>(am.LoadRef<IGE::Assets::MaterialAsset>(selectedFile.string()))->mMaterial };

    // @TODO: material inspector here
    ImGui::Text(selectedFile.filename().string().c_str());
  }

} // namespace GUI
