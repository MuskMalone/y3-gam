#version 460 core

in vec2 fragTexCoord;
out vec4 fragColor;

uniform vec2      u_Resolution;
uniform float     u_Strength; 
uniform sampler2D u_ScreenTex; 

void mainImage(out vec4 O, in vec2 fragCoord) {
    vec2 uv = fragCoord / u_Resolution.xy;
    
    // Calculate the distance from the center
    vec2 center = vec2(0.5, 0.5);
    float dist = length(uv - center);
    
    // Use uniform for vignette strength
    float vignette = smoothstep(1.0, 0.5, dist * u_Strength);
    
    vec4 texColor = texture(u_ScreenTex, uv);
    O = texColor * vignette;
}

void main() {
    mainImage(fragColor, fragTexCoord * u_Resolution);
}
