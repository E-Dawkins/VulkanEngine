#include "pch.h"
#include "SphereColliderComponent.h"

SphereColliderComponent::SphereColliderComponent()
{
    m_type = CT_SPHERE;

    m_visualizedMesh = Renderer::GetInstance()->GetMesh("sphere");
}

bool SphereColliderComponent::SphereCollision(ColliderComponent* _otherCollider)
{
    const auto otherSphere = dynamic_cast<SphereColliderComponent*>(_otherCollider);

    const float rA = GetRadius();
    const float rB = otherSphere->GetRadius();
    
    // Not colliding, return early
    const float dist = distance(transform.GetWorldPosition(), otherSphere->transform.GetWorldPosition());
    if (dist > rA + rB)
    {
        return false;
    }
    
    // Collision point is the closest point on sphere A
    glm::vec3 contact = MathHelpers::ClosestPointOnSphere(otherSphere->transform.GetWorldPosition(), transform.GetWorldPosition(), rA);

    // Collision normal is just the direction to the collision point
    glm::vec3 normal = normalize(contact - transform.GetWorldPosition());

    // Collision penetration is radius - distance to point
    float pen = rB - length(contact - otherSphere->transform.GetWorldPosition());

    ResolveCollision(otherSphere, contact, normal, pen);
    
    return true;
}

glm::mat3 SphereColliderComponent::GetMoment() const
{
    if (m_kinematic)
    {
        return glm::mat3(1);
    }

    return glm::mat3((2.f / 3.f) * GetMass() * GetRadius() * GetRadius());
}

glm::mat4 SphereColliderComponent::GetColliderMatrix()
{
    Transform copy = transform;
    copy.SetWorldScale(glm::vec3(GetRadius()));
    return copy.GetWorldMatrix();
}
