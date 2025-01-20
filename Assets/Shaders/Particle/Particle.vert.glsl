
#version 460 core
// #extension GL_ARB_bindless_texture : enable

#include "..\\Assets\\Shaders\\Particle\\Common.glsl"
#include "..\\Assets\\Shaders\\Particle\\ParticleUniforms.glsl"
// struct TextureData{
//     vec2 texCoords[4];
//     uvec2 texHdl;
// };

// layout(std430, binding = 9) buffer TextureHandles 
// { TextureData TexHdls[]; }; 

// out mat4 vertTransform;

out mat4 T;
out mat4 R;
out mat4 S; 

out vec4 vertFragColor;
out uint texIdx;

out int particleIdx;

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
mat4 scale(vec2 scale) {
    return mat4(
        scale.x, 0.0, 0.0, 0.0,
        0.0, scale.y, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}
// Function to compute the billboard alignment
mat4 computeBillboardAlignment(mat4 viewMatrix) {
    // Extract camera rotation (upper 3x3) and invert it to align the quad to the camera
    mat3 cameraRotation = mat3(viewMatrix);
    cameraRotation[0] = normalize(cameraRotation[0]);
    cameraRotation[1] = normalize(cameraRotation[1]);
    cameraRotation[2] = normalize(cameraRotation[2]);

    // Convert 3x3 rotation matrix to 4x4
    return mat4(
        vec4(cameraRotation[0], 0.0),
        vec4(cameraRotation[1], 0.0),
        vec4(cameraRotation[2], 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );
}

void main() {
    particleIdx = gl_VertexID;
    if (!Particles[gl_VertexID].alive) {
        T = mat4(0.0);
        R = mat4(0.0);
        S = mat4(0.0);        
        texIdx = 0;
    } else {

        // Billboard alignment
        mat4 billboardAlignment = computeBillboardAlignment(vertView);
        T = translate(Particles[gl_VertexID].pos);
        R = rotate(vec3(0));
        S = scale(Particles[gl_VertexID].size);
        // vertTransform = vertViewProjection * T * S;
        vertFragColor = Particles[gl_VertexID].col;
        texIdx = Particles[gl_VertexID].emtIdx;
    }

}