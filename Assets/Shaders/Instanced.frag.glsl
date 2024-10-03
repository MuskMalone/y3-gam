#version 460 core

layout(location = 0) out vec4 fragColor;

in vec4 v_Color;
in vec2 v_TexCoord;
in flat float v_TexIdx;

uniform sampler2D u_Tex[32]; // Array of textures

in vec3 v_FragPos;              // Fragment position in world space
in vec3 v_Normal;               // Normal in world space
in vec3 v_Tangent;              // Tangent in world space
in vec3 v_Bitangent;            // Bitangent in world space

// PBR parameters (hardcoded for now)
uniform vec3 u_Albedo;
uniform float u_Metallic;
uniform float u_Roughness;
uniform float u_AO;

//lighting parameters
uniform vec3 u_CamPos;       // Camera position in world space
const vec3 u_LightPos = vec3(5.0, 2.0, 5.0); // Example light position
const vec3 u_LightColor = vec3(10.0, 10.0, 10.0); // Example light color

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

void main(){
    // Sample texture using the provided texture index
    vec4 texColor = texture(u_Tex[int(v_TexIdx)], v_TexCoord);
    vec3 albedo = texColor.rgb * u_Albedo;

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
    F0 = mix(F0, u_Albedo, u_Metallic);

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, u_Roughness);        
    float G   = GeometrySmith(N, V, L, u_Roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0); 

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - u_Metallic;	

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;  
            
    vec3 Lo = vec3(0.0);
    float NdotL = max(dot(N, L), 0.0);                
    Lo += (kD * albedo / PI + specular) * radiance * NdotL; 

    vec3 ambient = vec3(0.03) * u_Albedo * u_AO;
    vec3 color = ambient + Lo;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); // Gamma correction

    // Apply per-instance color and output final color
    fragColor = v_Color;
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