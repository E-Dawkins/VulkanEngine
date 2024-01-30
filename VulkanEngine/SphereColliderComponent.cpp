#include "pch.h"
#include "SphereColliderComponent.h"

SphereColliderComponent::SphereColliderComponent()
{
    m_type = CT_SPHERE;
}

bool SphereColliderComponent::SphereCollision(ColliderComponent* _otherCollider, float& _collisionVelocity, glm::vec3& _collisionPoint)
{
    const auto otherSphere = dynamic_cast<SphereColliderComponent*>(_otherCollider);
    const float distance = glm::distance(transform.GetWorldPosition(), otherSphere->transform.GetWorldPosition());

    // Not colliding, return false
    if (distance > m_radius + otherSphere->m_radius)
    {
        return false;
    }
    
    // The collision velocity is the total net velocity in the system
    _collisionVelocity = length(m_velocity + otherSphere->m_velocity);

    // Collision point is the mid point between both colliders
    _collisionPoint = (transform.GetWorldPosition() + otherSphere->transform.GetWorldPosition()) * 0.5f;
    
    return true;
}
