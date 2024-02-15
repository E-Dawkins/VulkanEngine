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
    // Otherwise, just set them to their local counterparts
    else
    {
        m_worldPosition = m_localPosition;
        m_worldRotation = m_localRotation;
        m_worldScale = m_localScale;
    }
}

#pragma region Getters
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

glm::vec3 Transform::GetWorldPosition() const
{
    return m_worldPosition;
}

glm::quat Transform::GetWorldRotation() const
{
    return m_worldRotation;
}

glm::vec3 Transform::GetWorldScale() const
{
    return m_worldScale;
}

glm::vec3 Transform::GetLocalPosition() const
{
    return m_localPosition;
}

glm::quat Transform::GetLocalRotation() const
{
    return m_localRotation;
}

glm::vec3 Transform::GetLocalScale() const
{
    return m_localScale;
}

glm::vec3 Transform::GetLocalUp() const
{
    return normalize(m_worldRotation * g_upVector * conjugate(m_worldRotation));
}

glm::vec3 Transform::GetLocalRight() const
{
    return normalize(m_worldRotation * g_rightVector * conjugate(m_worldRotation));
}

glm::vec3 Transform::GetLocalForward() const
{
    return normalize(m_worldRotation * g_forwardVector * conjugate(m_worldRotation));
}

#pragma endregion

#pragma region Setters
void Transform::SetParent(Transform* _parent)
{
    m_parent = _parent;

    UpdateTransform();
}

void Transform::SetWorldPosition(const glm::vec3 _worldPos)
{
    m_worldPosition = _worldPos;
    
    // If we have a parent, set the local position to the distance vector between us and our parent
    // Otherwise, just set it to the world position
    m_localPosition = (m_parent ? _worldPos - m_parent->m_worldPosition : _worldPos);
}

void Transform::SetWorldRotation(const glm::quat _worldRot)
{
    m_worldRotation = _worldRot;

    // If we have a parent, set the local rotation to the difference between parent rotation and our rotation
    // Otherwise, just set it to the world rotation
    m_localRotation = (m_parent ? _worldRot - m_parent->m_worldRotation : _worldRot);
}

void Transform::SetWorldScale(const glm::vec3 _worldScale)
{
    m_worldScale = _worldScale;

    // If we have a parent, set the local scale to the ratio of our world scale and parents' world scale
    // Otherwise, just set it to the world scale
    m_localScale = (m_parent ? _worldScale / m_parent->m_worldScale : _worldScale);
}

void Transform::SetLocalPosition(const glm::vec3 _localPos)
{
    m_localPosition = _localPos;

    // If we have a parent, set the world position to the combined position of parent world & our local
    // Otherwise, just set it to the local position
    m_worldPosition = (m_parent ? m_parent->m_worldPosition + _localPos : _localPos);
}

void Transform::SetLocalRotation(glm::quat _localRot)
{
    m_localRotation = _localRot;

    // If we have a parent, set the world rotation to the combined rotation of parent world & our local
    // Otherwise, just set it to the local rotation
    m_worldRotation = (m_parent ? m_parent->m_worldRotation + _localRot : _localRot);
}

void Transform::SetLocalScale(glm::vec3 _localScale)
{
    m_localScale = _localScale;

    // If we have a parent, set the world scale to the product of parents' world scale & our local scale
    // Otherwise, just set it to the local scale
    m_worldScale = (m_parent ? m_parent->m_worldScale * _localScale : _localScale);
}
#pragma endregion