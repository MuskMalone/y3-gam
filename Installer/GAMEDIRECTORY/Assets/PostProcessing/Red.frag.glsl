#version 460 core

in vec2 fragTexCoord; // Received texture coordinate from vertex shader

uniform vec3      u_Resolution;           // viewport resolution (in pixels)
uniform float     u_Time;                 // shader playback time (in seconds)
// uniform float     iTimeDelta;            // render time (in seconds)
// uniform float     iFrameRate;            // shader frame rate
uniform int       u_Frame;                // shader playback frame
// uniform float     iChannelTime[4];       // channel playback time (in seconds)
// uniform vec3      iChannelResolution[4]; // channel resolution (in pixels)
uniform vec4      u_Mouse;                // mouse pixel coords. xy: current (if MLB down), zw: click

uniform sampler2D u_ScreenTex;

out vec4 fragColor;

void main() {
    fragColor = texture(u_ScreenTex, fragTexCoord) * vec4(1,0.1,0.5,1);
}