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

void mainImage( out vec4 O,  vec2 U )
{
    float ABERRATION_X = 0.06;
    float ABERRATION_Y = 0.06;
    vec2 R = u_Resolution.xy, m = vec2(ABERRATION_X, ABERRATION_Y); 
	// U/= R;
    float d = (length(m)<.02) ? .015 : m.x/10.;
  //float d = (length(m)<.02) ? .05-.05*cos(iDate.w) : m.x/10.;
 
	O = vec4( texture(u_ScreenTex,U-d).r,
              texture(u_ScreenTex,U  ).g,
              texture(u_ScreenTex,U+d).b,
              1);
}

void main(){
    mainImage(fragColor, fragTexCoord);
}