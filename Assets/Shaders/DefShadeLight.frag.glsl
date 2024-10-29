#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
    int  mType;
    vec3 Position;
    vec3 Direction;
    vec3 Color;
    float Linear;
    float Quadratic;
    float Radius;
    float CutOff;       //Spotlight
    float OuterCutOff;  //Spotlight
};

const int Light_Type_Directional = 0;
const int Light_Type_Spotlight = 1;
const int MAX_LIGHTS = 32;
uniform int numofLights;
uniform Light lights[MAX_LIGHTS];
uniform vec3 viewPos;

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    
    // then calculate lighting as usual
    vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
    for(int i = 0; i < numofLights; ++i)
    {
        // calculate distance between light source and current fragment
        float distance = length(lights[i].Position - FragPos);
        if(distance < lights[i].Radius)
        {
             if(lights[i].mType == Light_Type_Directional)
            {
                vec3 lightDir = normalize(-lights[i].Direction); // Negate for lighting calculations
                vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;
                lighting += diffuse;
     
            }

           
           if(lights[i].mType == Light_Type_Spotlight)
            {
                       
                // diffuse
                vec3 lightDir = normalize(lights[i].Position - FragPos);
                vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;
                // specular
                vec3 halfwayDir = normalize(lightDir + viewDir);  
                float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
                vec3 specular = lights[i].Color * spec * Specular;
                // attenuation
            
            
                float theta = dot(lightDir, normalize(-lights[i].Direction));
                float epsilon = lights[i].CutOff - lights[i].OuterCutOff;
                float intensity = clamp((theta - lights[i].OuterCutOff) / epsilon, 0.0, 1.0);
                diffuse *= intensity;
                specular *= intensity;
                float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
                diffuse *= attenuation;
                specular *= attenuation;
                lighting += diffuse + specular;
            }
            
        }
    }    
    FragColor = vec4(lighting, 1.0);
}