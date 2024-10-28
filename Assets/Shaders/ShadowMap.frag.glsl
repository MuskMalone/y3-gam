#version 460 core

uniform float near;
uniform float far;

out vec4 fragColor;

float LinearizeDepth(float depth)  {
  float z = depth * 2.0 - 1.0;
  return (2.0 * near * far) / (far + near - z * (far - near));	
}

// we dont have to do anything here since opengl will handle the depth mapping for us
void main()
{
  float depth = LinearizeDepth(gl_FragCoord.z) / 50.0;
  fragColor = vec4(vec3(depth), 1.0);
}
