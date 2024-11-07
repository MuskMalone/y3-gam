#include "pch.h"
#include "Core/Components/Material.h"
#include "Graphics/MaterialData.h"

namespace Component {

    Material::Material() : matIdx(0), shaderName("PBR") {
        auto mat = Graphics::MaterialData::Create(Graphics::ShaderLibrary::Get("PBR"));
        matIdx = Graphics::MaterialTable::AddMaterial(mat);
    }

    void Material::Clear() noexcept {
        matIdx = 0;
        shaderName = "PBR";

        SetAlbedoColor(glm::vec3(1.0f));
        SetMetalness(0.0f);
        SetRoughness(0.5f);
        SetAO(1.0f);
        SetEmission(0.0f);
        SetTransparency(1.0f);
        SetTiling(glm::vec2(1.0f));
        SetOffset(glm::vec2(0.0f));

        SetAlbedoMap(IGE::Assets::GUID{});
        SetNormalMap(IGE::Assets::GUID{});
        SetMetalnessMap(IGE::Assets::GUID{});
        SetRoughnessMap(IGE::Assets::GUID{});
    }

    const std::string& Material::GetShaderName() const { return shaderName; }
    void Material::SetShaderName(const std::string& name) { shaderName = name; }

    glm::vec3 Material::GetAlbedoColor() const { return albedoColor; }
    void Material::SetAlbedoColor(const glm::vec3& color) {
        albedoColor = color;
    }

    float Material::GetMetalness() const { return metalness; }
    void Material::SetMetalness(float value) {
        metalness = value;
    }

    float Material::GetRoughness() const { return roughness; }
    void Material::SetRoughness(float value) {
        roughness = value;
    }

    float Material::GetAO() const { return ao; }
    void Material::SetAO(float aoValue) {
        ao = aoValue;
    }

    float Material::GetEmission() const { return emission; }
    void Material::SetEmission(float value) {
        emission = value;
    }

    float Material::GetTransparency() const { return transparency; }
    void Material::SetTransparency(float value) {
        transparency = value;
    }

    glm::vec2 Material::GetTiling() const { return tiling; }
    void Material::SetTiling(const glm::vec2& tilingValue) {
        tiling = tilingValue;
    }

    glm::vec2 Material::GetOffset() const { return offset; }
    void Material::SetOffset(const glm::vec2& offsetValue) {
        offset = offsetValue;
    }

    IGE::Assets::GUID Material::GetAlbedoMap() const { return albedoMap; }
    void Material::SetAlbedoMap(const IGE::Assets::GUID& texture) {
        albedoMap = texture;
    }

    IGE::Assets::GUID Material::GetNormalMap() const { return normalMap; }
    void Material::SetNormalMap(const IGE::Assets::GUID& texture) {
        normalMap = texture;
    }

    IGE::Assets::GUID Material::GetMetalnessMap() const { return metalnessMap; }
    void Material::SetMetalnessMap(const IGE::Assets::GUID& texture) {
        metalnessMap = texture;
    }

    IGE::Assets::GUID Material::GetRoughnessMap() const { return roughnessMap; }
    void Material::SetRoughnessMap(const IGE::Assets::GUID& texture) {
        roughnessMap = texture;
    }

}
