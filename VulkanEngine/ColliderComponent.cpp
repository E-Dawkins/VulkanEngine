#include "pch.h"
#include "ColliderComponent.h"

#include "Material_Base.h"
#include "Mesh.h"
#include "PhysicsSolver.h"

void ColliderComponent::BeginComponent()
{
    SceneComponent::BeginComponent();

    PhysicsSolver::GetInstance()->RegisterCollider(this);

    // Set-up visualized mesh if it has been set
    if (m_visualizedMesh)
    {
        Renderer::GetInstance()->AddDrawCall([&] (const VkCommandBuffer _buffer)
        {
            if (g_visualizeColliders)
            {
                const auto material = Renderer::GetInstance()->GetMaterial("collider");
                material->pushConstants.model = transform.GetWorldMatrix();
                material->UpdateMaterial();
                material->RenderMaterial(_buffer);
                m_visualizedMesh->DrawMesh(_buffer);
            }
        });
    }
}

void ColliderComponent::CheckForCollision(ColliderComponent* _otherCollider)
{
    // If we have already collided with the other collider, skip calculations
    if (std::ranges::find(m_previousCollisions, _otherCollider) != m_previousCollisions.end())
    {
        return;
    }
    
    glm::vec3 contactPt;
    glm::vec3 contactNorm;
    float penetration = 0.f;
    bool didCollide = false;
    
    // Determine which collision function to use, based on the other colliders' type
    switch (_otherCollider->m_type)
    {
        case CT_SPHERE: didCollide = SphereCollision(_otherCollider, contactPt, contactNorm, penetration); break;

        case CT_UNKNOWN: std::cout << "Unknown collider type!" << std::endl;
    }

    // If the colliders did collide, then resolve the collision
    if (didCollide)
    {
        ResolveCollision(_otherCollider, contactPt, contactNorm, penetration);
    }
}

void ColliderComponent::UpdateCollider(float _deltaSeconds)
{
    // If the collider can't move, never update its' position or cell id
    if (m_kinematic)
    {
        m_velocity = glm::vec3(0);
        return;
    }
        
    if (m_useGravity)
    {
        ApplyForce(g_gravity, glm::vec3(0));
    }
    
    transform.SetWorldPosition(transform.GetWorldPosition() + (m_velocity * _deltaSeconds));
    transform.SetWorldRotation(transform.GetWorldRotation() * glm::quat(m_angularVelocity));
}

bool ColliderComponent::SphereCollision(ColliderComponent* _otherCollider, glm::vec3& _collisionPoint,
                                        glm::vec3& _collisionNormal, float& _penetration)
{
    std::cout << m_ctStrings[CT_SPHERE] << " collision not implemented for " << m_ctStrings[m_type] << std::endl;

    return false;
}

void ColliderComponent::ResolveCollision(ColliderComponent* _other, glm::vec3 _contactPt,
                                         glm::vec3 _ptNormal, float _penetration)
{
    // Register that these two objects have collided this frame
    m_previousCollisions.push_back(_other);
    _other->m_previousCollisions.push_back(this);

    // Ensure normal is actually normalized
    const glm::vec3 normal = normalize(_ptNormal);

    float e = (m_elasticity + _other->m_elasticity) * 0.5f;

    glm::vec3 rA = _contactPt - transform.GetWorldPosition();
    glm::vec3 rB = _contactPt - _other->transform.GetWorldPosition();
    glm::vec3 rACrossN = cross(rA, normal);
    glm::vec3 rBCrossN = cross(rB, normal);
    glm::mat3 invTensorA = inverse(GetMoment());
    glm::mat3 invTensorB = inverse(_other->GetMoment());

    float j = -(1.f + e) * dot(m_velocity - _other->m_velocity, normal) + dot(rACrossN, m_angularVelocity) - dot(rBCrossN, _other->m_angularVelocity) /
        ((1.f / GetMass()) + (1.f / _other->GetMass()) + dot(rACrossN, (invTensorA * rACrossN)) + dot(rBCrossN, invTensorB * rBCrossN));

    glm::vec3 force = j * normal;

    ApplyForce(force, _contactPt - transform.GetWorldPosition());
    _other->ApplyForce(-force, _contactPt - _other->transform.GetWorldPosition());
    
    // Colliders are penetrating, so apply contact forces to separate the two colliders
    if (_penetration > 0)
    {
        ApplyContactForces(_other, _ptNormal, _penetration);
    }
}

void ColliderComponent::ApplyForce(glm::vec3 _force, glm::vec3 _pos)
{
    m_velocity += _force / GetMass();
    m_angularVelocity += degrees(inverse(GetMoment()) * cross(_force, _pos));
}

void ColliderComponent::ApplyContactForces(ColliderComponent* _other, glm::vec3 _ptNormal, float _penetration)
{
    const float otherMass = _other ? _other->GetMass() : INT_MAX;
    const float ourMassFactor = otherMass / (GetMass() + otherMass);

    transform.SetWorldPosition(transform.GetWorldPosition() - ourMassFactor * _ptNormal * _penetration);

    if (_other)
    {
        _other->transform.SetWorldPosition(_other->transform.GetWorldPosition() + (1 - ourMassFactor) * _ptNormal * _penetration);
    }
}
