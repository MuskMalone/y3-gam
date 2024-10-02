#include <pch.h>

#include <Application.h>
//#define IGE_ASSETMGR_SAMPLE
#include "Asset/IGEAssets.h"


int main()
{

#ifdef IGE_ASSETMGR_SAMPLE
	AssetManagerSampleTest();
#endif
    IGEAssetsRegisterTypes();
    auto am{ IGE::Assets::AssetManager::GetInstance() };
    am->ImportAsset<IGE::Assets::TextureAsset>("C:\\Users\\terra\\Downloads\\testcases\\mushroom.png");
    am->ImportAsset<IGE::Assets::TextureAsset>("C:\\Users\\terra\\Downloads\\testcases\\man.jpg");
    am->ImportAsset<IGE::Assets::TextureAsset>("C:\\Users\\terra\\Downloads\\testcases\\cat.jpg");
    am->ImportAsset<IGE::Assets::TextureAsset>("C:\\Users\\terra\\Downloads\\testcases\\foot.png");
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