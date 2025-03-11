// #version 460 core

// in vec2 v_TexCoord;

// uniform sampler2D u_BloomColor;
// uniform vec2 u_Resolution;
// uniform int u_PassId;
// layout(location=1) out vec4 fragColor; // saves this to an alt fb

// int samples = 100;
// int LOD = 1;         // gaussian done on MIPmap at scale LOD
// int sLOD = 1 << LOD; // tile size = 2^LOD
// float sigma = float(samples) * 0.25;

// float gaussian(vec2 i) {
//     i /= sigma;
//     return exp(-0.5 * dot(i, i)) / (6.28 * sigma * sigma);
// }

// vec4 blur(sampler2D sp, vec2 uv, vec2 scale, float blurScale)
// {
//     vec4 sumColor = vec4(0.0);
//     float sumWeight = 0.0;
//     int s = samples / sLOD;
    
//     for (int i = 0; i < s * s; i++)
//     {
//         // Compute an offset in pixel coordinates. Then multiply by scale (1/resolution)
//         // and by blurScale to control the radius of the blur.
//         vec2 d = vec2(i % s, i / s) * float(sLOD) - float(samples) * 0.5;
//         float w = gaussian(d);
//         // Notice the blurScale multiplying the offset!
//         vec4 c = textureLod(sp, uv + scale * d * blurScale, float(LOD));
//         // Force alpha to 1, so that all colors are treated equally.
//         c.a = 1.0;
//         sumColor += c * w;
//         sumWeight += w;
//     }
    
//     return vec4(sumColor.rgb / sumWeight, 1.0);
// }

// void main() {
    // // Fetch the bloom properties (color and intensity)
    // vec4 bloomProps = texture(u_BloomColor, v_TexCoord);
    // // Compute a blurScale from the intensity (stored in the alpha channel)
    // float blurScale = bloomProps.w;//smoothstep(0.0, 64.0, bloomProps.w);
    // // Use the standard scale (in UV space) and your computed blurScale.
    // fragColor = blur(u_BloomColor, v_TexCoord, 1.0 / u_Resolution, blurScale);

// }
#version 460 core

// The interpolated texture coordinate from your vertex shader.
in vec2 v_TexCoord;

// The source texture. Its alpha (w) channel holds the blur intensity/scale.
uniform sampler2D u_BloomColor;

// Resolution of the image (or framebuffer).
uniform vec2 u_Resolution;

// Which pass to run: 1 for horizontal, 2 for vertical.
uniform int u_PassId;

// Output color.
layout(location = 0) out vec4 fragColor;

// Gaussian function in 1D.
float gaussian(float x, float sigma) {
    // No normalization here since we normalize later.
    return exp(-0.5 * (x * x) / (sigma * sigma));
}

//
// Horizontal blur pass with dynamic weights and sample count.
//
void pass1() {
    // Compute one pixel's size in UV space.
    vec2 fragSize = 1.0 / u_Resolution;
    
    // Sample the center pixel to get its color and the blur intensity from w.
    vec4 center = texture(u_BloomColor, v_TexCoord);
    // Clamp intensity to [0,1]. You can adjust this if your w range is different.
    float blurIntensity = clamp(center.w, 0.0, 1.0);

    // Map the intensity to a dynamic blur radius.
    // For example, if intensity is 0, use 1 sample; if 1, use 64 samples.
    int dynamicBlurRad = max(1, int(mix(1.0, 64.0, blurIntensity)));
    // Also map the intensity to a sigma value.
    // For instance, when intensity is 0 use a tight sigma, and when 1 use a wide sigma.
    float sigma = mix(0.5, 10.0, blurIntensity);

    // Compute the center weight (gaussian(0) is always 1).
    float weightCenter = gaussian(0.0, sigma);
    float sumWeight = weightCenter;
    vec3 col = center.rgb * weightCenter;

    // Loop over positive offsets; sample symmetrically to the left and right.
    for (int i = 1; i < dynamicBlurRad; ++i) {
        // Compute the weight for this offset.
        float w = gaussian(float(i), sigma);
        // Multiply the UV offset by the blurIntensity so that higher intensity
        // both increases the number of samples and stretches the sampling offsets.
        float offset = fragSize.x * float(i) * blurIntensity;
        vec3 samplePos = texture(u_BloomColor, v_TexCoord + vec2(offset, 0.0)).rgb;
        vec3 sampleNeg = texture(u_BloomColor, v_TexCoord - vec2(offset, 0.0)).rgb;
        col += (samplePos + sampleNeg) * w;
        sumWeight += 2.0 * w;
    }
    
    // Normalize the accumulated color by the total weight.
    col /= sumWeight;
    // Force the output alpha to 1.
    fragColor = vec4(col, 1.0);
}

//
// Vertical blur pass with dynamic weights and sample count.
//
void pass2() {
    vec2 fragSize = 1.0 / u_Resolution;
    
    vec4 center = texture(u_BloomColor, v_TexCoord);
    float blurIntensity = clamp(center.w, 0.0, 1.0);
    int dynamicBlurRad = max(1, int(mix(1.0, 64.0, blurIntensity)));
    float sigma = mix(0.5, 10.0, blurIntensity);

    float weightCenter = gaussian(0.0, sigma);
    float sumWeight = weightCenter;
    vec3 col = center.rgb * weightCenter;

    for (int i = 1; i < dynamicBlurRad; ++i) {
        float w = gaussian(float(i), sigma);
        float offset = fragSize.y * float(i) * blurIntensity;
        vec3 samplePos = texture(u_BloomColor, v_TexCoord + vec2(0.0, offset)).rgb;
        vec3 sampleNeg = texture(u_BloomColor, v_TexCoord - vec2(0.0, offset)).rgb;
        col += (samplePos + sampleNeg) * w;
        sumWeight += 2.0 * w;
    }
    
    col /= sumWeight;
    fragColor = vec4(col, 1.0);
}

void main() {
    if (u_PassId == 1)
        pass1();
    else if (u_PassId == 2)
        pass2();
}
