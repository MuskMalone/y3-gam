#version 460 core

uniform float u_Near;
uniform float u_Far;

out vec4 fragColor;

// required for perspective proj visualization
/*float LinearizeDepth(float depth)  {
  float z = depth * 2.0 - 1.0;  // back to NDC 
  return (2.0 * u_Near * u_Far) / (u_Far + u_Near - z * (u_Far - u_Near));	
}*/

// we dont have to do anything here since opengl will handle the depth mapping for us
void main()
{
  // set color to render the shadow map
  fragColor = vec4(vec3(gl_FragCoord.z), 1.0);

  //float depth = LinearizeDepth(gl_FragCoord.z) / 50.0;
  //fragColor = vec4(vec3(depth), 1.0);
}
