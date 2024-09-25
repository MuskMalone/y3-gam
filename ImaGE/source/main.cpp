#include <pch.h>
#include <Application.h>
#include "Asset/IGEAssets.h"


int main()
{
    IGE::Assets::AssetManager am {};        am.RegisterTypes<Float, Integer>();
    auto floatAssetGUID{ am.ImportAsset<Float>("../some/float/asset/here") };
    auto integerAssetGUID{ am.ImportAsset<Integer>("../other/integer/asset/here") };
    am.LoadRef<Float>(floatAssetGUID);
    am.LoadRef<Integer>(integerAssetGUID);

    IGE::Assets::Ref<Float> floatRef { am.GetAsset<Float>(floatAssetGUID) };
    IGE::Assets::Ref<Integer> integerRef { am.GetAsset<Integer>(integerAssetGUID) };
    {//testing for asset manager
        std::cout << am.GetInstanceInfo(floatRef) << std::endl;
        std::cout << am.GetInstanceInfo(integerRef) << std::endl;

        std::cout << "floatref pointer internal: " << floatRef->f << std::endl;
        std::cout << "integerref pointer internal: " << integerRef->i << std::endl << std::endl;

        IGE::Assets::Ref<Float> floatRef1 { am.GetAsset<Float>(floatAssetGUID) };
        IGE::Assets::Ref<Integer> integerRef1 { am.GetAsset<Integer>(integerAssetGUID) };

        std::cout << am.GetInstanceInfo(floatRef1) << std::endl;
        std::cout << am.GetInstanceInfo(integerRef1) << std::endl;

        floatRef1->f = 123.f;
        integerRef1->i = 456;
        std::cout << "floatref pointer internal after assignment: " << floatRef->f << std::endl;
        std::cout << "integerref pointer internal after assignment: " << integerRef->i << std::endl << std::endl;

        std::cout << am.GetInstanceInfo(floatRef1) << std::endl;
        std::cout << am.GetInstanceInfo(integerRef1) << std::endl;
    }
    std::cout << am.GetInstanceInfo(floatRef) << std::endl;
    std::cout << am.GetInstanceInfo(integerRef) << std::endl;
//  try
//  {
//    Application myApp{ "ImaGE", WINDOW_WIDTH<int>, WINDOW_HEIGHT<int> };
//
//    myApp.Init();
//
//    myApp.Run();
//  }
//#ifdef _DEBUG
//  catch (std::exception const& e)
//  {
//    std::cerr << e.what() << std::endl;
//  }
//#else
//  catch (std::exception const&)
//  {
//
//  }
//#endif
  return 0;
}
