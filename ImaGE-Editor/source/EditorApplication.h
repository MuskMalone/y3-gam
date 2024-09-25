#include "Application.h"
#include "GUI/GUIManager.h"

class EditorApplication : public Application {
public:

  EditorApplication(EditorApplication::ApplicationSpecification const& spec);
  ~EditorApplication();

  virtual void Init() override;
  virtual void Run() override;
  virtual void SetCallbacks() override;

private:
  GUI::GUIManager mGUIManager;

private:
  void ImGuiStartFrame() const;
  static void WindowDropCallback(GLFWwindow*, int pathCount, const char* paths[]);
};