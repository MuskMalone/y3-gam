#include "pch.h"
#include <EditorApplication.h>

int Main(int argc, char** argv) {
  // To be read from settings file
  EditorApplication::ApplicationSpecification spec;
  spec.EnableImGui = true;
  spec.Fullscreen = false;
  spec.Name = "ImaGE Editor";
  spec.Resizable = true;
  spec.StartMaximized = false;
  spec.VSync = true;
  spec.WindowHeight = 1080;
  spec.WindowWidth = 1920;

  EditorApplication myApp{ spec };
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

#if NDEBUG

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
  return Main(__argc, __argv);
}

#else

int main(int argc, char** argv) {
  return Main(argc, argv);
}

#endif