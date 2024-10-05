#pragma once
#include <memory>
#include <Graphics/MeshFactory.h>
#include <Graphics/Mesh.h>

namespace Assets {
  template <typename ResourceType>
  struct PseudoAssetRef {
    using PseudoPartialRef = std::shared_ptr<ResourceType>;

    PseudoPartialRef ptr;
  };

  class PseudoAssetManager {
  public:
    template <typename ResourceType>
    std::shared_ptr<ResourceType> GetResource(PseudoAssetRef<ResourceType>& ref);

    template <typename ResourceType>
    void FreeResource(PseudoAssetRef<ResourceType>& ref);

  private:

  };

  template <typename ResourceType>
  std::shared_ptr<ResourceType> PseudoAssetManager::GetResource(PseudoAssetRef<ResourceType>& ref) {
    if (std::is_same<ResourceType, Graphics::Mesh>()) {
      return std::make_shared<Graphics::Mesh>(meshSrc);
    }
    else if (std::is_same<ResourceType, >()) {

    }

    return {};
  }

  template <typename ResourceType>
  void PseudoAssetManager::FreeResource(PseudoAssetRef<ResourceType>& ref) {
    ref.ptr.reset();
  }
} // namespace Assets
