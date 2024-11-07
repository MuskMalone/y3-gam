#include "pch.h"
#include "Core/Components/MaterialComponent.h"
#include "Graphics/Material.h"

namespace Component {

    MaterialComponent::MaterialComponent() : matIdx(0), shaderName("PBR") {
        auto mat = Graphics::Material::Create(Graphics::ShaderLibrary::Get("PBR"));
        // matIdx = Graphics::MaterialTable::AddMaterial(mat);
    }

    void MaterialComponent::Clear() noexcept {
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

    const std::string& MaterialComponent::GetShaderName() const { return shaderName; }
    void MaterialComponent::SetShaderName(const std::string& name) { shaderName = name; }

    glm::vec3 MaterialComponent::GetAlbedoColor() const { return albedoColor; }
    void MaterialComponent::SetAlbedoColor(const glm::vec3& color) {
        albedoColor = color;
    }

    float MaterialComponent::GetMetalness() const { return metalness; }
    void MaterialComponent::SetMetalness(float value) {
        metalness = value;
    }

    float MaterialComponent::GetRoughness() const { return roughness; }
    void MaterialComponent::SetRoughness(float value) {
        roughness = value;
    }

    float MaterialComponent::GetAO() const { return ao; }
    void MaterialComponent::SetAO(float aoValue) {
        ao = aoValue;
    }

    float MaterialComponent::GetEmission() const { return emission; }
    void MaterialComponent::SetEmission(float value) {
        emission = value;
    }

    float MaterialComponent::GetTransparency() const { return transparency; }
    void MaterialComponent::SetTransparency(float value) {
        transparency = value;
    }

    glm::vec2 MaterialComponent::GetTiling() const { return tiling; }
    void MaterialComponent::SetTiling(const glm::vec2& tilingValue) {
        tiling = tilingValue;
    }

    glm::vec2 MaterialComponent::GetOffset() const { return offset; }
    void MaterialComponent::SetOffset(const glm::vec2& offsetValue) {
        offset = offsetValue;
    }

    IGE::Assets::GUID MaterialComponent::GetAlbedoMap() const { return albedoMap; }
    void MaterialComponent::SetAlbedoMap(const IGE::Assets::GUID& texture) {
        albedoMap = texture;
    }

    IGE::Assets::GUID MaterialComponent::GetNormalMap() const { return normalMap; }
    void MaterialComponent::SetNormalMap(const IGE::Assets::GUID& texture) {
        normalMap = texture;
    }

    IGE::Assets::GUID MaterialComponent::GetMetalnessMap() const { return metalnessMap; }
    void MaterialComponent::SetMetalnessMap(const IGE::Assets::GUID& texture) {
        metalnessMap = texture;
    }

    IGE::Assets::GUID MaterialComponent::GetRoughnessMap() const { return roughnessMap; }
    void MaterialComponent::SetRoughnessMap(const IGE::Assets::GUID& texture) {
        roughnessMap = texture;
    }

}
