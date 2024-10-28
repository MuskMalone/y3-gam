#version 460 core

layout(location = 0) in vec3 a_Position;

// New per-instance attributes (starting at location 7)
layout(location = 7) in mat4 a_ModelMatrix; // Model transformation matrix for each instance
layout(location = 11) in int a_MaterialIdx; 
layout(location = 12) in int a_Entity;

uniform mat4 u_LightProjMtx;
uniform mat4 u_ViewProjMtx;

void main()
{
  // transform the vertex to the light's perspective
  gl_Position = u_LightProjMtx * a_ModelMatrix * vec4(a_Position, 1.0);
  //gl_Position = u_ViewProjMtx * vec4(1.0);
}
