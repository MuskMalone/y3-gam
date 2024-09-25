#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 uMdlTransform;
uniform mat4 uViewTransform;
uniform mat4 uProjTransform;

void main()
{
  gl_Position = uProjTransform * uViewTransform * uMdlTransform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
