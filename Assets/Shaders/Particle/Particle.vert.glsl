
#version 450 core
// #extension GL_ARB_bindless_texture : enable

#include "..\\Assets\\Shaders\\Particle\\Common.glsl"

// struct TextureData{
//     vec2 texCoords[4];
//     uvec2 texHdl;
// };

// layout(std430, binding = 9) buffer TextureHandles 
// { TextureData TexHdls[]; }; 

uniform mat4 vertViewProjection;
out mat4 vertTransform;
out vec4 vertFragColor;
out uint texIdx;

mat4 rotate(vec3 eulerAngles) {
    float cx = cos(eulerAngles.x); // cos(pitch)
    float sx = sin(eulerAngles.x); // sin(pitch)

    float cy = cos(eulerAngles.y); // cos(yaw)
    float sy = sin(eulerAngles.y); // sin(yaw)

    float cz = cos(eulerAngles.z); // cos(roll)
    float sz = sin(eulerAngles.z); // sin(roll)

    // Rotation matrix for X-axis (pitch)
    mat4 rotX = mat4(
        1.0, 0.0,  0.0, 0.0,
        0.0,  cx, -sx, 0.0,
        0.0,  sx,  cx, 0.0,
        0.0, 0.0,  0.0, 1.0
    );

    // Rotation matrix for Y-axis (yaw)
    mat4 rotY = mat4(
        cy,  0.0, sy,  0.0,
        0.0, 1.0, 0.0, 0.0,
       -sy, 0.0, cy,  0.0,
        0.0, 0.0, 0.0, 1.0
    );

    // Rotation matrix for Z-axis (roll)
    mat4 rotZ = mat4(
        cz, -sz, 0.0, 0.0,
        sz,  cz, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );

    // Combine rotations in the order Z * Y * X (roll -> yaw -> pitch)
    return rotZ * rotY * rotX;
}
mat4 translate(vec3 pos) {
    return mat4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(pos, 1.0)
    );
}
mat4 scale(vec3 scale) {
    return mat4(
        scale.x, 0.0, 0.0, 0.0,
        0.0, scale.y, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

void main() {
    if (!Particles[gl_VertexID].alive) {
        // vertTransform = mat4(0.0);
        // texIdx = 0;
    } else {
        vertTransform = vertViewProjection * translate(Particles[gl_VertexID].pos) * rotate(Particles[gl_VertexID].rot) * scale(Particles[gl_VertexID].size);
        vertFragColor = Particles[gl_VertexID].col;
        texIdx = Particles[gl_VertexID].emtIdx;
    }

}