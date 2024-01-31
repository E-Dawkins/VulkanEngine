#include "pch.h"
#include "MeshComponent.h"

MeshComponent::MeshComponent()
{
    // Add draw call to renderer pass
    Renderer::GetInstance()->AddDrawCall([this](const VkCommandBuffer _commandBuffer) {DrawMesh(_commandBuffer);});
}

void MeshComponent::TickComponent(const float _deltaSeconds)
{
    SceneComponent::TickComponent(_deltaSeconds);
    
    m_material->pushConstants.model = transform.GetWorldMatrix();
    m_material->UpdateMaterial();
}

void MeshComponent::CleanupComponent()
{
    SceneComponent::CleanupComponent();

    delete m_material;
}

void MeshComponent::DrawMesh(const VkCommandBuffer _commandBuffer) const
{
    m_material->RenderMaterial(_commandBuffer);
    m_meshPtr->DrawMesh(_commandBuffer);
}

