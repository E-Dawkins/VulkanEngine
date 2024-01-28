#include "pch.h"
#include "MeshComponent.h"

void MeshComponent::TickComponent(const float _deltaSeconds)
{
    if (m_meshPtr)
    {
        if (!m_meshPtr->transform.m_parent)
        {
            m_meshPtr->transform.SetParent(&transform);
        }
        
        m_meshPtr->transform.UpdateTransform();
        
        m_meshPtr->UpdateMesh(_deltaSeconds);
    }
}

