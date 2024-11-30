#pragma once
#include <Events/Events.h>
#include <Asset/SmartPointer.h>

namespace Events {

  // int pathCount, const char* paths[]
  class AddFilesFromExplorerEvent : public Event
  {
  public:
    AddFilesFromExplorerEvent(int pathCount, const char* paths[]) : Event(EventType::ADD_FILES) {
      for (int i{}; i < pathCount; ++i) {
        mPaths.emplace_back(paths[i]);
      }
    }
    inline std::string GetName() const noexcept override { return "Adding " + std::to_string(mPaths.size()) + " files from file explorer"; }

    std::vector<std::string> mPaths;
  };

  class RegisterAssetsEvent : public Event
  {
  public:
    RegisterAssetsEvent(std::vector<std::string> const& paths) : Event(EventType::REGISTER_FILES), mPaths{ paths } {}
    RegisterAssetsEvent(std::vector<std::string>&& paths) : Event(EventType::REGISTER_FILES), mPaths{ std::move(paths) } {}
    inline std::string GetName() const noexcept override { return "Registering " + std::to_string(mPaths.size()) + " files to AssetManager"; }

    std::vector<std::string> const mPaths;
  };

  // entity, guid, name of fileType
  class GUIDInvalidated : public Event
  {
  public:
    GUIDInvalidated(ECS::Entity entity, IGE::Assets::GUID guid, std::string fileType) : Event(EventType::INVALID_GUID),
      mEntity{ entity }, mGUID{ guid }, mFileType{ std::move(fileType) } {}
    inline std::string GetName() const noexcept override {
      return mFileType + " asset " + std::to_string(static_cast<uint64_t>(mGUID)) + " of Entity " + std::to_string(mEntity.GetEntityID()) + " has to be remapped";
    }

    std::string const mFileType;
    IGE::Assets::GUID const mGUID;
    ECS::Entity const mEntity;
  };

  class TriggerGUIDRemap: public Event
  {
  public:
    TriggerGUIDRemap() : Event(EventType::TRIGGER_GUID_REMAP) {}
    inline std::string GetName() const noexcept override { return "Triggering GUID remap interface"; }
  };

  // guid, newPath, assetTypeName
  class RemapGUID : public Event
  {
  public:
    RemapGUID(IGE::Assets::GUID guid, std::string path, std::string assetTypeName) : Event(EventType::GUID_REMAP),
      mPath{ std::move(path) }, mGUID{ guid }, mAssetType{ std::move(assetTypeName) } {}
    inline std::string GetName() const noexcept override {
      return "Adding guid " + std::to_string(static_cast<uint64_t>(mGUID)) + " to " + mPath;
    }

    std::string const mPath, mAssetType;
    IGE::Assets::GUID const mGUID;
  };

} // namespace Events
