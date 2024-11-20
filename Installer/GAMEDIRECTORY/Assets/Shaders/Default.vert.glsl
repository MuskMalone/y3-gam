#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIdx;
layout(location = 4) in vec3 a_Tangent;
layout(location = 5) in vec3 a_Bitangent;
layout(location = 6) in vec4 a_Color;

out vec4 v_Color;
out vec2 v_TexCoord;
out flat float v_TexIdx;

out vec3 v_FragPos;

out vec3 v_Normal;           // Normal in world space
out vec3 v_Tangent;          // Tangent in world space
out vec3 v_Bitangent;        // Bitangent in world space
		
uniform mat4 u_ViewProjMtx;
//uniform mat4 u_MdlViewMtx;

void main(){
	//gl_Position = u_ViewProjMtx * u_MdlViewMtx * vec4(a_Position, 1.0);

	v_Normal = a_Normal;
    v_Tangent = a_Tangent;
    v_Bitangent = a_Bitangent;

	v_FragPos = a_Position;
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_TexIdx = a_TexIdx;
	gl_Position = u_ViewProjMtx * vec4(a_Position, 1.0);
} 