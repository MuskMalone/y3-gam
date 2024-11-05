/*!*********************************************************************
\file   Material.h
\author 
\date   5-October-2024
\brief  Definition of Material component, which determines the texture
        for the object when it is rendered.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include "Color.h"
#include "Graphics/Material.h"

namespace Component {
    struct Material {
        Material() {

            auto mat =  Graphics::Material::Create(Graphics::ShaderLibrary::Get("PBR"));
            //matIdx = Graphics::MaterialTable::AddMaterial(mat);
        }

      /*!*********************************************************************
      \brief
        Resets the material to default values
      ************************************************************************/
        inline void Clear() noexcept {
            matIdx = 0;
            shaderName = "PBR";

            // Reset component and backend material to default values
            SetAlbedoColor(glm::vec3(1.0f));
            SetMetalness(0.0f);
            SetRoughness(0.5f);
            SetAO(1.0f);
            SetEmission(0.0f);
            SetTransparency(1.0f);
            SetTiling(glm::vec2(1.0f));
            SetOffset(glm::vec2(0.0f));

            // Reset texture GUIDs
            SetAlbedoMap(IGE::Assets::GUID{});
            SetNormalMap(IGE::Assets::GUID{});
            SetMetalnessMap(IGE::Assets::GUID{});
            SetRoughnessMap(IGE::Assets::GUID{});
        }


      const std::string& GetShaderName() const { return shaderName; }
      void SetShaderName(const std::string& name) { shaderName = name; }

      // Albedo color
      glm::vec3 GetAlbedoColor() const { return albedoColor; }
      void SetAlbedoColor(const glm::vec3& color) {
          albedoColor = color;
         // material->SetAlbedoColor(color);
      }

      // Metalness
      float GetMetalness() const { return metalness; }
      void SetMetalness(float value) {
          metalness = value;
          //material->SetMetalness(value);
      }

      // Roughness
      float GetRoughness() const { return roughness; }
      void SetRoughness(float value) {
          roughness = value;
          //material->SetRoughness(value);
      }

      // Ambient Occlusion (AO)
      float GetAO() const { return ao; }
      void SetAO(float aoValue) {
          ao = aoValue;
          //material->SetAO(aoValue);
      }

      // Emission
      float GetEmission() const { return emission; }
      void SetEmission(float value) {
          emission = value;
          //material->SetEmission(value);
      }

      // Transparency
      float GetTransparency() const { return transparency; }
      void SetTransparency(float value) {
          transparency = value;
          //material->SetTransparency(value);
      }

      // Tiling
      glm::vec2 GetTiling() const { return tiling; }
      void SetTiling(const glm::vec2& tilingValue) {
          tiling = tilingValue;
          //material->SetTiling(tilingValue);
      }

      // Offset
      glm::vec2 GetOffset() const { return offset; }
      void SetOffset(const glm::vec2& offsetValue) {
          offset = offsetValue;
          //material->SetOffset(offsetValue);
      }

      // Texture maps
      IGE::Assets::GUID GetAlbedoMap() const { return albedoMap; }
      void SetAlbedoMap(const IGE::Assets::GUID& texture) {
          albedoMap = texture;
          //material->SetAlbedoMap(texture);
      }

      IGE::Assets::GUID GetNormalMap() const { return normalMap; }
      void SetNormalMap(const IGE::Assets::GUID& texture) {
          normalMap = texture;
          //material->SetNormalMap(texture);
      }

      IGE::Assets::GUID GetMetalnessMap() const { return metalnessMap; }
      void SetMetalnessMap(const IGE::Assets::GUID& texture) {
          metalnessMap = texture;
          //material->SetMetalnessMap(texture);
      }

      IGE::Assets::GUID GetRoughnessMap() const { return roughnessMap; }
      void SetRoughnessMap(const IGE::Assets::GUID& texture) {
          roughnessMap = texture;
          //material->SetRoughnessMap(texture);
      }
      //std::shared_ptr<Graphics::Material> material;
      //uint32_t matIdx = 0;

              // Material properties
     // std::shared_ptr<Graphics::Material> material;
      uint32_t matIdx = 0;
      std::string shaderName;
      glm::vec3 albedoColor{ 1.0f, 1.0f, 1.0f };
      float metalness{ 0.0f };
      float roughness{ 0.5f };
      float ao{ 1.0f };
      float emission{ 0.0f };
      float transparency{ 1.0f };
      uint32_t flags{ 0 };
      glm::vec2 tiling{ 1.0f, 1.0f };
      glm::vec2 offset{ 0.0f, 0.0f };

      // Texture GUIDs for material maps
      IGE::Assets::GUID albedoMap{};
      IGE::Assets::GUID normalMap{};
      IGE::Assets::GUID metalnessMap{};
      IGE::Assets::GUID roughnessMap{};
    };
}