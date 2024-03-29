#version 450

layout(binding = 0) uniform UniformBufferObject
{
    mat4 view;
    mat4 proj;
} ubo;

layout(push_constant) uniform PushConsts
{
    mat4 model;
    vec4 color;
} pushConsts;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec4 fragColor;

void main()
{
    gl_Position = ubo.proj * ubo.view * pushConsts.model * vec4(inPosition, 1.0);
    fragTexCoord = inTexCoord;
    fragColor = pushConsts.color;
}