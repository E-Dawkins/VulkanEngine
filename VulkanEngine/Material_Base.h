#pragma once

class Material_Base
{
public:
    Material_Base(std::string _vertexShaderPath, std::string _fragmentShaderPath);
    virtual ~Material_Base();

    void Init();
    void CreateGraphicsResources();
    virtual void RenderMaterial(VkCommandBuffer _commandBuffer, VkPipelineLayout _pipelineLayout) const;
    void UpdateMaterial();

    VkDescriptorSetLayout* GetDescriptorSetLayout() { return &m_descriptorSetLayout; }
    VkShaderModule GetVertShaderModule() const { return m_vertShaderModule; }
    VkShaderModule GetFragShaderModule() const { return m_fragShaderModule; }
    std::vector<VkPushConstantRange> GetPushConstantRanges() { return m_pushConstantRanges; }
    
private:
    void LoadShaderModules();
    static std::vector<char> ReadFile(const std::string& _filename);
    static VkShaderModule CreateShaderModule(const std::vector<char>& _code);
    void CreateUniformBuffer();
    void CreateDescriptorPool();
    void CreateDescriptorSetLayout();
    void CreateDescriptorSets();

protected:
    virtual void AppendSetLayoutBindings(std::vector<VkDescriptorSetLayoutBinding>& _bindings) {}
    virtual void AppendExtraPoolSizes(std::vector<VkDescriptorPoolSize>& _poolSizes) {}
    virtual void AppendExtraDescriptorWrites(std::vector<VkWriteDescriptorSet>& _descriptorWrites) {}
    virtual void SetupPushConstantRanges() {}

public:
    struct UniformBufferObject
    {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    } ubo{};
    
protected:
    std::string m_vertexShaderPath;
    std::string m_fragmentShaderPath;

    VkBuffer m_uniformBuffer{};
    VkDeviceMemory m_uniformBufferMemory{};
    void* m_uniformBufferMapped{};
    
    VkDescriptorPool m_descriptorPool{};
    VkDescriptorSetLayout m_descriptorSetLayout{};
    VkDescriptorSet m_descriptorSet{};

    std::vector<VkPushConstantRange> m_pushConstantRanges{};

    VkShaderModule m_vertShaderModule{};
    VkShaderModule m_fragShaderModule{};
};
