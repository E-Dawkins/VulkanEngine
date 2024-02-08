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
                material->pushConstants.model = GetColliderMatrix();
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
        case CT_CUBE: didCollide = CubeCollision(_otherCollider, contactPt, contactNorm, penetration); break;

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
        m_linearVelocity = glm::vec3(0);
        m_angularVelocity = glm::vec3(0);
        return;
    }
        
    if (m_useGravity)
    {
        m_linearVelocity += g_gravity;
    }
    
    transform.SetWorldPosition(transform.GetWorldPosition() + (m_linearVelocity * _deltaSeconds));

    const glm::quat qOld = transform.GetWorldRotation();
    const glm::quat w = glm::quat(0, m_angularVelocity);
    // Derived formula to apply angular velocity to a known quaternion rotation
    const glm::quat qNew = qOld + (_deltaSeconds / 2.f) * w * qOld;
    transform.SetWorldRotation(normalize(qNew));
}

bool ColliderComponent::SphereCollision(ColliderComponent* _otherCollider, glm::vec3& _collisionPoint,
                                        glm::vec3& _collisionNormal, float& _penetration)
{
    std::cout << m_ctStrings[CT_SPHERE] << " collision not implemented for " << m_ctStrings[m_type] << std::endl;

    return false;
}

bool ColliderComponent::CubeCollision(ColliderComponent* _otherCollider, glm::vec3& _collisionPoint,
                                        glm::vec3& _collisionNormal, float& _penetration)
{
    std::cout << m_ctStrings[CT_CUBE] << " collision not implemented for " << m_ctStrings[m_type] << std::endl;

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

    // Helpful variables for later calculations
    glm::vec3 rA = _contactPt - transform.GetWorldPosition();
    glm::vec3 rB = _contactPt - _other->transform.GetWorldPosition();

    glm::vec3 vA = m_linearVelocity + cross(m_angularVelocity, rA);
    glm::vec3 vB = _other->m_linearVelocity + cross(_other->m_angularVelocity, rB);

    glm::vec3 dv = vB - vA;

    // Combined elasticity of the collision
    float e = m_elasticity * _other->m_elasticity;
    float eTerm = dot(normal,
        m_linearVelocity + cross(rA, m_angularVelocity)
        - _other->m_linearVelocity - cross(rB, _other->m_angularVelocity));
    float combinedElasticity = e * eTerm;

    // Collision Response - the generic collision response, i.e. minimal angular velocity, mostly linear velocity.
    float constraintMass =
        ((1.f / GetMass()) + (1.f / _other->GetMass())) +
            dot(normal,
                cross(inverse(GetMoment())
                * cross(rA, normal), rA) +
                cross(inverse(_other->GetMoment())
                * cross(rB, normal), rB)
            );

    if (constraintMass > 0.f)
    {
        float jn = max(-dot(dv, normal) + combinedElasticity, 0.f);
        jn /= constraintMass;
        
        m_linearVelocity -= normal * (jn / GetMass());
        _other->m_linearVelocity += normal * (jn / _other->GetMass());
        
        m_angularVelocity -= inverse(GetMoment()) * cross(rA, normal * jn);
        _other->m_angularVelocity += inverse(_other->GetMoment()) * cross(rB, normal * jn);
    }

    // Friction - this is where most of the angular velocity gets applied based on the friction between the objects.
    glm::vec3 tangent = dv - normal * dot(dv, normal);
    float tanLength = length(tangent);

    if (tanLength > 0)
    {
        tangent /= tanLength;

        float frictionalMass = ((1.f / GetMass()) + (1.f / _other->GetMass())) +
            dot(tangent,
                cross(inverse(GetMoment()) * cross(rA, tangent), rA) +
                cross(inverse(_other->GetMoment()) * cross(rB, tangent), rB)
            );

        if (frictionalMass > 0.f)
        {
            float frictionCoef = m_friction * _other->m_friction;
            float jt = -dot(dv, tangent) * frictionCoef;

            jt /= frictionalMass;

            m_linearVelocity -= tangent * (jt / GetMass());
            _other->m_linearVelocity += tangent * (jt / _other->GetMass());

            m_angularVelocity -= inverse(GetMoment()) * cross(rA, tangent * jt);
            _other->m_angularVelocity += inverse(_other->GetMoment()) * cross(rB, tangent * jt);
        }
    }
    
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
