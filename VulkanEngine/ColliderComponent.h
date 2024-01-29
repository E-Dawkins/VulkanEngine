#pragma once
#include <map>

#include "SceneComponent.h"

class ColliderComponent : public SceneComponent
{
public:
    ColliderComponent();

    void TickComponent(float _deltaSeconds) override;
    
    void ResolveCollision(ColliderComponent* _otherCollider);

    /* Collision Resolvers */
    virtual bool SphereCollision(ColliderComponent* _otherCollider,float& _collisionVelocity, glm::vec3& _collisionPoint);

    /* Setters */
    void SetUseGravity(const bool _useGravity) { m_useGravity = _useGravity; }
    void SetVelocity(const glm::vec3 _velocity) { m_velocity = _velocity; }
    void SetKinematic(const bool _kinematic) { m_kinematic = _kinematic; }

    void ClearPreviousCollisions() { m_previousCollisions.clear(); }
    
protected:
    enum ColliderType
    {
        CT_UNKNOWN,
        CT_SPHERE
    };

    std::map<ColliderType, std::string> m_ctStrings =
    {
        {CT_UNKNOWN, "UNKNOWN"},
        {CT_SPHERE, "SPHERE"}
    };

    ColliderType m_type = CT_UNKNOWN;

    glm::vec3 m_velocity = glm::vec3(0);

    bool m_useGravity = false;
    bool m_kinematic = false;
    
    std::vector<ColliderComponent*> m_previousCollisions;
};
