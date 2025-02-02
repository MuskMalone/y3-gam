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
    vec4 Emission;     // Emission as vec4 (color + intensity)
};

layout(std430, binding = 0) buffer MaterialPropsBuffer {
    MaterialProperties materials[];
};

layout(location = 0) out vec4 fragColor;
layout(location = 1) out int entityID;

in vec2 v_TexCoord; // Texture coordinates
in flat int v_EntityID; // Entity ID
in flat int v_MaterialIdx; // Material index

uniform float u_Time; // Time for animation
uniform vec2 u_Resolution; // Screen resolution

void main() {
    // Assign entity ID for picking/rendering purposes
    entityID = v_EntityID;

    // Fetch material properties
    MaterialProperties mat = materials[v_MaterialIdx];

    // Normalize texture coordinates based on resolution
    vec2 uv = v_TexCoord * mat.Tiling + mat.Offset;

    // Background color from material
    vec3 baseColor = mat.AlbedoColor.rgb;

    // Matrix-based ripple distortion
    vec4 k = vec4(u_Time) * 0.8;
    k.xy = uv * 7.0;

    float val1 = length(0.5 - fract(k.xyw *= mat3(vec3(-2.0, -1.0, 0.0),
        vec3(3.0, -1.0, 1.0),
        vec3(1.0, -1.0, -1.0)) * 0.5));
    float val2 = length(0.5 - fract(k.xyw *= mat3(vec3(-2.0, -1.0, 0.0),
        vec3(3.0, -1.0, 1.0),
        vec3(1.0, -1.0, -1.0)) * 0.2));
    float val3 = length(0.5 - fract(k.xyw *= mat3(vec3(-2.0, -1.0, 0.0),
        vec3(3.0, -1.0, 1.0),
        vec3(1.0, -1.0, -1.0)) * 0.5));

    // Dynamic ripple intensity
    float ripple = pow(min(min(val1, val2), val3), 7.0) * 3.0;

    // Combine ripple effect with base color
    vec3 finalColor = baseColor + vec3(ripple);

    // Apply transparency from material
    float alpha = mat.Transparency;

    // Final output color
    fragColor = vec4(finalColor, alpha);
}
