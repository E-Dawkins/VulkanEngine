#pragma once
#include "Material_Base.h"
#include "Texture.h"

class Material_Unlit final : public Material_Base
{
public:
    Material_Unlit(const std::string& _vertexShaderPath, const std::string& _fragmentShaderPath)
        : Material_Base(_vertexShaderPath, _fragmentShaderPath) { }
    ~Material_Unlit();

private:
    void RenderMaterial(VkCommandBuffer _commandBuffer, VkPipelineLayout _pipelineLayout) const override;
    std::vector<VkDescriptorSetLayoutBinding> GetSetLayoutBindings() override;
    std::vector<VkDescriptorPoolSize> GetExtraPoolSizes() override;
    void CreateDescriptorSets() override;
    void SetupPushConstantRanges() override;

public:
    struct PushConstantData
    {
        glm::vec4 color;
    } pushConstants{};

    Texture* texture = nullptr;
};
