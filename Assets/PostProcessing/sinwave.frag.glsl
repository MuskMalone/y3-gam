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

void mainImage( out vec4 fc, in vec2 fragCoord )
{
    float POWER = 0.002; // How much the effect can spread horizontally
    float VERTICAL_SPREAD = 7.0; // How vertically is the sin wave spread
    float ANIM_SPEED = 0.4f; // Animation speed
    
	vec2 uv = fragCoord.xy;
    float y = (uv.y + u_Time * ANIM_SPEED) * VERTICAL_SPREAD;
    
    uv.x += ( 
        // This is the heart of the effect, feel free to modify
        // The sin functions here or add more to make it more complex 
        // and less regular
        sin(y) 
        + sin(y * 10.0) * 0.2 
        + sin(y * 50.0) * 0.03
    ) 
        * POWER // Limit by maximum spread
        * sin(uv.y * 3.14) // Disable on edges / make the spread a bell curve
        * sin(u_Time); // And make the power change in time
    
	fc = texture(u_ScreenTex, uv);
}

void main(){
    //fragColor = vec4(u_Resolution, 0, 1);
    mainImage(fragColor, fragTexCoord);
}