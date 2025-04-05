#version 460 core

in vec2 v_TexCoord;

uniform sampler2D u_TexViewPosition;
uniform sampler2D u_TexFragColor;
uniform sampler2D u_BloomColor;

layout(location=0) out vec4 fragColor;
uniform float u_MinDist;
uniform float u_MaxDist;
uniform vec3 u_FogColor;
void main()
{
    float eps = 0.001;
    vec4 texRes = texture(u_TexViewPosition, v_TexCoord);
    vec3 pos = texRes.xyz;
    float w = texRes.a;
    if (abs(pos.x) < eps && abs(pos.y) < eps && abs(pos.z) < eps){
        pos = vec3(99999.f);
    }
    float dist = length(pos);
    float fogFactor = clamp((dist - u_MinDist) / (u_MaxDist - u_MinDist), 0.0f, 1.0f);
    vec3 color = texture(u_TexFragColor, v_TexCoord).xyz;
    if (abs(w) > eps)
        fragColor = vec4(mix(color, u_FogColor, fogFactor), 1.0f) + texture(u_BloomColor, v_TexCoord);
    else{
        fragColor = vec4(color, 1.0f) + texture(u_BloomColor, v_TexCoord);
    }
    // fragColor = texture(u_BloomColor, v_TexCoord);
    // fragColor = texture(u_TexFragColor, v_TexCoord);
}