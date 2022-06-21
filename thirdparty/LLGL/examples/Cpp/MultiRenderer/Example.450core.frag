// GLSL Fragment Shader "PS"
// Generated by XShaderCompiler
// 14/08/2019 22:56:59

#version 450

layout(location = 0) in vec2 xsv_TEXCOORD0;

layout(location = 0) out vec4 SV_Target0;

layout(binding = 1) uniform texture2D colorMap;

layout(binding = 2) uniform sampler colorMapSampler;

void main()
{
    vec4 color = texture(sampler2D(colorMap, colorMapSampler), xsv_TEXCOORD0);
    SV_Target0 = mix(vec4(1), color, vec4(color.a));
}

