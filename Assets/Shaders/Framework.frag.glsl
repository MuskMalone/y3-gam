#version 460 core

out vec4 FragColor;

uniform vec4 uVtxClr;

void main()
{
  FragColor = uVtxClr;
}