#include "pch.h"
#include "MeshComponent.h"

MeshComponent::MeshComponent()
{
    // Attach the Mesh transform to the MeshComp transform
    m_meshPtr->transform.SetParent(&transform);
}

void MeshComponent::TickComponent(const float _deltaSeconds)
{
    if (m_meshPtr)
    {
        m_meshPtr->transform.UpdateTransform();
        m_meshPtr->UpdateMesh(_deltaSeconds);
    }
}

