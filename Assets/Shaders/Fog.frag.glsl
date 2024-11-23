#version 460 core

in vec2 v_TexCoord;

uniform sampler2D u_TexViewPosition;
uniform sampler2D u_TexFragColor;

layout(location=0) out vec4 fragColor;

void main()
{
    fragColor = vec4(texture(u_TexViewPosition, v_TexCoord), 1);
}