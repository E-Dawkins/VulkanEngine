#pragma once
#include <vector>

#include "ColliderComponent.h"
#include "CubeColliderComponent.h"

class CubeColliderComponent : public ColliderComponent
{
public:
    CubeColliderComponent();

    bool CubeCollision(ColliderComponent* _otherCollider) override;
    bool SATImplementation(CubeColliderComponent* _otherCube, glm::vec3& _contact, float& _pen, glm::vec3& _normal) const;
    
    glm::mat3 GetMoment() const override;

private:
    void GetMinMax(Transform _otherTransform, glm::vec3& _min, glm::vec3& _max, glm::vec3& _localContact, int& _numLocalContacts) const;
};
