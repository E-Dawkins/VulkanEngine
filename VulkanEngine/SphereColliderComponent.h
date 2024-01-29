#pragma once
#include "ColliderComponent.h"

class SphereColliderComponent : public ColliderComponent
{
public:
    SphereColliderComponent();
    
    bool SphereCollision(ColliderComponent* _otherCollider, float& _collisionVelocity, glm::vec3& _collisionPoint) override;

    /* Setters */
    void SetRadius(const float _radius) { m_radius = _radius; }

protected:
    float m_radius = 1.f;
};
