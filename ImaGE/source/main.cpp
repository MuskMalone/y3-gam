#include <pch.h>
#include <Application.h>
#include "Asset/IGEAssets.h"


int main()
{
    {//testing for asset manager

        IGE::Assets::AssetManager am {};
        am.RegisterTypes<Float, Integer>();
        auto floatAssetGUID{ am.ImportAsset<Float>("../some/float/asset/here") };
        auto integerAssetGUID{ am.ImportAsset<Integer>("../other/integer/asset/here") };

        IGE::Assets::Ref<Float> floatRef = am.GetAsset<Float>(floatAssetGUID);
        IGE::Assets::Ref<Integer> integerRef = am.GetAsset<Integer>(integerAssetGUID);
        
        //always load before using Refs if they are just imported!!
        am.LoadRef(floatRef);
        am.LoadRef(integerRef);

        std::cout << "floatref pointer internal: " << floatRef->f << std::endl;
        std::cout << "integerref pointer internal: " << integerRef->i << std::endl;

        floatRef->f = 123.f;
        integerRef->i = 456;
        std::cout << "floatref pointer internal after assignment: " << floatRef->f << std::endl;
        std::cout << "integerref pointer internal after assignment: " << integerRef->i << std::endl;
    }
  try
  {
    Application myApp{ "ImaGE", WINDOW_WIDTH<int>, WINDOW_HEIGHT<int> };

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
