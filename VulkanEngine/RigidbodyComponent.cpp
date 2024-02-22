#include "pch.h"
#include "RigidbodyComponent.h"

#include <vector>

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
        m_linearVelocity += (g_gravity * _deltaSeconds) / GetMass();
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
                cross(cross(rA, normal), rA) / GetMoment() +
                cross(cross(rB, normal), rB) / _otherRb->GetMoment()
            );

    if (constraintMass > 0.f)
    {
        float jn = max(-dot(dv, normal) + combinedElasticity, 0.f);
        jn /= constraintMass;

        ApplyImpulse(-jn * normal, rA);
        _otherRb->ApplyImpulse(jn * normal, rB);
    }

    // Friction - this is where most of the angular velocity gets applied based on the friction between the objects.
    // glm::vec3 tangent = dv - normal * dot(dv, normal);
    glm::vec3 tangent = MathHelpers::GetTangentToVector(normal);
    float tanLength = length(tangent);

    if (tanLength > 0)
    {
        tangent /= tanLength;

        float frictionalMass = ((1.f / GetMass()) + (1.f / _otherRb->GetMass())) +
            dot(tangent,
                cross(cross(rA, tangent), rA) / GetMoment() +
                cross(cross(rB, tangent), rB) / _otherRb->GetMoment()
            );

        if (frictionalMass > 0.f)
        {
            float frictionCoef = m_friction * _otherRb->m_friction;
            float jt = -dot(dv, tangent) * frictionCoef;

            jt /= frictionalMass;

            ApplyImpulse(-jt * tangent, rA);
            _otherRb->ApplyImpulse(jt * tangent, rB);
        }
    }

    if (_penetration > 0)
    {
        ApplyContactForces(_otherRb, _normal, _penetration);
    }
}

void RigidbodyComponent::ApplyImpulse(const glm::vec3 _impulse, const glm::vec3 _contactVector)
{
    m_linearVelocity += _impulse / GetMass();
    m_angularVelocity += cross(_contactVector, _impulse) / GetMoment();
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
    const float r1 = MathHelpers::MinComponent(transform.GetWorldScale());
    const float r2 = MathHelpers::MinComponent(_otherRb->transform.GetWorldScale());

    const glm::vec3 toOther = _otherRb->transform.GetWorldPosition() - transform.GetWorldPosition();
    const float dist = length(toOther);

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
    const glm::vec3 cube1Extents = transform.GetWorldRotation() * transform.GetWorldScale() * conjugate(transform.GetWorldRotation());
    const glm::vec3 cube2Extents = _otherRb->transform.GetWorldRotation() * _otherRb->transform.GetWorldScale() * conjugate(_otherRb->transform.GetWorldRotation());
    
    const std::vector<glm::vec3> cube1Corners = MathHelpers::GetCubeCorners(transform.GetWorldPosition(), cube1Extents);
    const std::vector<glm::vec3> cube2Corners = MathHelpers::GetCubeCorners(_otherRb->transform.GetWorldPosition(), cube2Extents);

    const std::vector normals =
    {
        transform.GetLocalRight(),
        transform.GetLocalForward(),
        transform.GetLocalUp(),
        _otherRb->transform.GetLocalRight(),
        _otherRb->transform.GetLocalForward(),
        _otherRb->transform.GetLocalUp(),
        normalize(cross(transform.GetLocalRight(), _otherRb->transform.GetLocalRight())),
        normalize(cross(transform.GetLocalRight(), _otherRb->transform.GetLocalForward())),
        normalize(cross(transform.GetLocalRight(), _otherRb->transform.GetLocalUp())),
        normalize(cross(transform.GetLocalForward(), _otherRb->transform.GetLocalRight())),
        normalize(cross(transform.GetLocalForward(), _otherRb->transform.GetLocalForward())),
        normalize(cross(transform.GetLocalForward(), _otherRb->transform.GetLocalUp())),
        normalize(cross(transform.GetLocalUp(), _otherRb->transform.GetLocalRight())),
        normalize(cross(transform.GetLocalUp(), _otherRb->transform.GetLocalForward())),
        normalize(cross(transform.GetLocalUp(), _otherRb->transform.GetLocalUp()))
    };
    
    FindAxisLeastPenetration(normals, transform.GetWorldPosition(), cube2Corners, _normal, _penetration);
    _penetration = length(_normal * cube1Extents) - _penetration;

    if (_penetration <= 0.f)
    {
        return false;
    }

    glm::vec3 pt1 = transform.GetWorldPosition() + _normal * _penetration;
    glm::vec3 pt2 = _otherRb->transform.GetWorldPosition() - _normal * _penetration;
    _contactPt = (pt1 + pt2) / 2.f;
    
    return true;
}

