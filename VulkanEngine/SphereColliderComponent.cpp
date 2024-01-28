#include "pch.h"
#include "SphereColliderComponent.h"

SphereColliderComponent::SphereColliderComponent()
{
    m_type = CT_SPHERE;
}

void SphereColliderComponent::SphereCollision(ColliderComponent* _otherCollider)
{
    const auto otherSphere = dynamic_cast<SphereColliderComponent*>(_otherCollider);
    const float distance = glm::distance(transform.m_worldPosition, otherSphere->transform.m_worldPosition);

    if (distance < m_radius + otherSphere->m_radius)
    {
        std::cout << "Sphere on sphere collision!" << std::endl;
    }
}
