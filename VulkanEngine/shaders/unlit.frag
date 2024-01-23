#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(push_constant) uniform PushConsts {
    vec4 color;
} pushConsts;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(pushConsts.color.rgb * texture(texSampler, fragTexCoord).rgb, pushConsts.color.a);
}