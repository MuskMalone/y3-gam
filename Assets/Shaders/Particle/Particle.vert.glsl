
#version 450 core
// #extension GL_ARB_bindless_texture : enable

#include "..\\Assets\\Shaders\\Particle\\Common.glsl"

struct TextureData{
    vec2 texCoords[4];
    uvec2 texHdl;
};

// layout(std430, binding = 9) buffer TextureHandles 
// { TextureData TexHdls[]; }; 

uniform mat4 vertViewProjection;
out mat4 vertTransform;
out vec4 vertFragColor;
out uint texIdx;

mat4 rotate(float angle) {
    float c = cos(angle);
    float s = sin(angle);

    return mat4(
        c, s, 0.0, 0.0,
        -s, c, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}
mat4 translate(vec4 pos) {
    return mat4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(vec3(pos), 1.0)
    );
}
mat4 scale(vec2 scale) {
    return mat4(
        scale.x, 0.0, 0.0, 0.0,
        0.0, scale.y, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

void main() {
    if (!Particles[gl_VertexID].alive) {
        vertTransform = mat4(0.0);
        texIdx = 0;
    } else {
        vertTransform = vertViewProjection * translate(Particles[gl_VertexID].pos) * rotate(Particles[gl_VertexID].rot) * scale(Particles[gl_VertexID].size);
        vertFragColor = Particles[gl_VertexID].col;
        texIdx = Particles[gl_VertexID].emtIdx;
    }
}