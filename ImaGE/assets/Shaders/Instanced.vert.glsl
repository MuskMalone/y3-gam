#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIdx;
layout(location = 4) in vec3 a_Tangent;
layout(location = 5) in vec3 a_Bitangent;
layout(location = 6) in vec4 a_Color;

// New per-instance attributes (starting at location 7)
layout(location = 7) in mat4 a_ModelMatrix; // Model transformation matrix for each instance
layout(location = 11) in vec4 a_InstanceColor; // Instance-specific color

out vec4 v_Color;
out vec2 v_TexCoord;
out flat float v_TexIdx;

out vec3 v_FragPos;

out vec3 v_Normal;           // Normal in world space
out vec3 v_Tangent;          // Tangent in world space
out vec3 v_Bitangent;        // Bitangent in world space

uniform mat4 u_ViewProjMtx;

void main(){
    // Apply per-instance transformation to the vertex position
    vec4 worldPosition = a_ModelMatrix * vec4(a_Position, 1.0);
    gl_Position = u_ViewProjMtx * worldPosition;

    // Transform other attributes to world space
    v_Normal = mat3(a_ModelMatrix) * a_Normal;
    v_Tangent = mat3(a_ModelMatrix) * a_Tangent;
    v_Bitangent = mat3(a_ModelMatrix) * a_Bitangent;

    // Output per-instance attributes
    v_FragPos = worldPosition.xyz;
    v_Color = a_InstanceColor * a_Color; // Combine vertex color and instance color
    v_TexCoord = a_TexCoord;
    v_TexIdx = a_TexIdx;
}
