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
    // Only update the colliders' position / rotation if it is not static
    if (m_kinematic)
    {
        m_velocity = glm::vec3(0);
        m_angularVelocity = glm::vec3(0);
        return;
    }
        
    if (m_useGravity)
    {
        m_velocity += g_gravity;
    }
    
    transform.SetWorldPosition(transform.GetWorldPosition() + (m_velocity * _deltaSeconds));

    const glm::quat qOld = transform.GetWorldRotation();
    const glm::quat w = glm::quat(0, m_angularVelocity);
    // derived formula to apply angular velocity to a known quaternion rotation
    const glm::quat qNew = qOld + (_deltaSeconds / 2.f) * w * qOld;
    transform.SetWorldRotation(normalize(qNew));
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

    // Combined elasticity of the collision
    float e = m_elasticity * _other->m_elasticity;

    // These are just to reduce the size of the mammoth equation below
    glm::vec3 rA = _contactPt - transform.GetWorldPosition();
    glm::vec3 rB = _contactPt - _other->transform.GetWorldPosition();
    glm::vec3 rACrossN = cross(rA, normal);
    glm::vec3 rBCrossN = cross(rB, normal);
    glm::mat3 invTensorA = inverse(GetMoment());
    glm::mat3 invTensorB = inverse(_other->GetMoment());

    /* General Equation for the Impulse Magnitude Along a Collision Normal */
    // Mammoth of an equation, but this gets the impulse magnitude along the collision normal...
    float j = -(1.f + e) * dot(m_velocity - _other->m_velocity, normal) + dot(rACrossN, m_angularVelocity) - dot(rBCrossN, _other->m_angularVelocity) /
        ((1.f / GetMass()) + (1.f / _other->GetMass()) + dot(rACrossN, (invTensorA * rACrossN)) + dot(rBCrossN, invTensorB * rBCrossN));

    // ...so to get the actual force, we multiply the previously calculated magnitude by the collision normal.
    glm::vec3 force = j * normal;

    // Delta (change) in velocity, and the extra tangential velocity created by 
    // the fact that the point is rotating around the object's centre. (I think?)
    glm::vec<3, float> deltaVel = _other->m_velocity - m_velocity;
    glm::vec3 tangent = normalize(deltaVel - normal * dot(deltaVel, normal));

    // Apply the correct linear and angular velocities
    m_velocity += force / GetMass();
    _other->m_velocity -= force / GetMass();
    
    m_angularVelocity += inverse(GetMoment()) * cross(rA, tangent * force);
    _other->m_angularVelocity -= inverse(_other->GetMoment()) * cross(rB, tangent * force);
    
    // Colliders are penetrating, so apply contact forces to separate them
    if (_penetration > 0)
    {
        ApplyContactForces(_other, _ptNormal, _penetration);
    }
}

void ColliderComponent::ApplyContactForces(ColliderComponent* _other, glm::vec3 _ptNormal, float _penetration)
{
    const float otherMass = _other ? _other->GetMass() : FLT_MAX;
    const float ourMassFactor = otherMass / (GetMass() + otherMass);

    transform.SetWorldPosition(transform.GetWorldPosition() - ourMassFactor * _ptNormal * _penetration);

    if (_other)
    {
        _other->transform.SetWorldPosition(_other->transform.GetWorldPosition() + (1 - ourMassFactor) * _ptNormal * _penetration);
    }
}