bool RigidbodyComponent::Cube2Sphere(RigidbodyComponent* _otherRb, glm::vec3& _contactPt, glm::vec3& _normal, float& _penetration)
{
    std::cout << "Cube2Sphere not implemented!" << std::endl;
    return false;
}

glm::vec3 RigidbodyComponent::GetSupportPoint(glm::vec3 _axis, std::vector<glm::vec3> _points)
{
    float bestProjection = -FLT_MAX;
    glm::vec3 bestVertex;

    for (glm::vec3 pt : _points)
    {
        const float dotVal = dot(pt, _axis);
        if (dotVal > bestProjection)
        {
            bestVertex = pt;
            bestProjection = dotVal;
        }
    }

    return bestVertex;
}

void RigidbodyComponent::FindAxisLeastPenetration(std::vector<glm::vec3> _possibleAxes, glm::vec3 _posA,
                                                  std::vector<glm::vec3> _pointsB, glm::vec3& _normal,
                                                  float& _penetration)
{
    _penetration = -FLT_MAX;

    for (int i = 0; i < _possibleAxes.size(); i++)
    {
        glm::vec3 n = _possibleAxes[i];
        
        glm::vec3 s = GetSupportPoint(-n, _pointsB);

        const float d = dot(n, s - _posA);

        if (d > _penetration)
        {
            _penetration = d;
            _normal = n;
        }
    }
}

void RigidbodyComponent::SATTest(const glm::vec3 _axis, const std::vector<glm::vec3> _points, float& _minOnAxis,
                                 float& _maxOnAxis)
{
    _minOnAxis = FLT_MAX;
    _maxOnAxis = -FLT_MAX;

    for (glm::vec3 pt : _points)
    {
        const float dotVal = dot(pt, _axis);
        if (dotVal < _minOnAxis) _minOnAxis = dotVal;
        if (dotVal > _maxOnAxis) _maxOnAxis = dotVal;
    }
}

glm::mat3 RigidbodyComponent::GetMoment() const
{
    if (m_isKinematic)
    {
        return glm::mat3(1);
    }

    if (m_type == CT_SPHERE)
    {
        const float radius = MathHelpers::MinComponent(transform.GetWorldScale());
        return glm::mat3(1) * (2.f / 5.f) * GetMass() * powf(radius, 2.f);
    }

    if (m_type == CT_CUBE)
    {
        const glm::vec3 extents = transform.GetWorldRotation() * transform.GetWorldScale()
                        * conjugate(transform.GetWorldRotation());
        const float inertia = (1.f / 12.f) * GetMass();

        glm::mat3 moment(1);
        moment[0].x = inertia * (powf(extents.y, 2.f) + powf(extents.z, 2.f));
        moment[1].y = inertia * (powf(extents.x, 2.f) + powf(extents.z, 2.f));
        moment[2].z = inertia * (powf(extents.x, 2.f) + powf(extents.y, 2.f));
        
        return moment;
    }

    return glm::mat3(1); // should never get here
}

glm::mat4 RigidbodyComponent::GetRigidbodyMatrix() const
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
