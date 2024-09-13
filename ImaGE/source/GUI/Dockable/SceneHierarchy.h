#pragma once
#include <GUI/GUIWindow.h>
#include <set>

#define HIERARCHY_DUMMY_DATA

namespace GUI
{

  class SceneHierarchy : public GUIWindow
  {
  public:
    SceneHierarchy(std::string const& name);

    void Run();

  private:
#ifdef HIERARCHY_DUMMY_DATA
    using Entity = unsigned;
    std::set<Entity> mEntities;
#endif

    static constexpr char sDragDropPayload[] = "ENTITY";

    void RecurseDownHeirarchy(Entity parent);
  };

} // namespace GUI
