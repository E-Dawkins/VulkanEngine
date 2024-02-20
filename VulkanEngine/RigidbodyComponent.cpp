#include "pch.h"
#include "RigidbodyComponent.h"

#include "Material_Base.h"
#include "Mesh.h"
#include "PhysicsSolver.h"

using namespace std;

RigidbodyComponent::RigidbodyComponent() :
    m_useGravity(true), m_isKinematic(false), m_isTrigger(false),
    m_mass(1), m_elasticity(1), m_friction(1), m_linearDrag(0), m_angularDrag(0),
    m_linearVelocity(0), m_angularVelocity(0),
    m_type(CT_SPHERE),
    m_visualizedMesh(nullptr)
{
    
}

void RigidbodyComponent::BeginComponent()
{
    SceneComponent::BeginComponent();
    
    // Set-up mesh and add a draw call for it
    SetupMesh();

    PhysicsSolver::GetInstance()->RegisterObject(this);
}

void RigidbodyComponent::TickComponent(float _deltaSeconds)
{
    SceneComponent::TickComponent(_deltaSeconds);

    if (m_isKinematic)
    {
        m_linearVelocity = glm::vec3(0);
        m_angularVelocity = glm::vec3(0);
        return;
    }

    transform.SetWorldPosition(transform.GetWorldPosition() + (m_linearVelocity * _deltaSeconds));

    if (m_useGravity)
    {
        ApplyForce(g_gravity * _deltaSeconds, glm::vec3(0));
    }

    const glm::quat qOld = transform.GetWorldRotation();
    const glm::quat w = glm::quat(0, m_angularVelocity);
    // Derived formula to apply angular velocity to a known quaternion rotation
    const glm::quat qNew = qOld + (_deltaSeconds / 2.f) * w * qOld;
    transform.SetWorldRotation(normalize(qNew));
}

void RigidbodyComponent::CheckCollision(RigidbodyComponent* _otherRb)
{
    const int functionIndex = GetType() * CT_COUNT + _otherRb->GetType();
    
    glm::vec3 contactPt(0), normal(0);
    float penetration = 0;

    bool result;
    
    switch (functionIndex)
    {
        case 0: result = Sphere2Sphere(_otherRb, contactPt, normal, penetration); break;
        case 1: result = Sphere2Cube(_otherRb, contactPt, normal, penetration); break;
        case 2: result = Cube2Sphere(_otherRb, contactPt, normal, penetration); break;
        case 3: result = Cube2Cube(_otherRb, contactPt, normal, penetration); break;
        default: result = false; break;
    }
    
    if (result)
    {
        ResolveCollision(_otherRb, contactPt, normal, penetration);
    }
}

void RigidbodyComponent::ResolveCollision(RigidbodyComponent* _otherRb, glm::vec3 _contactPt, glm::vec3 _normal, float _penetration)
{
    // Ensure normal is actually normalized
    const glm::vec3 normal = normalize(_normal);

    // Helpful variables for later calculations
    glm::vec3 rA = _contactPt - transform.GetWorldPosition();
    glm::vec3 rB = _contactPt - _otherRb->transform.GetWorldPosition();

    glm::vec3 vA = m_linearVelocity + cross(m_angularVelocity, rA);
    glm::vec3 vB = _otherRb->m_linearVelocity + cross(_otherRb->m_angularVelocity, rB);

    glm::vec3 dv = vB - vA;

    // Combined elasticity of the collision
    float e = m_elasticity * _otherRb->m_elasticity;
    float eTerm = dot(normal,
        m_linearVelocity + cross(rA, m_angularVelocity)
        - _otherRb->m_linearVelocity - cross(rB, _otherRb->m_angularVelocity));
    float combinedElasticity = e * eTerm;

    // Collision Response - the generic collision response, i.e. minimal angular velocity, mostly linear velocity.
    float constraintMass =
        ((1.f / GetMass()) + (1.f / _otherRb->GetMass())) +
            dot(normal,
                cross(inverse(GetMoment())
                * cross(rA, normal), rA) +
                cross(inverse(_otherRb->GetMoment())
                * cross(rB, normal), rB)
            );

    if (constraintMass > 0.f)
    {
        float jn = max(-dot(dv, normal) + combinedElasticity, 0.f);
        jn /= constraintMass;
        
        m_linearVelocity -= normal * (jn / GetMass());
        _otherRb->m_linearVelocity += normal * (jn / _otherRb->GetMass());
        
        m_angularVelocity -= inverse(GetMoment()) * cross(rA, normal * jn);
        _otherRb->m_angularVelocity += inverse(_otherRb->GetMoment()) * cross(rB, normal * jn);
    }

    // Friction - this is where most of the angular velocity gets applied based on the friction between the objects.
    glm::vec3 tangent = dv - normal * dot(dv, normal);
    float tanLength = length(tangent);

    if (tanLength > 0)
    {
        tangent /= tanLength;

        float frictionalMass = ((1.f / GetMass()) + (1.f / _otherRb->GetMass())) +
            dot(tangent,
                cross(inverse(GetMoment()) * cross(rA, tangent), rA) +
                cross(inverse(_otherRb->GetMoment()) * cross(rB, tangent), rB)
            );

        if (frictionalMass > 0.f)
        {
            float frictionCoef = m_friction * _otherRb->m_friction;
            float jt = -dot(dv, tangent) * frictionCoef;

            jt /= frictionalMass;

            m_linearVelocity -= tangent * (jt / GetMass());
            _otherRb->m_linearVelocity += tangent * (jt / _otherRb->GetMass());

            m_angularVelocity -= inverse(GetMoment()) * cross(rA, tangent * jt);
            _otherRb->m_angularVelocity += inverse(_otherRb->GetMoment()) * cross(rB, tangent * jt);
        }
    }

    if (_penetration > 0)
    {
        ApplyContactForces(_otherRb, _normal, _penetration);
    }
}

