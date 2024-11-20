#version 460 core

in vec2 fragTexCoord; // Received texture coordinate from vertex shader
in vec3 fragDir;

out vec4 fragColor;

// Uniforms to control the sky and ground appearance
uniform vec3 u_SkyColor = vec3(0.5, 0.7, 1.0);  // Sky color (light blue)
uniform vec3 u_HorizonColor = vec3(1.0, 1.0, 1.0);  // Horizon color (white or bright blue)
uniform vec3 u_GroundColor = vec3(0.3, 0.3, 0.3);  // Ground color (gray)
uniform float u_HorizonSharpness = 5.0;  // Controls the sharpness of the horizon blend
uniform vec3 u_AtmosphereColor = vec3(1.f,1.f ,1.0f);
uniform float u_AtmosphereThickness = 1.f;
void main() {
    //fragColor = texture(u_Texture, fragTexCoord);
    // Normalize the direction vector
    vec3 dir = normalize(fragDir);


    // Blend factors for transitions
    float groundToAtmosphereBlend = smoothstep(-0.05, 0.0, dir.y); // Ground to atmosphere transition
    float atmosphereToSkyBlend = smoothstep(0.0, 0.2 * u_AtmosphereThickness, dir.y);      // Atmosphere to sky transition

    // Interpolate between ground and atmosphere colors
    vec3 groundAtmosphereMix = mix(u_GroundColor, u_AtmosphereColor, groundToAtmosphereBlend);

    // Interpolate between atmosphere and sky colors
    vec3 finalColor = mix(groundAtmosphereMix, u_SkyColor, atmosphereToSkyBlend);

    // Output the color
    fragColor = vec4(finalColor, 1.0);

}