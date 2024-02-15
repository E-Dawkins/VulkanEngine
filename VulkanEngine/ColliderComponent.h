﻿#pragma once
#include <map>

#include "ColliderComponent.h"
#include "SceneComponent.h"

class ColliderComponent : public SceneComponent
{
public:
    void BeginComponent() override;
    
    void CheckForCollision(ColliderComponent* _otherCollider);
    void UpdateCollider(float _deltaSeconds);
    void ClearPreviousCollisions() { m_previousCollisions.clear(); }

    /* Collision Resolvers */
    virtual bool SphereCollision(ColliderComponent* _otherCollider);
    virtual bool CubeCollision(ColliderComponent* _otherCollider);

    /* Setters */
    void SetUseGravity(const bool _useGravity)      { m_useGravity = _useGravity; }
    void SetVelocity(const glm::vec3 _velocity)     { m_linearVelocity = _velocity; }
    void SetKinematic(const bool _kinematic)        { m_kinematic = _kinematic; }
    void SetMass(const float _mass)                 { m_mass = _mass; }
    void SetElasticity(const float _elasticity)     { m_elasticity = _elasticity; }
    void SetFriction(const float _friction)         { m_friction = _friction; }
    
    /* Getters */
    float GetMass() const                                   { return m_kinematic ? FLT_MAX : m_mass; }
    virtual glm::mat3 GetMoment() const                     { return glm::mat3(1); }

protected:
    void ResolveCollision(ColliderComponent* _other, glm::vec3 _contact, glm::vec3 _collisionNormal, float _pen);
    void ResolveCollision1(ColliderComponent* _other, glm::vec3 _contact, glm::vec3 _collisionNormal, float _pen);
    void ApplyForce(const glm::vec3 _force, const glm::vec3 _pos);
    void ApplyContactForces(ColliderComponent* _other, glm::vec3 _ptNormal, float _penetration);
    
    virtual glm::mat4 GetColliderMatrix() { return transform.GetWorldMatrix(); }
    
protected:
    friend class PhysicsSolver;
    
    enum ColliderType
    {
        CT_UNKNOWN,
        CT_SPHERE,
        CT_CUBE
    };

    std::map<ColliderType, std::string> m_ctStrings =
    {
        {CT_UNKNOWN, "UNKNOWN"},
        {CT_SPHERE, "SPHERE"},
        {CT_CUBE, "CUBE"}
    };

    ColliderType m_type = CT_UNKNOWN;
    
    glm::vec3 m_linearVelocity = glm::vec3(0);
    glm::vec3 m_angularVelocity = glm::vec3(0);

    float m_mass = 1.f;
    float m_elasticity = 1.f;
    float m_friction = 1.f;

    bool m_useGravity = false;
    bool m_kinematic = false;
    
    std::vector<ColliderComponent*> m_previousCollisions;

    std::shared_ptr<Mesh> m_visualizedMesh;
};