void RigidbodyComponent::ApplyForce(glm::vec3 _force, glm::vec3 _pos)
{
    m_linearVelocity += _force / GetMass();
    m_angularVelocity += inverse(GetMoment()) * cross(_force, _pos);
}

void RigidbodyComponent::ApplyContactForces(RigidbodyComponent* _otherRb, glm::vec3 _normal, float _penetration)
{
    const float otherMass = _otherRb ? _otherRb->GetMass() : FLT_MAX;
    const float ourMassFactor = otherMass / (GetMass() + otherMass);

    transform.SetWorldPosition(transform.GetWorldPosition() - ourMassFactor * _normal * _penetration);

    if (_otherRb)
    {
        _otherRb->transform.SetWorldPosition(_otherRb->transform.GetWorldPosition() + (1 - ourMassFactor) * _normal * _penetration);
    }
}

void RigidbodyComponent::SetupMesh()
{
    switch(m_type)
    {
        case CT_SPHERE: m_visualizedMesh = Renderer::GetInstance()->GetMesh("sphere"); break;
        case CT_CUBE: m_visualizedMesh = Renderer::GetInstance()->GetMesh("cube"); break;
    }

    if (m_visualizedMesh)
    {
        Renderer::GetInstance()->AddDrawCall([&] (const VkCommandBuffer _buffer)
        {
            if (g_visualizeColliders)
            {
                const auto material = Renderer::GetInstance()->GetMaterial("collider");
                material->pushConstants.model = GetRigidbodyMatrix();
                material->UpdateMaterial();
                material->RenderMaterial(_buffer);
                m_visualizedMesh->DrawMesh(_buffer);
            }
        });
    }
}

bool RigidbodyComponent::Sphere2Sphere(RigidbodyComponent* _otherRb, glm::vec3& _contactPt, glm::vec3& _normal, float& _penetration)
{
    float r1 = MathHelpers::MinComponent(transform.GetWorldScale());
    float r2 = MathHelpers::MinComponent(_otherRb->transform.GetWorldScale());

    glm::vec3 toOther = _otherRb->transform.GetWorldPosition() - transform.GetWorldPosition();
    float dist = length(toOther);

    if (dist > r1 + r2)
    {
        return false;
    }

    _normal = normalize(toOther);
    _contactPt = transform.GetWorldPosition() + _normal * r1;
    _penetration = r1 + r2 - dist;
    
    return true;
}

bool RigidbodyComponent::Sphere2Cube(RigidbodyComponent* _otherRb, glm::vec3& _contactPt, glm::vec3& _normal, float& _penetration)
{
    std::cout << "Sphere2Cube not implemented!" << std::endl;
    return false;
}

bool RigidbodyComponent::Cube2Cube(RigidbodyComponent* _otherRb, glm::vec3& _contactPt, glm::vec3& _normal, float& _penetration)
{
    std::cout << "Cube2Cube not implemented!" << std::endl;
    return false;
}

bool RigidbodyComponent::Cube2Sphere(RigidbodyComponent* _otherRb, glm::vec3& _contactPt, glm::vec3& _normal, float& _penetration)
{
    std::cout << "Cube2Sphere not implemented!" << std::endl;
    return false;
}

glm::mat3 RigidbodyComponent::GetMoment() const
{
    glm::mat3 moment(1);

    if (m_isKinematic)
    {
        return moment;
    }

    switch(m_type)
    {
        case CT_SPHERE: moment *= (2.f / 5.f) * GetMass() * powf(MathHelpers::MinComponent(transform.GetWorldScale()), 2.f); break;
    }
    
    return moment;
}

glm::mat4 RigidbodyComponent::GetRigidbodyMatrix()
{
    // Sphere is the exception, the radius is the minimum of its' scale
    if (m_type == CT_SPHERE)
    {
        Transform temp = transform;
        temp.SetWorldScale(glm::vec3(MathHelpers::MinComponent(temp.GetWorldScale())));
        return temp.GetWorldMatrix();
    }
    
    return transform.GetWorldMatrix();
}
