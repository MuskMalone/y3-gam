#pragma once

namespace Component
{
    //struct Skybox
    //{
    //    // Default constructor to initialize color to white and texture to an invalid GUID

    //    Skybox() = default;

    //    Skybox(IGE::Assets::GUID guid)
    //        : materialAsset(guid)
    //    {
    //        if(guid.IsValid())
    //            Graphics::MaterialTable::AddMaterialByGUID(guid);
    //    }
    //    /*!*********************************************************************
    //    \brief
    //      Resets the component to default values.
    //    ************************************************************************/
    //    inline void Clear() noexcept {
    //        materialAsset = {};
    //    }

    //    inline void SetGUID(const IGE::Assets::GUID& guid) {
    //        materialAsset = guid;
    //        if (!guid) { return; }

    //        Graphics::MaterialTable::AddMaterialByGUID(guid); // Update matIdx
    //    }

    //    IGE::Assets::GUID materialAsset;  // material GUID for skybox
    //};

    struct Skybox
    {
        // Default constructor to initialize color to white and texture to an invalid GUID

        Skybox() = default;

        /*!*********************************************************************
        \brief
          Resets the component to default values.
        ************************************************************************/
        inline void Clear() noexcept {
            tex1 = {};
            tex2 = {};
            blend = 0.f;
        }

        IGE::Assets::GUID tex1{};
        IGE::Assets::GUID tex2{};
        float blend{};
    };

} // namespace Component
