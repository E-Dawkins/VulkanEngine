#pragma once
#include "ColliderComponent.h"

class CubeColliderComponent : public ColliderComponent
{
public:
    CubeColliderComponent();

    bool CubeCollision(ColliderComponent* _otherCollider, glm::vec3& _collisionPoint, glm::vec3& _collisionNormal, float& _penetration) override;

    glm::mat3 GetMoment() const override;
};
