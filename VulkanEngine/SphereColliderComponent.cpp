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

    glm::vec3 aToB = normalize(otherSphere->transform.GetWorldPosition() - transform.GetWorldPosition());
    glm::vec3 bToA = normalize(transform.GetWorldPosition() - otherSphere->transform.GetWorldPosition());
    
    float rA = length(aToB * transform.GetWorldScale());
    float rB = length(bToA * otherSphere->transform.GetWorldScale());
    
    // Not colliding, return false
    if (distance(transform.GetWorldPosition(), otherSphere->transform.GetWorldPosition()) > rA + rB)
    {
        return false;
    }
    
    // Collision point is the edge point of the sphere
    _collisionPoint = transform.GetWorldPosition() + aToB * transform.GetWorldScale();

    // Collision normal is just the direction to the contact point
    _collisionNormal = normalize(_collisionPoint - transform.GetWorldPosition());

    // Collision penetration is the radius - distance to collision point
    _penetration = abs(rA - distance(transform.GetWorldPosition(), _collisionPoint));
    
    return true;
}

glm::mat3 SphereColliderComponent::GetMoment() const
{
    if (m_kinematic)
    {
        return glm::mat3(1);
    }

    glm::vec3 scale = transform.GetWorldScale();
    float inertia = (2.f / 3.f) * m_mass;
    
    glm::mat3 moment = glm::mat3(1);
    moment[0].x = inertia * scale.x * scale.x;
    moment[1].y = inertia * scale.y * scale.y;
    moment[2].z = inertia * scale.z * scale.z;
    
    return moment;
}
