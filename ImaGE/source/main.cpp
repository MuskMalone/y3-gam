#include <pch.h>
#include <Application.h>
#include <Physics/Physics.h>
int main()
{
  try
  {
    Application myApp{ "ImaGE :)", WINDOW_WIDTH<int>, WINDOW_HEIGHT<int> };

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
  return 0;
}
