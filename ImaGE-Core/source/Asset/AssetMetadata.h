#pragma once
#include <map>
#include <string>
namespace IGE{
    namespace Assets {
        struct AssetMetadata {
            using AssetProps = ::std::map<::std::string, ::std::string>;
            using AssetPropsKey = uint64_t; // this is wtv the guid has been converted to
            using AssetCategory = ::std::map<AssetPropsKey, AssetProps>;

            //split up bc deep nesting doesnt work
            using IGEProjProperties = ::std::map<::std::string, AssetCategory>;

            //std::map<
            //    std::string, 
            //    std::map<
            //        uint64_t, 
            //        std::map<
            //            std::string, 
            //            std::string
            //        >
            //    >
            //>
            //sample json output (mimics .vcproj)
            // "Assets":{
            //    "ModelAsset":{
            //        123456789 : { "path": "../path/to/file", "layers" : "3", "interpreter" : "fbx", ... yada yada }
            //    }
            //    "TextureAsset":{
            //        //same goes with texture assets
            //    }
            // }
            inline void Emplace(std::string const& assetCategory, IGE::Assets::GUID const& guid, AssetProps const& metadata) {
                //if no category exists create one
                if (mAssetProperties.find(assetCategory) == mAssetProperties.end()) {
                    mAssetProperties.emplace(assetCategory, AssetMetadata::AssetCategory{});
                }
                //if no asset with same guid exists insert this one
                if (mAssetProperties[assetCategory].find(guid) == mAssetProperties[assetCategory].end()) {
                    mAssetProperties[assetCategory].emplace(guid, metadata);
                }
            }
            IGEProjProperties mAssetProperties;


        };
    }
}
