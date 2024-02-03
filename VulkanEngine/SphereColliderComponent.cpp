#include "pch.h"
#include "SphereColliderComponent.h"

SphereColliderComponent::SphereColliderComponent()
{
    m_type = CT_SPHERE;

    m_visualizedMesh = Renderer::GetInstance()->GetMesh("sphere");
}

bool SphereColliderComponent::SphereCollision(ColliderComponent* _otherCollider, glm::vec3& _collisionPoint, glm::vec3& _collisionNormal, float& _penetration)
{
    const auto otherSphere = dynamic_cast<SphereColliderComponent*>(_otherCollider);
    const float dist = distance(transform.GetWorldPosition(), otherSphere->transform.GetWorldPosition());
    
    // Not colliding, return false
    if (dist > m_radius + otherSphere->m_radius)
    {
        return false;
    }
    
    // Collision point is the edge point of the sphere
    glm::vec3 direction = otherSphere->transform.GetWorldPosition() - transform.GetWorldPosition();
    _collisionPoint = direction * m_radius;

    // Collision normal is just the direction to the contact point
    _collisionNormal = _collisionPoint - transform.GetWorldPosition();

    // Collision penetration is the radius - distance to collision point
    _penetration = length(transform.GetWorldScale()) - distance(transform.GetWorldPosition(), _collisionPoint);
    
    return true;
}
