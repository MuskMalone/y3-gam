#version 460 core

layout(location = 0) out vec4 fragColor;

in vec4 v_Color;
in vec2 v_TexCoord;
in flat float v_TexIdx;

uniform sampler2D u_Tex[32]; //TODO CHANGE THIS IN FUTURE

void main(){
	//vec4 texColor = texture2D(u_Tex[int(v_TexIdx)], v_TexCoord);
	vec4 texColor = texture2D(u_Tex[0], v_TexCoord);
	fragColor = texColor * v_Color;
}