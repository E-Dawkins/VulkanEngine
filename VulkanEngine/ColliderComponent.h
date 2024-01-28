#pragma once
#include "SceneComponent.h"

#include <map>

class ColliderComponent : public SceneComponent
{
public:
    ColliderComponent();
    
    void ResolveCollision(ColliderComponent* _otherCollider);

    /* Collision Resolvers */
    virtual void SphereCollision(ColliderComponent* _otherCollider);
    
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
};
