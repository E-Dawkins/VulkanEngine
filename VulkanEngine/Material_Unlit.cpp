#include "pch.h"
#include "Material_Unlit.h"

#include "TextureSampler.h"

void Material_Unlit::RenderMaterial(VkCommandBuffer _commandBuffer) const
{
    Material_Base::RenderMaterial(_commandBuffer);
    
    vkCmdPushConstants(_commandBuffer, m_pipelineLayout, m_pushConstantRanges[0].stageFlags, m_pushConstantRanges[0].offset, m_pushConstantRanges[0].size, &pushConstants);
}

void Material_Unlit::AppendSetLayoutBindings(std::vector<VkDescriptorSetLayoutBinding>& _bindings)
{
    // Add binding for texture sampler
    _bindings.emplace_back();
    _bindings.back().binding = 1;
    _bindings.back().descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    _bindings.back().descriptorCount = 1;
    _bindings.back().stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    _bindings.back().pImmutableSamplers = nullptr; // optional
}

void Material_Unlit::AppendExtraPoolSizes(std::vector<VkDescriptorPoolSize>& _poolSizes)
{
    // Add pool size for texture sampler
    _poolSizes.emplace_back();
    _poolSizes.back().type = VK_DESCRIPTOR_TYPE_SAMPLER;
    _poolSizes.back().descriptorCount = 1;
}

void Material_Unlit::AppendExtraDescriptorWrites(std::vector<VkWriteDescriptorSet>& _descriptorWrites)
{
    // Add write descriptor set for texture sampler
    auto* imageInfo = new VkDescriptorImageInfo();
    imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo->imageView = m_texture->GetImageView();
    imageInfo->sampler = m_textureSampler->Sampler();

    _descriptorWrites.emplace_back();
    _descriptorWrites.back().sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    _descriptorWrites.back().dstSet = m_descriptorSet;
    _descriptorWrites.back().dstBinding = 1;
    _descriptorWrites.back().dstArrayElement = 0;
    _descriptorWrites.back().descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    _descriptorWrites.back().descriptorCount = 1;
    _descriptorWrites.back().pImageInfo = imageInfo;
}

void Material_Unlit::SetupPushConstantRanges()
{
    m_pushConstantRanges.emplace_back();
    m_pushConstantRanges[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    m_pushConstantRanges[0].offset = 0;
    m_pushConstantRanges[0].size = sizeof(PushConstantData);
}
