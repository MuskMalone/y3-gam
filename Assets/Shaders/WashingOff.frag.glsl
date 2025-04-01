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
    vec4 Emission;
};


layout(std430, binding = 0) buffer MaterialPropsBuffer {
    MaterialProperties materials[];
};


layout(location = 0) out vec4 fragColor;
layout(location = 1) out int entityID;
layout(location = 2) out vec4 viewPosition;
layout(location = 3) out vec4 bloomColor;
layout(location = 4) out vec4 highDefFragColor;

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

in vec3 v_ViewPosition;

in flat vec4 v_BloomProps;
in vec4 testingPos;

uniform bool u_ShadowsActive;
uniform float u_ShadowBias;
uniform int u_ShadowSoftness;
uniform sampler2D u_ShadowMap;

uniform int u_MatIdxOffset;
uniform sampler2D[16] u_AlbedoMaps;
uniform sampler2D[16] u_NormalMaps;

//lighting parameters
const int typeDir = 0;
const int typeSpot = 1;
const int typePoint = 2;
const int maxLights = 100;
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
uniform  float gSpecularPower;

uniform float u_Gamma; // Default value should be set in the application

uniform bool u_IsEditor;
uniform float u_Time;
uniform float u_DeltaTime;
uniform bool u_ShaderActive;
uniform bool u_JustActivated;
uniform sampler2D u_previousSmearBuffer;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
float CheckShadow(vec4 lightSpacePos);
float geomSmith(float dp, float Roughness);
vec4 GetAlbedoColor(sampler2D matTex, vec2 uv);

