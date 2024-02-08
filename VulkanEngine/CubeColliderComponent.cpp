#include "pch.h"
#include "CubeColliderComponent.h"

CubeColliderComponent::CubeColliderComponent()
{
    m_type = CT_CUBE;

    m_visualizedMesh = Renderer::GetInstance()->GetMesh("cube");
}

bool CubeColliderComponent::CubeCollision(ColliderComponent* _otherCollider, glm::vec3& _collisionPoint,
    glm::vec3& _collisionNormal, float& _penetration)
{
    auto otherCube = dynamic_cast<CubeColliderComponent*>(_otherCollider);

    
    
    return true;
}

glm::mat3 CubeColliderComponent::GetMoment() const
{
    if (m_kinematic)
    {
        return glm::mat3(1);
    }
    
    glm::vec3 scale = transform.GetWorldRotation() * transform.GetWorldScale()
                        * conjugate(transform.GetWorldRotation());
    float inertia = (1.f / 6.f) * GetMass();

    glm::mat3 moment = glm::mat3(1);
    moment[0].x = inertia * scale.x * scale.x;
    moment[1].y = inertia * scale.y * scale.y;
    moment[2].z = inertia * scale.z * scale.z;
    
    return moment;
}
