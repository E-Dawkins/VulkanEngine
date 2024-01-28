#pragma once
#include "Mesh.h"
#include "SceneComponent.h"

class MeshComponent : public SceneComponent
{
public:
    void TickComponent(float _deltaSeconds) override;

    Mesh*& GetMesh() { return m_meshPtr; }

private:
    Mesh* m_meshPtr = nullptr;
};
