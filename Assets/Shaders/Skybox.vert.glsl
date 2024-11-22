#version 460 core

layout(location = 0) in vec2 in_Position; // Vertex position
layout(location = 1) in vec2 in_TexCoord; // Texture coordinate

out vec2 fragTexCoord; // Pass texture coordinate to fragment shader
out vec3 fragDir; // Direction vector for the fragment shader

uniform mat4 u_InvViewProj; 

uniform bool u_isEnvMap = false;

void main() {

    vec4 worldPos = u_InvViewProj * vec4(in_Position, 1.0, 1.0);
    fragDir = worldPos.xyz / worldPos.w; // Perspective divide

    fragTexCoord = in_TexCoord;
    gl_Position = vec4(in_Position, 0.0, 1.0);
}