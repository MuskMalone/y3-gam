#version 460 core

in vec2 TexCoords;
layout(location = 0) out vec4 color;

uniform sampler2D uText;
uniform vec4 uTextColor;

void main()
{    
    // Sample the texture's red channel for alpha masking
    float alpha = texture(uText, TexCoords).r;

    // Combine the text color (with alpha) and the sampled alpha
    color = vec4(uTextColor.rgb, uTextColor.a * alpha);
}