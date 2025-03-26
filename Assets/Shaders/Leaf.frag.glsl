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
//uniform sampler2D[16] u_NormalMaps;

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
uniform float u_Dist; // Default value should be set in the application
uniform float u_LeafSize; // Default value should be set in the application
uniform vec3 u_TreePos;
uniform float u_MaxRot ; // 60 degrees in radians

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
float CheckShadow(vec4 lightSpacePos);
float geomSmith(float dp, float Roughness);


//const vec3 tSpace = vec3(1.2, 1.2, 1.2);

uniform float u_Time;
float dLeaf(vec3 p);
float rayMarch(vec3 rayOrigin, vec3 rayDirection);
vec4 GetAlbedoColor(vec3 FragPos, vec3 ViewPosition); 



void main(){
    entityID = v_EntityID;
    // //pls add this line for subsequent custom shaders
    viewPosition = vec4(v_ViewPosition, 1);
    bool hasRenderDir = false;
	//vec4 texColor = texture2D(u_NormalMaps[int(v_MaterialIdx)], texCoord); //currently unused
    MaterialProperties mat = materials[v_MaterialIdx];
    vec4 albedoTexture = GetAlbedoColor(v_FragPos,v_ViewPosition);
   // vec3 albedo = albedoTexture.rgb * mat.AlbedoColor.rgb; // Mixing texture and uniform

    // Check for transparency and discard the fragment
     if (albedoTexture.a < 0.01) {
        discard;
    }

   vec3 albedo = albedoTexture.xyz;

   
   

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

    fragColor = vec4(pow(TotalLight, vec3(1.0 / u_Gamma)), mat.Transparency);
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

float GetVeins(vec2 localPos, float veinScale) {
    // Create veins using sine waves
    float veins = sin(localPos.x * veinScale) * 0.2 + sin(localPos.y * veinScale * 2.0) * 0.1;
    return smoothstep(0.3, 0.5, abs(veins)); // Threshold to create vein-like patterns
}

float hash(float seed) {
    seed = fract(sin(seed) * 43758.5453);
    return fract(sin(seed) * 43758.5453);

}


float Signhash(float seed) {
    return fract(sin(seed) * 43758.5453123);
}


// Remap a value from [0, 1] to [-1, 1]
float remap(float value) {
    return (value * 2.0 - 1.0);
}


vec3 GetSway( float phaseShift) {
    float swayAmount = 0.4; // Adjust this value to control the swaying intensity
    float swayFrequency = 2.0; // Adjust this value to control the swaying speed
    float waveSpeed = 1.0; // Adjust this value to control the wave speed

    // Calculate the sway offset with a phase shift
    float swayTime = u_Time * waveSpeed + phaseShift * 6.28318530718; // Add phase shift
    return vec3(
        sin(swayTime ) * swayAmount,
        sin(swayTime ) * swayAmount,
        sin(swayTime) * swayAmount
    );
}

bool IsInLeaf(vec3 fragPos, vec3 center, float radius, float seedX, float seedY) {
    // Transform the fragment position to local leaf space


    vec3 localPos = fragPos - center;

    // Calculate the rotation angles based on random values
    float angleX = pow(seedX , seedY) * 6.28318530718; // Range [0, 2π)
    float angleY = pow(seedY , seedX) * 6.28318530718; // Range [0, 2π)

    // Create the 3D rotation matrices
    mat3 rotationX = mat3(
        1.0, 0.0, 0.0,
        0.0, cos(angleX), -sin(angleX),
        0.0, sin(angleX), cos(angleX)
    );
    mat3 rotationY = mat3(
        cos(angleY), 0.0, sin(angleY),
        0.0, 1.0, 0.0,
        -sin(angleY), 0.0, cos(angleY)
    );

    // Apply the rotation to the local position
    localPos = rotationX * rotationY * localPos;

    // Randomize the ellipsoid's dimensions using seedX and seedY
    float ellipsoidWidth = radius * (0.4 + 0.4 * seedX); // Random width
    float ellipsoidHeight = radius * (1.0 + 0.4 * seedY); // Random height
    float ellipsoidDepth = radius * (0.8 + 0.4 * hash(seedX + seedY)); // Random depth

    // Define the leaf shape using an ellipsoid with randomized dimensions
    float ellipsoid = (localPos.x * localPos.x) / (ellipsoidWidth * ellipsoidWidth) + 
                      (localPos.y * localPos.y) / (ellipsoidHeight * ellipsoidHeight) + 
                      (localPos.z * localPos.z) / (ellipsoidDepth * ellipsoidDepth);

    // Add some noise or asymmetry to make it look more like a leaf
    float noise = sin(localPos.x * 10.0 + localPos.y * 10.0) * 0.1; // Add wavy edges
    ellipsoid += noise;

    // Check if the point is inside the leaf shape
    return ellipsoid < 1.0;
}
vec4 leafColors[3] = vec4[](
    vec4(0.165, 0.82,0, 1.0),  // rgba(73, 84, 85, 255)
    vec4(0.129, 0.51, 0.227, 1.0), // rgba(93, 125, 101, 255)
    vec4(0.271, 0.639, 0.157, 1.0)//, // rgba(165, 221, 122, 255)
    //vec4(0.576, 0.839, 0.184, 1.0) // rgba(195, 242, 126, 255)
    // vec4(0.839, 1, 0.31, 1.0) // rgba(224, 250, 141, 255)
    
);



vec4 GetAlbedoColor(vec3 FragPos, vec3 ViewPosition) {
    // Use 3D coordinates
    vec3 globalCoord = FragPos;

    float minX = -25.0; // Leftmost x position of the tree
    float maxX = 20.0;  // Rightmost x position of the tree
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    float pcfDepth = texture(u_ShadowMap, FragPos.xy * texelSize).r;

    // Tile the leaves in 3D space
    vec3 tileCoord = trunc(globalCoord / u_Dist);
    for (int i = -3; i <= 3; ++i) {
        for (int j = -3; j <= 3; ++j) {
            for (int k = -3; k <= 3; ++k) {
                vec3 seed = tileCoord + vec3(i, j, k);
                vec3 offset = vec3(i, j, k) * u_Dist;  // Offset to get to the center of the current Quad
                float randomValueX = hash(seed.x + seed.y * 100.0 + seed.z * 10000.0); // Random value for this quad
                float randomValueY = hash(seed.y + seed.z * 100.0 + seed.x * 10000.0); // Random value for this quad
                float randomValueZ = hash(seed.z + seed.x * 100.0 + seed.y * 10000.0); // Random value for this quad
                float sizeChange = hash(seed.x + seed.y) * u_LeafSize/2.0;

                // Fixed leaf size
                float radius = u_LeafSize;
                //radius += sizeChange;
                // Calculate the leaf center
                vec3 center = tileCoord * u_Dist;

                if (tileCoord.x < 0)
                    center.x -= u_Dist / 2.0;
                else
                    center.x += u_Dist / 2.0;

                if (tileCoord.y < 0)
                    center.y -= u_Dist / 2.0;
                else
                    center.y += u_Dist / 2.0;

                if (tileCoord.z < 0)
                    center.z -= u_Dist / 2.0;
                else
                    center.z += u_Dist / 2.0;

                // Calculate the phase shift based on the leaf's x position
                float phaseShift = (center.x - minX) / (maxX - minX);

                // Calculate the sway offset with the phase shift
                vec3 swayOffset = GetSway(phaseShift);
                
                // Apply the sway offset and tile offset to the leaf center
                vec3 leafCenter = center  + offset ;
                leafCenter.x += swayOffset.x + remap(hash(randomValueX * randomValueY)) * u_LeafSize/2.0;

                if (IsInLeaf(globalCoord, leafCenter, radius, randomValueX, randomValueY)) {
                    int colorIndex = int(fract(hash(seed.x * 5 + seed.y * 10 + seed.z * 100)) * 2.0);
                    //int colorIndex = int((remap(hash(sizeChange)) +1.0)/2.0* 3.0);
                    if (colorIndex > 2)
                        colorIndex = 2;
                    return leafColors[colorIndex];
                }
            }
        }
    }

    return vec4(0.0); // Background color (black)
}