#version 460 core
//#extension GL_ARB_bindless_texture : require

struct MaterialProperties {
    vec2 Tiling;
    vec2 Offset;
    vec4 AlbedoColor;  // Base color
    float Metalness;   // Metalness factor
    float Roughness;   // Roughness factor
    float Transparency; // Transparency (alpha)
    float AO;          // Ambient occlusion
    vec4 Emission;
};

layout(std430, binding = 0) buffer MaterialPropsBuffer {
    MaterialProperties materials[];
};

layout(location = 0) out vec4 fragColor;
layout(location = 1) out int entityID;
layout(location = 2) out vec4 viewPosition;
layout(location = 3) out vec4 bloomColor;

in vec4 v_Color;
in vec2 v_TexCoord;
in flat float v_TexIdx; // not being used might delete

in flat int v_EntityID;
in flat int v_MaterialIdx;

in vec3 v_FragPos;      // Fragment position in world space
in vec3 v_Normal;       // Normal in world space
in vec3 v_Tangent;      // Tangent in world space
in vec3 v_Bitangent;    // Bitangent in world space

// shadows
in vec4 v_LightSpaceFragPos;

in flat vec4 v_BloomProps;

uniform bool u_ShadowsActive;
uniform float u_ShadowBias;
uniform int u_ShadowSoftness;
uniform sampler2D u_ShadowMap;

uniform int u_MatIdxOffset;
uniform sampler2D[16] u_AlbedoMaps;
uniform sampler2D[16] u_NormalMaps;

void main() {
    entityID = v_EntityID;
    MaterialProperties mat = materials[v_MaterialIdx];
    vec2 texCoord = v_TexCoord * mat.Tiling + mat.Offset;

    // Corrected: use addition for the material index offset
    vec4 albedoTexture = texture2D(u_AlbedoMaps[int(v_MaterialIdx) + u_MatIdxOffset], texCoord);
    vec3 albedo = albedoTexture.rgb * mat.AlbedoColor.rgb; // Mixing texture and uniform

    // Change transparency here (you can choose to multiply with texture alpha if needed)
    float alpha = mat.Transparency * albedoTexture.a;
    fragColor = vec4(albedo, alpha);
}
