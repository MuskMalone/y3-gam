#version 460 core

in vec2 v_TexCoord;

uniform sampler2D u_BloomColor;
uniform vec2 u_Resolution;
layout(location=1) out vec4 fragColor; // saves this to an alt fb

int samples = 100;
int LOD = 1;         // gaussian done on MIPmap at scale LOD
int sLOD = 1 << LOD; // tile size = 2^LOD
float sigma = float(samples) * 0.25;

float gaussian(vec2 i) {
    i /= sigma;
    return exp(-0.5 * dot(i, i)) / (6.28 * sigma * sigma);
}

vec4 blur(sampler2D sp, vec2 uv, vec2 scale, float blurScale)
{
    vec4 sumColor = vec4(0.0);
    float sumWeight = 0.0;
    int s = samples / sLOD;
    
    for (int i = 0; i < s * s; i++)
    {
        // Compute an offset in pixel coordinates. Then multiply by scale (1/resolution)
        // and by blurScale to control the radius of the blur.
        vec2 d = vec2(i % s, i / s) * float(sLOD) - float(samples) * 0.5;
        float w = gaussian(d);
        // Notice the blurScale multiplying the offset!
        vec4 c = textureLod(sp, uv + scale * d * blurScale, float(LOD));
        // Force alpha to 1, so that all colors are treated equally.
        c.a = 1.0;
        sumColor += c * w;
        sumWeight += w;
    }
    
    return vec4(sumColor.rgb / sumWeight, 1.0);
}

void main() {
    // Fetch the bloom properties (color and intensity)
    vec4 bloomProps = texture(u_BloomColor, v_TexCoord);
    // Compute a blurScale from the intensity (stored in the alpha channel)
    float blurScale = bloomProps.w;//smoothstep(0.0, 64.0, bloomProps.w);
    // Use the standard scale (in UV space) and your computed blurScale.
    fragColor = blur(u_BloomColor, v_TexCoord, 1.0 / u_Resolution, blurScale);
}
