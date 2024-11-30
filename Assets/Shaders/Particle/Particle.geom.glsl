#version 460 core
#include "..\\Assets\\Shaders\\Particle\\Common.glsl"

layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

in mat4 vertTransform[];

out mat4 translate[];
out mat4 rotate[];
out mat4 scale[]; 

in vec4 vertFragColor[];
in uint texIdx[];

out vec4 geomColor;
out vec2 geomTexCoord;

// Helper function to remove rotation from a matrix
mat4 extractTranslationAndScale(mat4 transform) {
    return mat4(
        vec4(length(transform[0].xyz), 0.0, 0.0, 0.0), // Scale X
        vec4(0.0, length(transform[1].xyz), 0.0, 0.0), // Scale Y
        vec4(0.0, 0.0, length(transform[2].xyz), 0.0), // Scale Z (optional for billboards)
        transform[3]                                   // Translation
    );
}

void EmitVertexBillboard(vec4 position, vec3 offset, vec2 texCoord, mat4 transform) {
    gl_Position = transform * (position + vec4(offset, 0.0));
    geomTexCoord = texCoord;
    EmitVertex();
}

void main() {
    vec4 position = vec4(0, 0, 0, 1); // Center of the quad
    geomColor = vertFragColor[0];     // Pass the particle color

    float size = 0.5;                 // Half-size of the quad

    // Remove rotation from the transformation matrix
    mat4 billboardTransform = extractTranslationAndScale(vertTransform[0]);

    // Emit vertices for a single quad (billboarded)
    EmitVertexBillboard(position, vec3(-size, -size, 0), vec2(0, 0), billboardTransform); // Bottom-left
    EmitVertexBillboard(position, vec3(size, -size, 0), vec2(1, 0), billboardTransform);  // Bottom-right
    EmitVertexBillboard(position, vec3(-size, size, 0), vec2(0, 1), billboardTransform);  // Top-left
    EmitVertexBillboard(position, vec3(size, size, 0), vec2(1, 1), billboardTransform);   // Top-right
    EndPrimitive();
}
