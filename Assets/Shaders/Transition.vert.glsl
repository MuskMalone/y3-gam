#version 460 core

layout(location = 0) in vec2 in_Position; // Vertex position
layout(location = 1) in vec2 in_TexCoord; // Texture coordinate

out vec2 fragTexCoord; // Pass texture coordinate to fragment shader

void main() {
    fragTexCoord = in_TexCoord;
    gl_Position = vec4(in_Position, 0.0, 1.0);
}