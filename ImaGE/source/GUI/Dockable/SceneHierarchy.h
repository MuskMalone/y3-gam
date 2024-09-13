#pragma once
#include <GUI/GUIWindow.h>
#include <set>

// forward declaration
namespace ECS { class Entity; }

namespace GUI
{

  class SceneHierarchy : public GUIWindow
  {
  public:
    SceneHierarchy(std::string const& name);

    void Run();

  private:
    static constexpr char sDragDropPayload[] = "ENTITY";

    void RecurseDownHeirarchy(ECS::Entity parent);
  };

} // namespace GUI