void main(){
    entityID = v_EntityID;
    // //pls add this line for subsequent custom shaders
    viewPosition = vec4(v_ViewPosition, 1);

    bool hasRenderDir = false;
	//vec4 texColor = texture2D(u_NormalMaps[int(v_MaterialIdx)], texCoord); //currently unused
    MaterialProperties mat = materials[v_MaterialIdx];
    vec2 texCoord = v_TexCoord * mat.Tiling + mat.Offset;
    vec4 albedoTexture;
    if (!u_IsEditor){
        if (!u_JustActivated){
            albedoTexture = GetAlbedoColor(u_previousSmearBuffer, gl_FragCoord.xy); //texture2D(u_AlbedoMaps[int(v_MaterialIdx) + u_MatIdxOffset], texCoord);
        }else{
            albedoTexture = texture2D(u_AlbedoMaps[int(v_MaterialIdx) + u_MatIdxOffset], texCoord);
        }
    }
    else{
        albedoTexture = texture2D(u_AlbedoMaps[int(v_MaterialIdx) + u_MatIdxOffset], texCoord);
    }
    vec3 albedo = albedoTexture.rgb * mat.AlbedoColor.rgb; // Mixing texture and uniform

    // Check for transparency and discard the fragment
    if (albedoTexture.a * mat.AlbedoColor.a < 0.01) {
        discard;
    }

	// Normalize inputs
    vec3 N = normalize(v_Normal);
    vec3 TotalLight = u_AmbientLight * albedo * mat.AO;
    vec3 V = normalize(u_CamPos - v_FragPos);    // View direction

    for (int i = 0; i < numlights; ++i) {

        vec3 LightIntensity = u_LightColor[i] * u_LightIntensity[i];
        vec3 l = vec3(0.0);
        float shadow = 0.0; // Shadow factor default (0.0 = no shadow)

        if(u_type[i] == typeDir) {
            if (hasRenderDir)
                continue;
            hasRenderDir = true;
            l = -u_LightDirection[i].xyz;

            if (u_ShadowsActive) {
                shadow = CheckShadow(v_LightSpaceFragPos);  // 1.0 if in shadow and 0.0 otherwise
            }
        }
        else if(u_type[i] == typePoint)
        {
            l = u_LightPos[i] - v_FragPos;
            float LightToPixelDist = length(l);
            l = normalize(l);
            float attenuation = smoothstep(0.0, u_Range[i], u_Range[i] - LightToPixelDist);
            LightIntensity *= attenuation;
        }
         else if(u_type[i] == typeSpot)
        {
            // Spotlight setup
            l = u_LightPos[i] - v_FragPos;
            float distance = length(l);
            l = normalize(l);

            // Use u_LightDirection for spotlight effect calculation
            vec3 spotDir = normalize(u_LightDirection[i]);
            float spotCosAngle = dot(spotDir, -l);  // Angle between spotlight direction and L

            // Convert inner and outer angles from degrees to cosine for comparison
            float innerAngleCos = cos(radians(u_InnerSpotAngle[i] * 0.5));
            float outerAngleCos = cos(radians(u_OuterSpotAngle[i] * 0.5));

            // Calculate spotlight intensity effect
            float spotEffect = smoothstep(outerAngleCos, innerAngleCos, spotCosAngle);

            // Calculate distance attenuation
            float attenuation = smoothstep(0.0, u_Range[i], u_Range[i] - distance) * spotEffect;

            // Final light color for spotlight
            LightIntensity *= attenuation;
        }

        vec3 n = N;
        vec3 h = normalize(V + l);

         // Replace standard Lambertian NdotL with Half-Lambert
        float NdotL = 0.5 + 0.5 * dot(n, l); // Half-Lambert shading
        float NdotV = max(dot(n, V), 0.0);
        float NdotH = max(dot(n, h), 0.0);
        float HdotV = max(dot(h, V), 0.0);

        vec3 fLambert = pow(albedo, vec3(u_Gamma)); // Gamma correction
        vec3 F0 = vec3(0);

        if (mat.Metalness > 0.0)
            F0 = mix(vec3(0.04), fLambert, mat.Metalness);

        vec3 F = fresnelSchlick(HdotV, F0);
        vec3 kS = F;
        vec3 kD = 1.0 - kS;

        float Roughness = mat.Roughness;

        vec3 SpecBRDF_nom = DistributionGGX(N, h, mat.Roughness) * F * geomSmith(NdotL, mat.Roughness) * geomSmith(NdotV, mat.Roughness);
        float SpecBRDF_denom = 4.0 * NdotV * NdotL + 0.0001;
        vec3 SpecBRDF = SpecBRDF_nom / SpecBRDF_denom;

        // Apply Half-Lambert to the diffuse term
        vec3 DiffuseBRDF = kD * fLambert / PI * NdotL; // NdotL is already Half-Lambert

        vec3 FinalColor = (DiffuseBRDF + SpecBRDF) * LightIntensity;

        TotalLight += FinalColor * (1.0 - shadow);
    }

    // vec3 ambient =  u_AmbientLight * albedo * mat.AO;

    // vec3 emission = albedo * mat.Emission; // Uniform emission
    // vec3 color = ambient + Lo + emission;
    // color = color / (color + vec3(1.0));
    // color = pow(color, vec3(1.0/2.2)); //gamma correction
    // //change transparency here
    // float alpha = mat.Transparency;
    //fragColor = vec4(color, alpha) * v_Color;
    // HDR tone mapping

    vec3 Emission = mat.Emission.xyz * mat.Emission.w;
    TotalLight += Emission;
    TotalLight = TotalLight / (TotalLight + vec3(1.0));

    float luminance = dot(mat.Emission.xyz, vec3(0.2126, 0.7152, 0.0722)); // Standard Rec. 709 weights


    bloomColor = vec4(0,0,0,1);
    if (v_BloomProps.x > 0.1){ // if there is bloom and it is above threshold
        if (luminance >= v_BloomProps.y){
            fragColor = vec4(mat.Emission.xyz, 1);
            bloomColor = vec4(mat.Emission.xyz, v_BloomProps.z);
        }else{
            fragColor = fragColor * vec4(mat.Emission.xyz, 1);
        }
    }

    fragColor = vec4(albedo, albedoTexture.a);//, mat.Transparency);//vec4(pow(TotalLight, vec3(1.0 / u_Gamma)), mat.Transparency);
    highDefFragColor = vec4(albedo, albedoTexture.a);//, mat.Transparency);
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

float geomSmith(float dp, float Roughness)
{
    float k = (Roughness + 1.0) * (Roughness + 1.0) / 8.0;
    float denom = dp * (1 - k) + k;
    return dp / denom;
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

    // force value to 0 if out of bounds
    if(projCoords.z > 1.0) {
        return 0.0;
    }
    //projCoords.xy = clamp(projCoords.xy, 0.0, 1.0);

    if (u_ShadowSoftness == 0) {
        float closestDepth = texture(u_ShadowMap, projCoords.xy).r;

        // if current depth more than what is in the shadow map,
        // it means that it is in shadow
        return projCoords.z - u_ShadowBias > closestDepth ? 1.0 : 0.0;
    }

    return SimplePCF(projCoords);
}

const float pi = 3.1415926;
vec2 hash22(vec2 p) // Dave hoshkin hash
{
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yzx+33.33);
    return fract((p3.xx+p3.yz)*p3.zy);
}
vec2 R;
vec4 B(sampler2D originalTex, vec2 p){
 	return texture(originalTex, p / R);    
}
#define Smear_Factor 0.05
vec4 GetAlbedoColor(sampler2D matTex, vec2 u)
{
    R = vec2(textureSize(matTex, 0));
    vec4 bA = B(matTex, u);
    vec2 d = vec2(0);
    
    vec4 Up = B(matTex, u + vec2(.0, 1.));
    vec4 Down = B(matTex, u + vec2(.0, -1.));
    vec4 Left = B(matTex, u + vec2(-1., .0));
    vec4 Right = B(matTex, u + vec2(1., .0));
    
    d = normalize(Up.yz + Down.yz + Left.yz + Right.yz);
    
    d += (hash22(u - sin((u/R.xy)*1.))*20.);
    
    float angle = abs(atan(d.y, d.x)) - pi;
    
    u.x += 0.25;
    u.y += 0.3;
    
    float s = smoothstep(0.0, 1.0, angle);
    
    vec4 new = B(matTex, u - d);
    
    bA = mix(bA, new, Smear_Factor);
    
    return bA;
}