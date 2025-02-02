#version 460 core
#include "..\\Assets\\Shaders\\Particle\\Common.glsl"
#include "..\\Assets\\Shaders\\Particle\\ParticleUniforms.glsl"

flat in int geomParticleIdx;
flat in vec4 geomCenterCoord;
in vec4 geomCoord;
in vec4 geomColor;

out vec4 FragColor;

void main() {
    // Convert gl_Position from clip space to NDC
    vec3 ndc = geomCoord.xyz / geomCoord.w;

    // Map NDC depth (z) to screen space (0 to 1) for depth texture comparison
    vec2 screenUV = ndc.xy * 0.5 + 0.5; // Map NDC xy to [0, 1]
    float sampledDepth = texture(depthBuffer, screenUV).r;

    // Perform depth testing
    float fragmentDepth = ndc.z * 0.5 + 0.5;
    if (fragmentDepth > sampledDepth + 0.0001) {
        discard; // Fragment is occluded
    }

    float fadeRadius = min(Particles[geomParticleIdx].size.x, Particles[geomParticleIdx].size.y) * 0.5;
    // Compute the fade factor based on distance from the quad center
    float distanceFromCenter = length(geomCoord.xy - geomCenterCoord.xy);

    float fadeFactor = smoothstep(0.0, fadeRadius, distanceFromCenter);

    // Apply the fade factor to the alpha channel
    vec4 finalColor = geomColor;
    // finalColor.rgb *= finalColor.a; // Pre-multiply RGB by Alpha
    finalColor.a *= (1.0 - fadeFactor); // Fade out as distance increases

    // Output the final color
    FragColor = finalColor;
}
