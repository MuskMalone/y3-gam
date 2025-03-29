#version 460 core

layout(location = 0) out vec4 fragColor;
layout(location = 1) out int entity;

in vec4 v_Color;
in vec2 v_TexCoord;
in flat float v_TexIdx;
in flat int v_Entity;

uniform sampler2D u_Tex[32]; //TODO CHANGE THIS IN FUTURE
uniform float u_Gamma;

void main() {
    vec4 texColor = texture(u_Tex[int(v_TexIdx)], v_TexCoord);
    vec4 finalColor = (texColor * v_Color); 
    fragColor = vec4(pow(vec3(finalColor), vec3(1.0 / u_Gamma )* 1.8 ), finalColor.a);
    entity = v_Entity;
}
