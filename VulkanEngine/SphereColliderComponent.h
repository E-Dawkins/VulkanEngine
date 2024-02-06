#pragma once

#include "ColliderComponent.h"
#include "SphereColliderComponent.h"

class SphereColliderComponent : public ColliderComponent
{
public:
    SphereColliderComponent();
    
    bool SphereCollision(ColliderComponent* _otherCollider, glm::vec3& _collisionPoint, glm::vec3& _collisionNormal, float& _penetration) override;

    glm::mat3 GetMoment() const override;
};
