#version 460 core

in vec2 fragTexCoord; // Received texture coordinate from vertex shader

uniform sampler2D u_ScreenTex;

out vec4 fragColor;

void main() {
    fragColor = texture(u_ScreenTex, fragTexCoord);
}