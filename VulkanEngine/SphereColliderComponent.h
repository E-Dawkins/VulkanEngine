#pragma once

#include "ColliderComponent.h"
#include "SphereColliderComponent.h"

class SphereColliderComponent : public ColliderComponent
{
public:
    SphereColliderComponent();
    
    bool SphereCollision(ColliderComponent* _otherCollider, glm::vec3& _collisionPoint, glm::vec3& _collisionNormal, float& _penetration) override;

    glm::mat3 GetMoment() const override
    {
        return m_kinematic ?
            glm::mat3(1) :
            (2.f / 3.f) * m_mass * m_radius * m_radius * glm::mat3(1);
    }
    
    /* Setters */
    void SetRadius(const float _radius)
    {
        m_radius = _radius;
        transform.SetWorldScale(glm::vec3(m_radius));
    }

protected:
    float m_radius = 1.f;
};
