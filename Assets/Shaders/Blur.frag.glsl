#version 460 core

in vec2 v_TexCoord;

uniform sampler2D u_BloomColor;
uniform vec2 u_Resolution;
layout(location=1) out vec4 fragColor; // saves this to an alt fb

int samples = 100;
int          LOD = 1;         // gaussian done on MIPmap at scale LOD
int          sLOD = 1 << LOD; // tile size = 2^LOD
float sigma = float(samples) * .25;

float gaussian(vec2 i) {
    return exp( -.5* dot(i/=sigma,i) ) / ( 6.28 * sigma*sigma );
}

vec4 blur(sampler2D sp, vec2 uv, vec2 scale)
{
    vec4 sumColor = vec4(0.0);
    float sumWeight = 0.0;
    int s = samples / sLOD;

    for(int i = 0; i < s*s; i++)
    {
        // d is in pixel coordinates; scale converts to normalized UVs
        vec2 d = vec2(i % s, i / s) * float(sLOD) - float(samples) * 0.5;
        float w = gaussian(d);
        vec4 c = textureLod(sp, uv + scale * d, float(LOD));

        // Accumulate color * weight, and track total weight
        sumColor += c * w;
        sumWeight += w;
    }

    // Divide by the total Gaussian weight, *not* alpha
    return sumColor / sumWeight;
}

void main() {
    vec4 bloomProps = texture(u_BloomColor, v_TexCoord);
    float blurScale = smoothstep(0.0, 64.0, bloomProps.w);
    fragColor = blur(u_BloomColor, v_TexCoord, 1.0/u_Resolution);
}

