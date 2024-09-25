#include <pch.h>
#include <Application.h>
#include "Asset/IGEAssets.h"
#define IGE_ASSETMGR_SAMPLE

int main()
{
#ifdef IGE_ASSETMGR_SAMPLE
	AssetManagerSampleTest();
#endif
  Application myApp{ "ImaGE", WINDOW_WIDTH<int>, WINDOW_HEIGHT<int> };

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
