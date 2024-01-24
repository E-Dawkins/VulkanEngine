#include "pch.h"
#include "TextureSampler.h"

#include "Renderer.h"
#include "Texture.h"

TextureSampler::TextureSampler(const Texture* _texture)
{
    if (!_texture)
    {
        throw std::runtime_error("Texture sampler created with a null texture!");
        return;
    }
    
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(Renderer::GetPhysicalDevice(), &properties);

    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.minLod = 0.f; // optional
    samplerInfo.maxLod = static_cast<float>(_texture->GetMipLevels());
    samplerInfo.mipLodBias = 0.f; // optional

    if (vkCreateSampler(Renderer::GetDevice(), &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

TextureSampler::~TextureSampler()
{
    vkDestroySampler(Renderer::GetDevice(), m_sampler, nullptr);
}
