#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIdx;
//layout(location = 4) in int a_Entity;

out vec4 v_Color;
out vec2 v_TexCoord;
out flat float v_TexIdx;
//out flat int v_Entity;
		
uniform mat4 u_ViewProjMtx;
//uniform mat4 u_MdlViewMtx;

void main(){
	//gl_Position = u_ViewProjMtx * u_MdlViewMtx * vec4(a_Position, 1.0);
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_TexIdx = a_TexIdx;
	//v_Entity = a_Entity;
	gl_Position = u_ViewProjMtx * vec4(a_Position, 1.0);
} 
//layout(location = 0) in vec3 a_Position;
//layout(location = 1) in vec2 a_TexCoord;
//layout(location = 2) in vec4 a_Color;
//
//layout(location = 3) in mat4 a_ModelMatrix; // Model transformation matrix for each instance
//layout(location = 7) in int a_MaterialIdx; 
//layout(location = 8) in int a_Entity;
////layout(location = 7) in vec4 a_ColorInstanced; 
//
//out vec4 v_Color;
//out vec2 v_TexCoord;
//out flat float v_TexIdx;
//
//out int v_MaterialIdx;
//out int v_EntityID;
//		
//uniform mat4 u_ViewProjMtx;
//
//void main(){
//    v_MaterialIdx = a_MaterialIdx;
//    v_EntityID = a_Entity;
//
//	vec4 worldPosition = a_ModelMatrix * vec4(a_Position, 1.0);
//
//	v_TexCoord = a_TexCoord;
//	gl_Position = u_ViewProjMtx * worldPosition;
//
//} 