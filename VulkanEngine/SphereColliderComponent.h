#pragma once
#include "ColliderComponent.h"

class SphereColliderComponent : public ColliderComponent
{
public:
    SphereColliderComponent();
    
    bool SphereCollision(ColliderComponent* _otherCollider, glm::vec3& _collisionPoint, glm::vec3& _collisionNormal, float& _penetration) override;
    
    glm::mat3 GetMoment() const override;
    glm::mat4 GetColliderMatrix() override;

    float GetRadius() const
    {
        return MathHelpers::MinComponent(transform.GetWorldScale());
    }
};
