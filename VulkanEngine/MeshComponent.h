#pragma once
#include "SceneComponent.h"

#include "Material_Base.h"
#include "Mesh.h"

class MeshComponent : public SceneComponent
{
public:
    MeshComponent();
    
    void TickComponent(float _deltaSeconds) override;
    void CleanupComponent() override;

    void SetMesh(const std::shared_ptr<Mesh>& _mesh) { m_meshPtr = _mesh; }
    void SetMaterial(const std::shared_ptr<Material_Base>& _material) { m_material = _material; }

private:
    void DrawMesh(VkCommandBuffer _commandBuffer) const;

private:
    std::shared_ptr<Mesh> m_meshPtr{new Mesh()};
    std::shared_ptr<Material_Base> m_material = nullptr;
};
