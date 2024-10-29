#version 460 core

uniform sampler2D u_depthTex;
in vec2 v_TexCoord;

layout(location = 0) out vec4 fragColor;

void main()
{
  float depth = texture(u_depthTex, v_TexCoord).r;
  fragColor = vec4(vec3(depth), 1.f);
}
