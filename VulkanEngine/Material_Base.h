#pragma once

class Material_Base
{
public:
    Material_Base(std::string _vertexShaderPath, std::string _fragmentShaderPath, VkRenderPass _renderPass);
    virtual ~Material_Base();

    void Init();
    virtual void RenderMaterial(VkCommandBuffer _commandBuffer) const;
    void UpdateMaterial();

    /* Setters */
    void SetFillMode(const VkPolygonMode _fillMode) { m_fillMode = _fillMode; }
    
private:
    void LoadShaderModules();
    static std::vector<char> ReadFile(const std::string& _filename);
    static VkShaderModule CreateShaderModule(const std::vector<char>& _code);
    void CreateUniformBuffer();
    void CreateDescriptorPool();
    void CreateDescriptorSetLayout();
    virtual void CreateGraphicsPipeline();
    void CreateDescriptorSets();
    void SetupPushConstantRanges();

protected:
    virtual void AppendSetLayoutBindings(std::vector<VkDescriptorSetLayoutBinding>& _bindings) {}
    virtual void AppendExtraPoolSizes(std::vector<VkDescriptorPoolSize>& _poolSizes) {}
    virtual void AppendExtraDescriptorWrites(std::vector<VkWriteDescriptorSet>& _descriptorWrites) {}

public:
    struct UniformBufferObject
    {
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    } ubo{};

    struct PushConstantData
    {
        glm::mat4 model = glm::mat4(1);
        glm::vec4 color = glm::vec4(1);
    } pushConstants{};
    
protected:
    std::string m_vertexShaderPath;
    std::string m_fragmentShaderPath;

    VkShaderModule m_vertShaderModule{};
    VkShaderModule m_fragShaderModule{};
    
    VkBuffer m_uniformBuffer{};
    VkDeviceMemory m_uniformBufferMemory{};
    void* m_uniformBufferMapped{};
    
    VkDescriptorPool m_descriptorPool{};
    VkDescriptorSetLayout m_descriptorSetLayout{};
    VkDescriptorSet m_descriptorSet{};

    VkPipelineLayout m_pipelineLayout{};
    VkPipeline m_graphicsPipeline{};
    VkRenderPass m_renderPass;

    // User-defined material values
    VkPushConstantRange m_pushConstantRange{};
    VkPolygonMode m_fillMode = VK_POLYGON_MODE_FILL;  // default => fill each face
};
