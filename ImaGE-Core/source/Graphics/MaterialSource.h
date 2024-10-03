#pragma once

namespace Graphics {

    enum class MaterialFlag {
        NONE = 0,
        DEPTH_TEST = 1 << 0,
        BLEND = 1 << 1,
        TWO_SIDED = 1 << 2,
        DISABLE_SHADOW_CASTING = 1 << 3
    };

	class MaterialSource {
    public:
        MaterialSource(std::shared_ptr<Shader> const& shader, std::string const& = "");
        static std::shared_ptr<MaterialSource> Create(std::shared_ptr<Shader> const& shader);

        inline std::shared_ptr<Shader> GetShader() const { return mShader;}
        inline uint32_t GetFlags() { return mFlags; }
        inline void SetFlags(uint32_t flags) { mFlags = flags; }
        std::string const& GetName() const { return mName; }
        bool GetFlag(MaterialFlag flag) const;
        void SetFlag(MaterialFlag flag, bool value);

        // Default properties for instances
        inline glm::vec3 GetDefaultAlbedoColor() const { return mDefaultAlbedoColor; }
        inline float GetDefaultMetalness() const { return mDefaultMetalness; }
        inline float GetDefaultRoughness() const { return mDefaultRoughness; }
        inline float GetDefaultAO() const { return mDefaultAO; }
        inline float GetDefaultEmission() const { return mDefaultEmission; }
        inline float GetDefaultTransparency() const { return mDefaultTransparency; }

        // Set default properties for instances
        inline void SetDefaultAlbedoColor(const glm::vec3& color) { mDefaultAlbedoColor = color; }
        inline void SetDefaultMetalness(float metalness) { mDefaultMetalness = metalness; }
        inline void SetDefaultRoughness(float roughness) { mDefaultRoughness = roughness; }
        inline void SetDefaultAO(float ao) { mDefaultAO = ao; }
        inline void SetDefaultEmission(float emission) { mDefaultEmission = emission; }
        inline void SetDefaultTransparency(float transparency) { mDefaultTransparency = transparency; }

        // Default Texture
        std::shared_ptr<Texture> GetAlbedoMap() const;
        void SetAlbedoMap(std::shared_ptr<Texture> texture);

        std::shared_ptr<Texture> GetNormalMap() const;
        void SetNormalMap(std::shared_ptr<Texture> texture);

        std::shared_ptr<Texture> GetMetalnessMap() const;
        void SetMetalnessMap(std::shared_ptr<Texture> texture);

        std::shared_ptr<Texture> GetRoughnessMap() const;
        void SetRoughnessMap(std::shared_ptr<Texture> texture);

    private:
        std::shared_ptr<Shader> mShader;
        std::string mName;
        uint32_t mFlags;

        // Default properties for material instances
        glm::vec3 mDefaultAlbedoColor{ 1.0f, 1.0f, 1.0f };  // Default white albedo
        float mDefaultMetalness{ 0.0f };
        float mDefaultRoughness{ 0.5f };
        float mDefaultAO{ 1.0f }; // Default AO intensity
        float mDefaultEmission{ 0.0f };
        float mDefaultTransparency{ 1.0f }; // Fully opaque by default

        // Default texture maps
        std::shared_ptr<Texture> mAlbedoMap;
        std::shared_ptr<Texture> mNormalMap;
        std::shared_ptr<Texture> mMetalnessMap;
        std::shared_ptr<Texture> mRoughnessMap;
	};

}