#version 460 core

struct Material
{
  vec3 ambient, diffuse, specular;
  float shininess;
};

struct Light
{
  vec3 position;
  vec3 ambientIntensity, diffuseIntensity, specularIntensity;
};

out vec4 FragColor;

in vec3 vViewDir;
in vec3 vLightDir;
in vec3 vPosition;
in vec3 vNormal;

uniform vec4 uVtxClr;
uniform Light light;
uniform Material material;

vec3 BlinnPhong(vec3 normal, vec3 color, vec3 lightDir, vec3 viewDir)
{
  if (!any(notEqual(normal, vec3(0.0f, 0.0f, 0.0f))))
  {
    return vec3(0.f);
  }

  // ambient light: amb Intensity * amb reflection coef 
  vec3 ambient = light.ambientIntensity * material.ambient;
    
  float nDotPointLight = dot(normal, lightDir);

  // diffuse light: diffuse intensity * diffuse reflection coef * dot(normal, dir to light)
  vec3 diffuse = light.diffuseIntensity * material.diffuse * max(0.f, nDotPointLight);

  // H: dot(normalize(point to light vec + view vec), normal)
  float nDotH = pow(max(dot(normalize(lightDir + viewDir), normal), 0.f), material.shininess);
  
  // specular light: specular intensity * material specular reflection coef * dot(N, H)
  vec3 specular = light.specularIntensity * material.specular * nDotH;

  // illumination: ambient + diffuse + specular light
  return color * (ambient + diffuse) + specular;
}

void main()
{
  vec3 color = BlinnPhong(vNormal, vec3(uVtxClr), vLightDir, vViewDir);

  // Set with the gamma correction
  FragColor = vec4(pow(color, vec3(1.0f/2.2f)), 1.0f);
}
