#pragma once
#include <GUI/GUIWindow.h>
#include <set>
#include <Core/Entity.h>

// forward declaration
namespace Scenes { class SceneManager; }

namespace GUI
{

  class SceneHierarchy : public GUIWindow
  {
  public:
    SceneHierarchy(std::string const& name);

    void Run();

  private:
    ECS::EntityManager& mEntityManager;
    Scenes::SceneManager& mSceneManager;
    ECS::Entity mRightClickedEntity; // used to hold the entity the menu was opened on
    bool mRightClickMenu, mEntityOptionsMenu, mPrefabPopup;

    static constexpr char sDragDropPayload[] = "ENTITY";

    ECS::Entity CreateNewEntity() const;
    void RecurseDownHierarchy(ECS::Entity entity);
    void RunRightClickMenu();
    void RunEntityOptions();
  };

} // namespace GUI
