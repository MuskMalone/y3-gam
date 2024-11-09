#pragma once
#include <vector>
#include <unordered_set>
#include <filesystem>
#include <Core/Entity.h>
#include "Styles/Styler.h"

namespace GUI {
  class GUIVault {
  public:
    /*!*********************************************************************
    \brief
      Gets the currently selected entity in the editor
    \return
      The currently selected entity
    ************************************************************************/
    static inline ECS::Entity GetSelectedEntity() noexcept { return sSelectedEntity; }

    /*!*********************************************************************
    \brief
      Sets the currently selected entity in the editor
    \param entity
      The entity to set
    ************************************************************************/
    static inline void SetSelectedEntity(ECS::Entity const& entity) noexcept { sSelectedEntity = entity; }

    // entity multi-select functions
    static inline std::unordered_set<ECS::Entity::EntityID> const& GetSelectedEntities() noexcept { return sSelectedEntities; }
    static inline bool IsEntitySelected(ECS::Entity const& entity) { return sSelectedEntities.contains(entity.GetRawEnttEntityID()); }
    static inline void AddSelectedEntity(ECS::Entity const& entity) { sSelectedEntities.emplace(entity.GetRawEnttEntityID()); }
    static void RemoveSelectedEntity(ECS::Entity const& entity) { sSelectedEntities.erase(entity.GetRawEnttEntityID()); }
    static void ClearSelectedEntities() { sSelectedEntities.clear(); }

    /*!*********************************************************************
    \brief
      Gets the styler of the engine
    \return
      Reference to the Styler
    ************************************************************************/
    static inline Styler& GetStyler() noexcept { return mStyler; }

  private:
    inline static Styler mStyler; // handles editor's styles

    inline static std::unordered_set<ECS::Entity::EntityID> sSelectedEntities;
    inline static std::filesystem::path mSelectedFile;
    inline static ECS::Entity sSelectedEntity; // currently selected entity
  };

} // namespace GUI
