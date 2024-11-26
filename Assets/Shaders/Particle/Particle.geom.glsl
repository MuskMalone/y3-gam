
#version 450 core
// #extension GL_ARB_bindless_texture : enable

#include "..\\Assets\\Shaders\\Particle\\Common.glsl"

layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

// struct TextureData{
//     vec2 texCoords[4];
//     uvec2 texHdl;
// };

// layout(std430, binding = 9) buffer TextureHandles 
// { TextureData TexHdls[]; }; 

in mat4 vertTransform[];
in vec4 vertFragColor[];
in uint texIdx[];

out vec4 geomColor;
out vec2 geomTexCoord;
flat out uvec2 geomTexHdl;

void EmitVertexCube(vec4 position, vec3 offset, vec2 texCoord, uvec2 texHdl) {
    gl_Position = vertTransform[0] * (position + vec4(offset, 0.0));
    geomTexCoord = texCoord;
    geomTexHdl = texHdl;
    EmitVertex();
}

void main() {
    vec4 position = vec4(0,0,0,1);//gl_in[0].gl_Position;  // The position of the point
    geomColor = vertFragColor[0];  // Pass the color

    float size = 0.5;  // Half-size of the cube, since it's 1x1x1

// only front face needed
    EmitVertexCube(position, vec3(-size, -size, 0), vec2(0), 0); //bl
    EmitVertexCube(position, vec3(size, -size, 0),  vec2(0), 0); //br
    EmitVertexCube(position, vec3(-size, size, 0),  vec2(0), 0); //tl
    EmitVertexCube(position, vec3(size, size, 0),  vec2(0), 0); //tr
    EndPrimitive();

}