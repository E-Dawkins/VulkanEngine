#include "pch.h"
#include "Material_Base.h"

#include <fstream>

#include "Renderer.h"

Material_Base::Material_Base(std::string _vertexShaderPath, std::string _fragmentShaderPath)
    : m_vertexShaderPath(std::move(_vertexShaderPath)), m_fragmentShaderPath(std::move(_fragmentShaderPath))
{
    
}

Material_Base::~Material_Base()
{
    vkDestroyBuffer(Renderer::GetDevice(), m_uniformBuffer, nullptr);
    vkFreeMemory(Renderer::GetDevice(), m_uniformBufferMemory, nullptr);

    vkDestroyDescriptorPool(Renderer::GetDevice(), m_descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(Renderer::GetDevice(), m_descriptorSetLayout, nullptr);
}

void Material_Base::Init()
{
    SetupPushConstantRanges();
    LoadShaderModules();

    CreateUniformBuffer();
    CreateDescriptorPool();
    CreateDescriptorSetLayout();
}

void Material_Base::CreateGraphicsResources()
{
    CreateDescriptorSets();
}

void Material_Base::RenderMaterial(const VkCommandBuffer _commandBuffer, const VkPipelineLayout _pipelineLayout) const
{
    vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &m_descriptorSet, 0, nullptr);
}

void Material_Base::UpdateMaterial()
{
    // Update 'camera' view / projection matrices
    ubo.view = lookAt(glm::vec3(2), glm::vec3(0), glm::vec3(0.f, 0.f, 1.f));
    ubo.proj = glm::perspective(glm::radians(45.f), Renderer::GetSwapchainRatio(), 0.1f, 10.f);
    ubo.proj[1][1] *= -1;

    memcpy(m_uniformBufferMapped, &ubo, sizeof(ubo));
}

void Material_Base::LoadShaderModules()
{
    const std::vector<char> vertShaderCode = ReadFile(m_vertexShaderPath);
    const std::vector<char> fragShaderCode = ReadFile(m_fragmentShaderPath);

    m_vertShaderModule = CreateShaderModule(vertShaderCode);
    m_fragShaderModule = CreateShaderModule(fragShaderCode);
}

std::vector<char> Material_Base::ReadFile(const std::string& _filename)
{
    std::ifstream file(_filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    const size_t fileSize = file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), static_cast<std::streamsize>(fileSize));

    file.close();

    return buffer;
}

VkShaderModule Material_Base::CreateShaderModule(const std::vector<char>& _code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = _code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(_code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(Renderer::GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

void Material_Base::CreateUniformBuffer()
{
    constexpr VkDeviceSize bufferSize = sizeof(UniformBufferObject);
        
    Renderer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 m_uniformBuffer, m_uniformBufferMemory);

    vkMapMemory(Renderer::GetDevice(), m_uniformBufferMemory, 0, bufferSize, 0, &m_uniformBufferMapped);
}

void Material_Base::CreateDescriptorPool()
{
    // Base material pool size (uniform buffer)
    std::vector<VkDescriptorPoolSize> poolSizes{};

    poolSizes.emplace_back();
    poolSizes.back().type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes.back().descriptorCount = 1;

    // Per material pool sizes (i.e. texture samplers, other uniforms, etc.)
    AppendExtraPoolSizes(poolSizes);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = 1;

    if (vkCreateDescriptorPool(Renderer::GetDevice(), &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void Material_Base::CreateDescriptorSetLayout()
{
    // Base material binding (uniform buffer)
    std::vector<VkDescriptorSetLayoutBinding> bindings{};

    bindings.emplace_back();
    bindings.back().binding = 0;
    bindings.back().descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings.back().descriptorCount = 1;
    bindings.back().stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    bindings.back().pImmutableSamplers = nullptr; // optional

    // Per material bindings (i.e. texture samplers, other uniforms, etc.)
    AppendSetLayoutBindings(bindings);
    
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(Renderer::GetDevice(), &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void Material_Base::CreateDescriptorSets()
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
    descriptorWrites.back().sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites.back().dstSet = m_descriptorSet;
    descriptorWrites.back().dstBinding = 0;
    descriptorWrites.back().dstArrayElement = 0;
    descriptorWrites.back().descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites.back().descriptorCount = 1;
    descriptorWrites.back().pBufferInfo = &bufferInfo;

    // Append extra per child-material descriptor writes
    AppendExtraDescriptorWrites(descriptorWrites);

    vkUpdateDescriptorSets(Renderer::GetDevice(), (uint32_t)descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}
