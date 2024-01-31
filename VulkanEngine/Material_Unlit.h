#pragma once
#include "Material_Base.h"
#include "Texture.h"
#include "TextureSampler.h"


class Material_Unlit final : public Material_Base
{
public:
    Material_Unlit(const std::string& _vertexShaderPath, const std::string& _fragmentShaderPath, VkRenderPass _renderPass)
        : Material_Base(_vertexShaderPath, _fragmentShaderPath, _renderPass) { }
    
    void SetTexture(const std::shared_ptr<Texture>& _newTexture)
    {
        m_texture = _newTexture;
        m_textureSampler = Renderer::GetInstance()->GetTextureSampler(_newTexture);
    }

private:
    void RenderMaterial(VkCommandBuffer _commandBuffer) const override;
    void AppendSetLayoutBindings(std::vector<VkDescriptorSetLayoutBinding>& _bindings) override;
    void AppendExtraPoolSizes(std::vector<VkDescriptorPoolSize>& _poolSizes) override;
    void AppendExtraDescriptorWrites(std::vector<VkWriteDescriptorSet>& _descriptorWrites) override;
    void SetupPushConstantRanges() override;

public:
    struct PushConstantData
    {
        glm::vec4 color = glm::vec4(1);
    } pushConstants{};


private:
    std::shared_ptr<Texture> m_texture = nullptr;
    std::shared_ptr<TextureSampler> m_textureSampler = nullptr;
};
