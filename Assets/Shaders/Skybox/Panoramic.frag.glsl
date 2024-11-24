#version 460 core

in vec2 fragTexCoord; // Received texture coordinate from vertex shader
in vec3 fragDir;

out vec4 fragColor;

uniform sampler2D u_Panoramic1; 
uniform sampler2D u_Panoramic2;
uniform float u_Blend;
uniform float u_Exposure;         // Exposure control

const float PI = 3.14159265359;

void main() {
    //fragColor = texture(u_Texture, fragTexCoord);
    // Normalize the direction vector

    // Normalize the direction vector
    vec3 dir = normalize(fragDir);

    // Convert direction vector to spherical UV coordinates
    float u = 0.5 + atan(dir.z, dir.x) / (2.0 * PI); // Longitude
    float v = 0.5 + asin(dir.y) / PI;               // Latitude (invert to flip the image)

    // Sample the panoramic HDR textures
    vec3 color1 = texture(u_Panoramic1, vec2(u, v)).rgb;
    vec3 color2 = texture(u_Panoramic2, vec2(u, v)).rgb;

    // Blend the two textures based on u_Blend
    vec3 blendedColor = mix(color1, color2, u_Blend);

    // Apply exposure adjustment
    vec3 finalColor = blendedColor * u_Exposure;

//    // Apply gamma correction (assuming a gamma value of 2.2)
//    finalColor = pow(finalColor, vec3(1.0 / 2.2));

    // Output the color
    fragColor = vec4(finalColor, 1.0);
}