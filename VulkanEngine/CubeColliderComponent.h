#pragma once
#include "ColliderComponent.h"
#include "CubeColliderComponent.h"

class CubeColliderComponent : public ColliderComponent
{
public:
    CubeColliderComponent();

    bool CubeCollision(ColliderComponent* _otherCollider) override;
    bool CheckCubeCorners(::CubeColliderComponent* _otherCube, glm::vec3& _contact, int& _numContacts, float& _pen, glm::vec3& _edgeNormal);
    
    glm::mat3 GetMoment() const override;
};
