#version 460 core

layout(location = 0) out vec4 fragColor;
//layout(location = 1) out int entity; 


in vec4 v_Color;
in vec2 v_TexCoord;
in flat float v_TexIdx;
//in flat int v_Entity;

uniform sampler2D u_Tex[32]; //TODO CHANGE THIS IN FUTURE

void main(){
	vec4 texColor = texture2D(u_Tex[int(v_TexIdx)], v_TexCoord);
	fragColor = texColor * v_Color;
    //entity = v_Entity;
}

//layout(location = 0) out vec4 fragColor;
//layout(location = 1) out int entityID;
//
//in vec4 v_Color;
//in vec2 v_TexCoord;
//in flat int v_EntityID;
//
//uniform sampler2D u_Tex[32]; //TODO CHANGE THIS IN FUTURE
//
//void main(){
//    entityID = v_EntityID;
//	//vec4 texColor = texture2D(u_Tex[int(v_TexIdx)], v_TexCoord);
////	vec4 texColor = texture2D(u_Tex[0], v_TexCoord);
////	fragColor = texColor * v_Color;
//
//	//sample texture
//	vec4 texColor = texture2D(u_Tex[0], v_TexCoord);
//
//	fragColor = vec4(1.f);
//
//}