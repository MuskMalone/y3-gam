#version 460 core

layout(location = 0) out vec4 fragColor;

in vec4 v_Color;
in vec2 v_TexCoord;
in flat float v_TexIdx;

//uniform sampler2D u_Tex[32]; //TODO CHANGE THIS IN FUTURE
           
in vec3 v_FragPos;              // Fragment position in world space
in vec3 v_Normal;               // Normal in world space
in vec3 v_Tangent;              // Tangent in world space
in vec3 v_Bitangent;            // Bitangent in world space

// PBR parameters (hardcoded for now)
uniform vec3 u_Albedo;
uniform float u_Metalness;
uniform float u_Roughness;
uniform float u_AO;

uniform sampler2D u_AlbedoMap;

//lighting parameters
uniform vec3 u_CamPos;       // Camera position in world space
// Single light source (hardcoded for now)
const vec3 u_LightPos = vec3(5.0, 2.0, 5.0); // Example light position
const vec3 u_LightColor = vec3(1.0, 1.0, 1.0);       // Example white light
//uniform vec3 u_LightPos;     // Light position in world space
//uniform vec3 u_LightColor;   // Light color

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

void main(){

	//sample texture
	//vec4 texColor = texture2D(u_Tex[0], v_TexCoord);
    vec4 albedoTexture = texture(u_AlbedoMap, v_TexCoord);
    vec3 albedo = albedoTexture.rgb * u_Albedo; // Mixing texture and uniform

	// Normalize inputs
    vec3 N = normalize(v_Normal);
    vec3 V = normalize(u_CamPos - v_FragPos);    // View direction
    vec3 L = normalize(u_LightPos - v_FragPos);  // Light direction
    vec3 H = normalize(V + L);                   // Halfway vector

	// Calculate radiance
    vec3 lightColor = u_LightColor;
    float distance = length(u_LightPos - v_FragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = lightColor * attenuation;

    vec3 F0 = vec3(0.04); 
         F0 = mix(F0, albedo, u_Metalness);

    // cook-torrance brdf
    float NDF = DistributionGGX(N, H, u_Roughness);        
    float G   = GeometrySmith(N, V, L, u_Roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0); 

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - u_Metalness;	

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;  
            
    vec3 Lo = vec3(0.0);
    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);                
    Lo += (kD * albedo / PI + specular) * radiance * NdotL; 

    vec3 ambient = vec3(0.01) * albedo * u_AO;
    vec3 color = ambient + Lo;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); //gamma correction

    //change transparency here
	fragColor = vec4(color, 1.f) * v_Color;
    
    //fragColor = vec4(vec3(max(dot(H, V), 0.0)), 1.0);

    //fragColor = vec4(albedo, texColor.a) * v_Color;

}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}
