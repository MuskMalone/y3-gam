#include "Application.h"
#include "GUI/GUIManager.h"

namespace IGE {
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

    /*!*********************************************************************
    \brief
      Wrapper function to print out exceptions.
    \param e
      Exception caught
    ************************************************************************/
    void PrintException(Debug::ExceptionBase& e);

    /*!*********************************************************************
    \brief
      Wrapper function to print out exceptions.
    \param e
      Exception caught
  ************************************************************************/
    void PrintException(std::exception& e);

    /*!*********************************************************************
    \brief
      Callback function for dragging files into the editor. Sends the
      filepaths received to the asset manager.
    \param window
      The window the file was dragged into
    \param pathCount
      The number of files
    \param paths
      The paths of the files
    ************************************************************************/
    static void WindowDropCallback(GLFWwindow*, int pathCount, const char* paths[]);
  };
} // namespace IGE
