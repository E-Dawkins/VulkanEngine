#include "pch.h"
#include "Material_Base.h"

#include <fstream>

#include "Mesh.h"
#include "Renderer.h"

Material_Base::Material_Base(std::string _vertexShaderPath, std::string _fragmentShaderPath, const VkRenderPass _renderPass) :
    m_vertexShaderPath(std::move(_vertexShaderPath)), m_fragmentShaderPath(std::move(_fragmentShaderPath)), m_renderPass(_renderPass)
{
    
}

Material_Base::~Material_Base()
{
    vkDestroyBuffer(Renderer::GetInstance()->GetDevice(), m_uniformBuffer, nullptr);
    vkFreeMemory(Renderer::GetInstance()->GetDevice(), m_uniformBufferMemory, nullptr);

    vkDestroyDescriptorPool(Renderer::GetInstance()->GetDevice(), m_descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(Renderer::GetInstance()->GetDevice(), m_descriptorSetLayout, nullptr);

    vkDestroyPipeline(Renderer::GetInstance()->GetDevice(), m_graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(Renderer::GetInstance()->GetDevice(), m_pipelineLayout, nullptr);
}

void Material_Base::Init()
{
    SetupPushConstantRanges();
    LoadShaderModules();

    CreateUniformBuffer();
    CreateDescriptorPool();
    CreateDescriptorSetLayout();
    CreateDescriptorSets();
    
    CreateGraphicsPipeline();
}

void Material_Base::RenderMaterial(const VkCommandBuffer _commandBuffer) const
{
    vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);
    vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSet, 0, nullptr);
    vkCmdPushConstants(_commandBuffer, m_pipelineLayout, m_pushConstantRange.stageFlags, m_pushConstantRange.offset, m_pushConstantRange.size, &pushConstants);
}

void Material_Base::UpdateMaterial()
{
    // Update 'camera' view / projection matrices
    ubo.view = lookAt(glm::vec3(10, 0, 5), glm::vec3(0), g_upVector);
    ubo.proj = glm::perspective(glm::radians(45.f), Renderer::GetInstance()->GetSwapchainRatio(), 0.1f, 100.f);
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
    if (vkCreateShaderModule(Renderer::GetInstance()->GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

void Material_Base::CreateUniformBuffer()
{
    constexpr VkDeviceSize bufferSize = sizeof(UniformBufferObject);
        
    Renderer::GetInstance()->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 m_uniformBuffer, m_uniformBufferMemory);

    vkMapMemory(Renderer::GetInstance()->GetDevice(), m_uniformBufferMemory, 0, bufferSize, 0, &m_uniformBufferMapped);
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

    if (vkCreateDescriptorPool(Renderer::GetInstance()->GetDevice(), &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
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

    // Per material bindings (i.e. texture samplers, other uniforms, etc.)
    AppendSetLayoutBindings(bindings);
    
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(Renderer::GetInstance()->GetDevice(), &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void Material_Base::CreateGraphicsPipeline()
{
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = m_vertShaderModule;
    vertShaderStageInfo.pName = "main"; // shader entry point

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = m_fragShaderModule;
    fragShaderStageInfo.pName = "main"; // shader entry point

    VkPipelineShaderStageCreateInfo shaderStages[] =
    {
        vertShaderStageInfo,
        fragShaderStageInfo
    };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    const auto bindingDescription = Mesh::Vertex::GetBindingDescription();
    const auto attributeDescriptions = Mesh::Vertex::GetAttributeDescriptions();
    
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    
    VkViewport viewport{};
    viewport.x = 0.f;
    viewport.y = 0.f;
    viewport.width = static_cast<float>(Renderer::GetInstance()->GetSwapchainExtent().width);
    viewport.height = static_cast<float>(Renderer::GetInstance()->GetSwapchainExtent().height);
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = Renderer::GetInstance()->GetSwapchainExtent();

    std::vector dynamicStates =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = m_fillMode;
    rasterizer.lineWidth = 1.f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_TRUE; // enable sample shading in the pipeline
    multisampling.minSampleShading = .2f; // min fraction for sample shading, closer to one is smoother
    multisampling.rasterizationSamples = Renderer::GetInstance()->GetMsaaSampleCount();

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
        | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &m_pushConstantRange;

    if (vkCreatePipelineLayout(Renderer::GetInstance()->GetDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = m_renderPass;
    pipelineInfo.subpass = 0;

    if (vkCreateGraphicsPipelines(Renderer::GetInstance()->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }
    
    vkDestroyShaderModule(Renderer::GetInstance()->GetDevice(), m_fragShaderModule, nullptr);
    vkDestroyShaderModule(Renderer::GetInstance()->GetDevice(), m_vertShaderModule, nullptr);
}

void Material_Base::CreateDescriptorSets()
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_descriptorSetLayout;
    
    if (vkAllocateDescriptorSets(Renderer::GetInstance()->GetDevice(), &allocInfo, &m_descriptorSet) != VK_SUCCESS)
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

    vkUpdateDescriptorSets(Renderer::GetInstance()->GetDevice(), (uint32_t)descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

void Material_Base::SetupPushConstantRanges()
{
    m_pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    m_pushConstantRange.offset = 0;
    m_pushConstantRange.size = sizeof(PushConstantData);
}
