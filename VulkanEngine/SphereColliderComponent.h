#pragma once
#include "ColliderComponent.h"

class SphereColliderComponent : public ColliderComponent
{
public:
    SphereColliderComponent();
    
    bool SphereCollision(ColliderComponent* _otherCollider) override;
    
    glm::mat3 GetMoment() const override;
    glm::mat4 GetColliderMatrix() override;

    float GetRadius() const
    {
        return MathHelpers::MinComponent(transform.GetWorldScale());
    }
};
