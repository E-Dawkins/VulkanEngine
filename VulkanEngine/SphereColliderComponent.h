#pragma once

#include "ColliderComponent.h"
#include "SphereColliderComponent.h"

class SphereColliderComponent : public ColliderComponent
{
public:
    SphereColliderComponent();
    
    bool SphereCollision(ColliderComponent* _otherCollider, glm::vec3& _collisionPoint, glm::vec3& _collisionNormal, float& _penetration) override;
    
    /* Setters */
    void SetRadius(const float _radius)
    {
        m_radius = _radius;
        transform.SetWorldScale(glm::vec3(m_radius));
    }

protected:
    float m_radius = 1.f;
};
