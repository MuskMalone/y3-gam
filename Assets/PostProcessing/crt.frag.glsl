#version 460 core
in vec2 fragTexCoord; // Received texture coordinate from vertex shader
out vec4 fragColor;

uniform vec2      u_Resolution;           // viewport resolution (in pixels)
uniform float     u_Time;                 // shader playback time (in seconds)
// uniform float     iTimeDelta;            // render time (in seconds)
// uniform float     iFrameRate;            // shader frame rate
uniform int       u_Frame;                // shader playback frame
// uniform float     iChannelTime[4];       // channel playback time (in seconds)
// uniform vec3      iChannelResolution[4]; // channel resolution (in pixels)
uniform vec4      u_Mouse;                // mouse pixel coords. xy: current (if MLB down), zw: click
uniform sampler2D u_ScreenTex;          // input channel. XX = 2D/Cube
// uniform vec4      iDate;                 // (year, month, day, time in seconds)
// uniform float     iSampleRate;           // sound sample rate (i.e., 44100)
float warp = 0.25; // simulate curvature of CRT monitor
float scan = 0.5; // simulate darkness between scanlines

void mainImage(out vec4 fragColor,in vec2 fragCoord)
{
    // squared distance from center
    vec2 uv = fragCoord/u_Resolution.xy;
    vec2 dc = abs(0.5-uv);
    dc *= dc;
    
    // warp the fragment coordinates
    uv.x -= 0.5; uv.x *= 1.0+(dc.y*(0.3*warp)); uv.x += 0.5;
    uv.y -= 0.5; uv.y *= 1.0+(dc.x*(0.4*warp)); uv.y += 0.5;

    // sample inside boundaries, otherwise set to black

        // determine if we are drawing in a scanline
        float apply = abs(sin(fragCoord.y)*0.5*scan);
        // sample the texture
    	fragColor = vec4(mix(texture(u_ScreenTex,uv).rgb,vec3(0.0),apply),1.0);

}


void main(){
    mainImage(fragColor, fragTexCoord * u_Resolution);
}