#version 460 core

layout(location = 0) out vec4 fragColor;

in vec4 v_Color;
uniform sampler2D u_DepthTex;  // Depth texture
uniform vec2 u_ScreenSize;     // Screen resolution (to normalize gl_FragCoord.xy)
uniform float u_FarPlane;      // Far plane of the depth buffer

void main(){

    // Normalize screen coordinates
    vec2 screenUV = gl_FragCoord.xy / u_ScreenSize;

    // Sample the scene depth from the depth texture
    float sceneDepth = texture(u_DepthTex, screenUV).r; // Scene depth (normalized [0, 1])
    float sceneLinearDepth = sceneDepth * u_FarPlane;  // Reconstruct linear depth

    // Get the line's own depth from gl_FragCoord.z (normalized [0, 1])
    float lineDepth = gl_FragCoord.z;                // Depth in normalized device coordinates
    float lineLinearDepth = lineDepth * u_FarPlane;  // Reconstruct linear depth

    // Compare line depth to scene depth
    float alpha = 1.0; // Default fully opaque
    if (lineLinearDepth > sceneLinearDepth + 0.001) { // Line is behind the scene geometry
        alpha = 0.2; // Fade out
    }

    // Final line color with computed alpha
    fragColor = vec4(v_Color.rgb, v_Color.a * alpha);
	//fragColor = v_Color;
}