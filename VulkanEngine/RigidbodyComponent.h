#pragma once
#include "SceneComponent.h"

#include <vector>

enum ColliderType
{
    CT_SPHERE,
    CT_CUBE,
        
    CT_COUNT
};

class RigidbodyComponent final : public SceneComponent
{
public:
    RigidbodyComponent();

    void BeginComponent() override;
    void TickComponent(float _deltaSeconds) override;

    void CheckCollision(RigidbodyComponent* _otherRb);

    /* Getters */
    bool IsUsingGravity() const                 { return m_useGravity; }
    bool IsKinematic() const                    { return m_isKinematic; }
    bool IsTrigger() const                      { return m_isTrigger; }
    float GetMass() const                       { return m_isKinematic ? FLT_MAX : m_mass; }
    float GetElasticity() const                 { return m_elasticity; }
    float GetFriction() const                   { return m_friction; }
    float GetLinearDrag() const                 { return m_linearDrag; }
    float GetAngularDrag() const                { return m_angularDrag; }
    glm::vec3 GetLinearVelocity() const         { return m_linearVelocity; }
    glm::vec3 GetAngularVelocity() const        { return m_angularVelocity; }
    ColliderType GetType() const                { return m_type; }
    
    /* Setters */
    void SetUsingGravity(const bool _useGravity)                    { m_useGravity = _useGravity; }
    void SetIsKinematic(const bool _isKinematic)                    { m_isKinematic = _isKinematic; }
    void SetIsTrigger(const bool _isTrigger)                        { m_isTrigger = _isTrigger; }
    void SetMass(const float _mass)                                 { m_mass = _mass; }
    void SetElasticity(const float _elasticity)                     { m_elasticity = _elasticity; }
    void SetFriction(const float _friction)                         { m_friction = _friction; }
    void SetLinearDrag(const float _linearDrag)                     { m_linearDrag = _linearDrag; }
    void SetAngularDrag(const float _angularDrag)                   { m_angularDrag = _angularDrag; }
    void SetLinearVelocity(const glm::vec3 _linearVelocity)         { m_linearVelocity = _linearVelocity; }
    void SetAngularVelocity(const glm::vec3 _angularVelocity)       { m_angularVelocity = _angularVelocity; }
    void SetType(const ColliderType _type)                          { m_type = _type; }

private:
    void ResolveCollision(RigidbodyComponent* _otherRb, glm::vec3 _contactPt, glm::vec3 _normal, float _penetration);

    void ApplyImpulse(const glm::vec3 _impulse, const glm::vec3 _contactVector);
    void ApplyContactForces(RigidbodyComponent* _otherRb, glm::vec3 _normal, float _penetration);

    void SetupMesh();

    bool Sphere2Sphere(RigidbodyComponent* _otherRb, glm::vec3& _contactPt, glm::vec3& _normal, float& _penetration);
    bool Sphere2Cube(RigidbodyComponent* _otherRb, glm::vec3& _contactPt, glm::vec3& _normal, float& _penetration);
    bool Cube2Cube(RigidbodyComponent* _otherRb, glm::vec3& _contactPt, glm::vec3& _normal, float& _penetration);
    bool Cube2Sphere(RigidbodyComponent* _otherRb, glm::vec3& _contactPt, glm::vec3& _normal, float& _penetration);

    glm::vec3 GetSupportPoint(glm::vec3 _axis, std::vector<glm::vec3> _points);
    void FindAxisLeastPenetration(std::vector<glm::vec3> _possibleAxes, glm::vec3 _posA,
                                  std::vector<glm::vec3> _pointsB, glm::vec3& _normal, float&
                                  _penetration);
    void SATTest(glm::vec3 _axis, std::vector<glm::vec3> _points, float& _minOnAxis, float& _maxOnAxis);
    
    glm::mat3 GetMoment() const;
    glm::mat4 GetRigidbodyMatrix() const;

private:
    bool m_useGravity;
    bool m_isKinematic;
    bool m_isTrigger;
    
    float m_mass;
    float m_elasticity;
    float m_friction;
    float m_linearDrag;
    float m_angularDrag;

    glm::vec3 m_linearVelocity;
    glm::vec3 m_angularVelocity;

    ColliderType m_type;

    std::shared_ptr<Mesh> m_visualizedMesh;
};
