#include "pch.h"
#include "Material_Unlit.h"

#include "Renderer.h"

Material_Unlit::~Material_Unlit()
{
    delete texture;
}

void Material_Unlit::RenderMaterial(VkCommandBuffer _commandBuffer, VkPipelineLayout _pipelineLayout) const
{
    Material_Base::RenderMaterial(_commandBuffer, _pipelineLayout);
    
    vkCmdPushConstants(_commandBuffer, _pipelineLayout, m_pushConstantRanges[0].stageFlags, m_pushConstantRanges[0].offset, m_pushConstantRanges[0].size, &pushConstants);
}

std::vector<VkDescriptorSetLayoutBinding> Material_Unlit::GetSetLayoutBindings()
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    bindings.emplace_back();
    bindings[0].binding = 1;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings[0].pImmutableSamplers = nullptr; // optional

    return bindings;
}

std::vector<VkDescriptorPoolSize> Material_Unlit::GetExtraPoolSizes()
{
    std::vector<VkDescriptorPoolSize> poolSizes;
    
    poolSizes.emplace_back();
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = 1;

    return poolSizes;
}

void Material_Unlit::CreateDescriptorSets()
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_descriptorSetLayout;
    
    if (vkAllocateDescriptorSets(Renderer::GetDevice(), &allocInfo, &m_descriptorSet) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    // Base material descriptor writes
    std::vector<VkWriteDescriptorSet> descriptorWrites{};

    VkDescriptorBufferInfo bufferInfo;
    bufferInfo.buffer = m_uniformBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);
    
    descriptorWrites.emplace_back();
    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = m_descriptorSet;
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfo;
    
    VkDescriptorImageInfo imageInfo;
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = texture->GetImageView();
    imageInfo.sampler = texture->GetSampler();

    descriptorWrites.emplace_back();
    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = m_descriptorSet;
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(Renderer::GetDevice(), (uint32_t)descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

void Material_Unlit::SetupPushConstantRanges()
{
    m_pushConstantRanges.emplace_back();
    m_pushConstantRanges[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    m_pushConstantRanges[0].offset = 0;
    m_pushConstantRanges[0].size = sizeof(PushConstantData);
}
