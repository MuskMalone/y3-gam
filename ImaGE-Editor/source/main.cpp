#include "pch.h"
#include <EditorApplication.h>
#include <Serialization/Registration/RegisterClasses.h>
#include <Serialization/Registration/RegisterComponents.h>
#include <Serialization/Registration/RegisterEnumsAndFuncs.h>

int Main(int argc, char** argv) {
  // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  // To be read from settings file
  IGE::EditorApplication::ApplicationSpecification spec;
  spec.EnableImGui = true;
  spec.Fullscreen = false;
  spec.Name = "ImaGE Editor";
  spec.Resizable = true;
  spec.StartMaximized = false;
  spec.VSync = true;
  spec.WindowHeight = 1080;
  spec.WindowWidth = 1920;

  IGE::EditorApplication myApp{ spec };
  try
  {
    myApp.Init();

    myApp.Run();
  }
#ifdef _DEBUG
  catch (std::exception const& e)
  {
    std::cerr << e.what() << std::endl;
  }
#else
  catch (std::exception const&)
  {

  }
#endif

  myApp.Shutdown();
  return 0;
}

#if DISTRIBUTION

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
  return Main(__argc, __argv);
}

#else

int main(int argc, char** argv) {
  return Main(argc, argv);
}

#endif