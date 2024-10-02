#include "Application.h"
#include "GUI/GUIManager.h"

class EditorApplication : public Application {
public:

  EditorApplication(EditorApplication::ApplicationSpecification const& spec);
  ~EditorApplication();

  void Init() override;
  void Run() override;
  void Shutdown() override;

private:
  GUI::GUIManager mGUIManager;

private:
  void ImGuiStartFrame() const;
  void SetEditorCallbacks();
  void PrintException(Debug::ExceptionBase& e);
  void PrintException(std::exception& e);

  static void WindowDropCallback(GLFWwindow*, int pathCount, const char* paths[]);
};