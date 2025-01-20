#version 460 core
//#extension GL_ARB_bindless_texture : require

struct MaterialProperties {
    vec2 Tiling;
    vec2 Offset;
    vec4 AlbedoColor;  // Base color
    float Metalness;   // Metalness factor
    float Roughness;   // Roughness factor
    float Transparency; // Transparency (alpha)
    float AO;          // Ambient occlusion
    float Emission;
    float padding[3];
};


layout(std430, binding = 0) buffer MaterialPropsBuffer {
    MaterialProperties materials[];
};


layout(location = 0) out vec4 fragColor;
layout(location = 1) out int entityID;

in vec4 v_Color;
in vec2 v_TexCoord;
in flat float v_TexIdx; //not being used might delete

in flat int v_EntityID;
in flat int v_MaterialIdx;
           
in vec3 v_FragPos;              // Fragment position in world space
in vec3 v_Normal;               // Normal in world space
in vec3 v_Tangent;              // Tangent in world space
in vec3 v_Bitangent;            // Bitangent in world space

// shadows
in vec4 v_LightSpaceFragPos;
uniform bool u_ShadowsActive;
uniform float u_ShadowBias;
uniform int u_ShadowSoftness;
uniform sampler2D u_ShadowMap;

uniform int u_MatIdxOffset;
uniform sampler2D[16] u_AlbedoMaps;
//uniform sampler2D[16] u_NormalMaps;

//lighting parameters
const int typeDir = 0;
const int typeSpot = 1;
const int typePoint = 2;
const int maxLights = 30;
uniform vec3 u_CamPos;       // Camera position in world space
uniform int numlights;
uniform vec3 u_AmbientLight; 

uniform int u_type[maxLights];       // Camera position in world space

uniform vec3 u_LightDirection[maxLights]; // Directional light direction in world space
uniform vec3 u_LightColor[maxLights];     // Directional light color

//For spotlight
uniform  vec3 u_LightPos[maxLights]; // Position of the spotlight
uniform  float u_InnerSpotAngle[maxLights]; // Inner spot angle in degrees
uniform  float u_OuterSpotAngle[maxLights]; // Outer spot angle in degrees
uniform  float u_LightIntensity[maxLights]; // Intensity of the light
uniform  float u_Range[maxLights]; // Maximum range of the spotlight


const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
float CheckShadow(vec4 lightSpacePos);

void main(){
    entityID = v_EntityID;
    
	//vec4 texColor = texture2D(u_NormalMaps[int(v_MaterialIdx)], texCoord); //currently unused
    MaterialProperties mat = materials[v_MaterialIdx];
    vec2 texCoord = v_TexCoord * mat.Tiling + mat.Offset;
    vec4 albedoTexture = texture2D(u_AlbedoMaps[int(v_MaterialIdx) + u_MatIdxOffset], texCoord);
    vec3 albedo = albedoTexture.rgb * mat.AlbedoColor.rgb; // Mixing texture and uniform
	// Normalize inputs
    vec3 N = normalize(v_Normal);
    vec3 Lo = vec3(0); 

    for (int i = 0; i < numlights; ++i) {
        vec3 V = normalize(u_CamPos - v_FragPos);    // View direction
        vec3 L = vec3(0);
        vec3 lightColor = vec3(0); 
        float shadow = 0.0; // Shadow factor default (0.0 = no shadow)

        if(u_type[i] == typeDir)
        {
            L = normalize(-u_LightDirection[i]); // Light direction (directional light)
            lightColor = u_LightColor[i] * u_LightIntensity[i];
            if (u_ShadowsActive) {
                shadow = CheckShadow(v_LightSpaceFragPos);  // 1.0 if in shadow and 0.0 otherwise
            }
        }
        if(u_type[i] == typeSpot)
        {
                // Spotlight setup
            L = normalize(u_LightPos[i] - v_FragPos);  // Vector from spotlight to fragment
            float distance = length(u_LightPos[i] - v_FragPos);

            // Use u_LightDirection for spotlight effect calculation
            vec3 spotDir = normalize(u_LightDirection[i]);
            float spotCosAngle = dot(spotDir, -L);  // Angle between spotlight direction and L

            // Convert inner and outer angles from degrees to cosine for comparison
            float innerAngleCos = cos(radians(u_InnerSpotAngle[i] * 0.5));
            float outerAngleCos = cos(radians(u_OuterSpotAngle[i] * 0.5));

            // Calculate spotlight intensity effect
            float spotEffect = smoothstep(outerAngleCos, innerAngleCos, spotCosAngle);

            // Calculate distance attenuation
            float attenuation = smoothstep(0.0, u_Range[i], u_Range[i] - distance) * spotEffect;

            // Final light color for spotlight
            lightColor = u_LightColor[i] * u_LightIntensity[i] * attenuation;
        }
         if(u_type[i] == typePoint)
        {
            L = normalize(u_LightPos[i] - v_FragPos);  // Direction from fragment to light
            float distance = length(u_LightPos[i] - v_FragPos);  // Distance to light

            // Range attenuation based on inverse square law (simplified with smoothstep)
            float attenuation = smoothstep(0.0, u_Range[i], u_Range[i] - distance);
            lightColor = u_LightColor[i] * u_LightIntensity[i] * attenuation;

            // if (u_ShadowsActive) {
            //     shadow = CheckShadow(v_LightSpaceFragPos);  // Shadows for point light
            // }

            

        }

        vec3 H = normalize(V + L);                   // Halfway vector

        vec3 F0 = vec3(0.04); 
            F0 = mix(F0, albedo, mat.Metalness);

        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, mat.Roughness);        
        float G   = GeometrySmith(N, V, L, mat.Roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0); 

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - mat.Metalness;	

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular     = numerator / denominator;  
                
        float NdotL = max(dot(N, L), 0.0);

        Lo += (kD * albedo / PI + specular) * lightColor * NdotL * (1.0 - shadow);
    }

    vec3 ambient =  u_AmbientLight * albedo * mat.AO;

    vec3 emission = albedo * mat.Emission; // Uniform emission
    vec3 color = ambient + Lo + emission;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); //gamma correction
    //change transparency here
    float alpha = mat.Transparency;
	fragColor = vec4(color, alpha) * v_Color;
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

float SimplePCF(vec3 projCoords) {
    float shadow = 0;
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    int range = u_ShadowSoftness;

    for(int x = -range; x <= range; ++x)
    {
        for(int y = -range; y <= range; ++y)
        {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            // if current depth more than what is in the shadow map,
            // it means that it is in shadow
            shadow += projCoords.z - u_ShadowBias > pcfDepth ? 1.0 : 0.0;        
        }    
    }

    int sampleSize = range * 2 + 1;
    return shadow /= float(sampleSize * sampleSize);
}

float CheckShadow(vec4 lightSpacePos) {
    // perform perspective division and map to [0,1]
    vec3 projCoords = vec3(lightSpacePos / lightSpacePos.w) * 0.5 + 0.5;
    projCoords.xy = clamp(projCoords.xy, 0.0, 1.0);

    if (u_ShadowSoftness == 0) {
        float closestDepth = texture(u_ShadowMap, projCoords.xy).r;

        // if current depth more than what is in the shadow map,
        // it means that it is in shadow
        return projCoords.z - u_ShadowBias > closestDepth ? 1.0 : 0.0;
    }

    return SimplePCF(projCoords);
}