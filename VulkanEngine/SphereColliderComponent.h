#pragma once
#include "ColliderComponent.h"

class SphereColliderComponent : public ColliderComponent
{
public:
    SphereColliderComponent();
    
    void SphereCollision(ColliderComponent* _otherCollider) override;

    /* Setters */
    void SetRadius(const float _radius) { m_radius = _radius; }

protected:
    float m_radius = 1.f;
};
