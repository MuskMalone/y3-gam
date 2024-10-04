#version 460 core

layout(location = 0) in vec3 aPos;  // Only the vertex position is needed
layout(location = 1) in mat4 instanceModelMatrix;  // Instance transformation matrix (if using instancing)

uniform mat4 u_ViewProjMtx;  // View-Projection matrix for camera transformation

void main() {
    // Transform vertex position using the model matrix and view-projection matrix
    gl_Position = u_ViewProjMtx * instanceModelMatrix * vec4(aPos, 1.0);
}
