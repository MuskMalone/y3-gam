#pragma once
#include <vector>
#include <unordered_set>
#include <filesystem>
#include <Core/Entity.h>
#include "Styles/Styler.h"
#include <Events/SceneEvents.h>
#include <Events/EventCallback.h>

namespace GUI {
  class GUIManager;

  class GUIVault {
  public:
    /*!*********************************************************************
    \brief
      Gets the currently selected entity in the editor
    \return
      The currently selected entity
    ************************************************************************/
    static inline ECS::Entity GetSelectedEntity() noexcept { return sSelectedEntity; }
    static inline ECS::Entity GetPrevSelectedEntity() noexcept { return sPreviousEntity; }

    /*!*********************************************************************
    \brief
      Sets the currently selected entity in the editor
    \param entity
      The entity to set
    ************************************************************************/
    static void SetSelectedEntity(ECS::Entity entity);

    // entity multi-select functions
    static inline std::unordered_set<ECS::Entity::EntityID> const& GetSelectedEntities() noexcept { return sSelectedEntities; }
    static inline bool IsEntitySelected(ECS::Entity const& entity) { return sSelectedEntities.contains(entity.GetRawEnttEntityID()); }
    static inline void AddSelectedEntity(ECS::Entity const& entity) { sSelectedEntities.emplace(entity.GetRawEnttEntityID()); }
    static void RemoveSelectedEntity(ECS::Entity const& entity) { sSelectedEntities.erase(entity.GetRawEnttEntityID()); }
    static void ClearSelectedEntities() { sSelectedEntities.clear(); }

    /*!*********************************************************************
    \brief
      Sets the currently selected file in the editor. Automatically sets
      the selected entity to {}.
    \param file
      The path of the file to set
    ************************************************************************/
    static void SetSelectedFile(std::filesystem::path const& file);
    static inline std::filesystem::path const& GetSelectedFile() noexcept { return mSelectedFile; }

    static inline bool IsSceneModified() noexcept { return sSceneModified; }

    /*!*********************************************************************
    \brief
      Gets the styler of the engine
    \return
      Reference to the Styler
    ************************************************************************/
    static inline Styler& GetStyler() noexcept { return mStyler; }

    inline static bool sDevTools = false;
    inline static bool sShowCulledEntities = false; // visualize culled entities in editor view
    inline static bool sSerializePrettyScene = true, sSerializePrettyPrefab = true; // serialization formats

  private:
    friend class GUIManager;  // allow GUIManager to subscribe events for this class

    inline static Styler mStyler; // handles editor's styles

    inline static std::unordered_set<ECS::Entity::EntityID> sSelectedEntities;
    inline static std::filesystem::path mSelectedFile;  // @TODO: should change to GUID in future
    inline static ECS::Entity sSelectedEntity, sPreviousEntity; // currently selected entity
    inline static bool sSceneModified = false;

    static EVENT_CALLBACK_DECL(OnSceneModified);
    static EVENT_CALLBACK_DECL(OnSceneStateChange);
    static EVENT_CALLBACK_DECL(OnSceneSave);
  };

} // namespace GUI
