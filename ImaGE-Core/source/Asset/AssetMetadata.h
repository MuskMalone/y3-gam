#pragma once
#include <map>
#include <string>
namespace IGE{
    namespace Assets {
        struct AssetMetadata {
            using AssetProps = ::std::map<::std::string, ::std::string>;
            using AssetPropsKey = uint64_t; // this is wtv the guid has been converted to
            using AssetCategory = ::std::map<AssetPropsKey, AssetProps>;
            using IGEProjProperties = ::std::map<::std::string, AssetCategory>;
            //sample json output (mimics .vcproj)
            // "Assets":{
            //    "MeshAsset":{
            //        123456789 : { "path": "../path/to/file", "layers" : "3", "interpreter" : "fbx", ... yada yada }
            //    }
            //    "TextureAsset":{
            //        //same goes with texture assets
            //    }
            // }
            IGEProjProperties mAssetProperties;
        };
    }
}
