#include "pch.h"
#include "Transform.h"

void Transform::UpdateTransform()
{
    // Using the local variables + parent variables, generate the world variables
    if (m_parent)
    {
        m_parent->UpdateTransform();

        m_worldPosition = m_parent->m_worldPosition + m_localPosition;
        m_worldRotation = m_parent->m_worldRotation * m_localRotation;
        m_worldScale = m_parent->m_worldScale * m_localScale;
    }
}

glm::mat4 Transform::GetWorldMatrix() const
{
    const glm::mat4 positionMatrix = translate(glm::mat4(1.f), m_worldPosition);
    const glm::mat4 rotationMatrix = mat4_cast(m_worldRotation);
    const glm::mat4 scaleMatrix = scale(glm::mat4(1.f), m_worldScale);
    
    return positionMatrix * rotationMatrix * scaleMatrix;
}

glm::mat4 Transform::GetLocalMatrix() const
{
    const glm::mat4 positionMatrix = translate(glm::mat4(1.f), m_localPosition);
    const glm::mat4 rotationMatrix = mat4_cast(m_localRotation);
    const glm::mat4 scaleMatrix = scale(glm::mat4(1.f), m_localScale);
    
    return positionMatrix * rotationMatrix * scaleMatrix;
}
