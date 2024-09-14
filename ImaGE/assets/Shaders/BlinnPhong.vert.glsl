#version 460 core

struct Light
{
  vec3 position;
  vec3 ambientIntensity, diffuseIntensity, specularIntensity;
};

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 uMdlTransform;
uniform mat4 uViewTransform;
uniform mat4 uProjTransform;

uniform Light light;

out vec3 vViewDir;
out vec3 vLightDir;
out vec3 vPosition;
out vec3 vNormal;

void main()
{
  mat4 MV = uViewTransform * uMdlTransform;

  // normal transform mtx
  mat3 N = mat3(vec3(MV[0]), vec3(MV[1]), vec3(MV[2]));
  vNormal = normalize(N * aNormal);

  vec4 vertexPositionInView = MV * vec4(aPos, 1.f);
  vec3 lightPositionInView = vec3(uViewTransform * vec4(light.position, 1.0f));

  vLightDir = lightPositionInView - vec3(vertexPositionInView);
  vViewDir = normalize(-vec3(vertexPositionInView));
  vPosition = vertexPositionInView.xyz;
  gl_Position = uProjTransform * vertexPositionInView;
}
