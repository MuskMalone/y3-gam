#version 460 core
//#extension GL_ARB_bindless_texture : require

layout(location = 0) out vec4 fragColor;
layout(location = 1) out int entityID;

in vec4 v_Color;
in vec2 v_TexCoord;
in flat float v_TexIdx; //not being used might delete

in flat int v_EntityID;
           
in vec3 v_FragPos;              // Fragment position in world space
in vec3 v_Normal;               // Normal in world space
in vec3 v_Tangent;              // Tangent in world space
in vec3 v_Bitangent;            // Bitangent in world space

//PBR parameters
uniform vec3 u_Albedo;
uniform float u_Metalness;
uniform float u_Roughness;
uniform float u_Transparency;
uniform float u_AO;

in flat int v_MaterialIdx;

uniform sampler2D[16] u_AlbedoMaps;
uniform sampler2D[16] u_NormalMaps;

//lighting parameters
uniform vec3 u_CamPos;       // Camera position in world space
// Single light source (hardcoded for now)
const vec3 u_LightPos = vec3(5.0, 2.0, 5.0); // Example light position
const vec3 u_LightColor = vec3(10.0, 10.0, 10.0);       // Example white light
//uniform vec3 u_LightPos;     // Light position in world space
//uniform vec3 u_LightColor;   // Light color


void main(){
    entityID = v_EntityID;
    
	vec4 texColor = texture2D(u_NormalMaps[int(v_MaterialIdx)], v_TexCoord); //currently unused
    u_Albedo; //currently unused
    
    vec4 albedoTexture = texture2D(u_AlbedoMaps[int(v_MaterialIdx)], v_TexCoord);
    vec3 albedo = albedoTexture.rgb; // Mixing texture and uniform

	// Normalize inputs
    vec3 N = normalize(v_Normal);
    vec3 V = normalize(u_CamPos - v_FragPos);    // View direction
    vec3 L = normalize(u_LightPos - v_FragPos);  // Light direction
    vec3 H = normalize(V + L);                   // Halfway vector

    vec3 color = albedo;

    //change transparency here
    float alpha = u_Transparency;
	fragColor = vec4(color, alpha) * v_Color;

}