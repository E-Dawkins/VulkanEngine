#include "pch.h"
#include "CubeColliderComponent.h"

#include <glm/common.hpp>

CubeColliderComponent::CubeColliderComponent()
{
    m_type = CT_CUBE;

    m_visualizedMesh = Renderer::GetInstance()->GetMesh("cube");
}

bool CubeColliderComponent::CubeCollision(ColliderComponent* _otherCollider)
{
    const auto otherCube = dynamic_cast<CubeColliderComponent*>(_otherCollider);

    glm::vec3 normal(0);
    glm::vec3 contact(0);
    float pen = 0;

    SATImplementation(otherCube, contact, pen, normal);
    otherCube->SATImplementation(this, contact, pen, normal);
    
    if (pen > 0)
    {
        ResolveCollision1(otherCube, contact, normal, pen);
        
        return true;
    }
    
    return false;
}

bool CubeColliderComponent::SATImplementation(CubeColliderComponent* _otherCube, glm::vec3& _contact, float& _pen,
    glm::vec3& _normal) const
{
    // Find min & max vertices in each cardinal direction
    glm::vec3 localContact(0);
    int numLocalContacts = 0;
    
    glm::vec3 box1Min(0);
    glm::vec3 box1Max(0);
    GetMinMax(_otherCube->transform, box1Min, box1Max, localContact, numLocalContacts);
    
    glm::vec3 box2Min(0);
    glm::vec3 box2Max(0);
    _otherCube->GetMinMax(transform, box2Min, box2Max, localContact, numLocalContacts);
    
    if (box1Min.x > box2Max.x || box2Min.x > box1Max.x ||
        box1Min.y > box2Max.y || box2Min.y > box1Max.y ||
        box1Min.z > box2Max.z || box2Min.z > box1Max.z)
    {
        return false;
    }

    if (numLocalContacts == 0)
    {
        return false;
    }

    _pen = 0;
    
    float pen0 = box2Max.x - box1Min.x;
    if (box1Min.x < box2Max.x && (pen0 < _pen || _pen == 0.f))
    {
        _normal = -transform.GetLocalRight();
        _pen = pen0;
    }

    pen0 = box1Max.x - box2Min.x;
    if (box2Min.x < box1Max.x && (pen0 < _pen || _pen == 0.f))
    {
        _normal = transform.GetLocalRight();
        _pen = pen0;
    }

    pen0 = box2Max.y - box1Min.y;
    if (box1Min.y < box2Max.y && (pen0 < _pen || _pen == 0.f))
    {
        _normal = -transform.GetLocalForward();
        _pen = pen0;
    }

    pen0 = box1Max.y - box2Min.y;
    if (box2Min.y < box1Max.y && (pen0 < _pen || _pen == 0.f))
    {
        _normal = transform.GetLocalForward();
        _pen = pen0;
    }

    pen0 = box2Max.z - box1Min.z;
    if (box1Min.z < box2Max.z && (pen0 < _pen || _pen == 0.f))
    {
        _normal = -transform.GetLocalUp();
        _pen = pen0;
    }

    pen0 = box1Max.z - box2Min.z;
    if (box2Min.z < box1Max.z && (pen0 < _pen || _pen == 0.f))
    {
        _normal = transform.GetLocalUp();
        _pen = pen0;
    }
    
    _contact = localContact / static_cast<float>(numLocalContacts);
    
    return true;
}

glm::mat3 CubeColliderComponent::GetMoment() const
{
    if (m_kinematic)
    {
        return glm::mat3(1);
    }

    const glm::vec3 scale = transform.GetWorldRotation() * transform.GetWorldScale()
                        * conjugate(transform.GetWorldRotation());
    const float inertia = (1.f / 6.f) * GetMass();

    auto moment = glm::mat3(1);
    moment[0].x = inertia * scale.x * scale.x;
    moment[1].y = inertia * scale.y * scale.y;
    moment[2].z = inertia * scale.z * scale.z;
    
    return moment;
}

void CubeColliderComponent::GetMinMax(Transform _otherTransform, glm::vec3& _min, glm::vec3& _max, glm::vec3& _localContact, int& _numLocalContacts) const
{
    bool first = true;
    _min = glm::vec3(0);
    _max = glm::vec3(0);

    glm::vec3 extents = abs(transform.GetWorldRotation() * transform.GetWorldScale() * conjugate(transform.GetWorldRotation()));
    glm::vec3 otherExtents = abs(_otherTransform.GetWorldRotation() * _otherTransform.GetWorldScale() * conjugate(_otherTransform.GetWorldRotation()));
    
    for (float x = -otherExtents.x; x <= otherExtents.x; x += otherExtents.x * 2.f)
    {
        for (float y = -otherExtents.y; y <= otherExtents.y; y += otherExtents.y * 2.f)
        {
            for (float z = -otherExtents.z; z <= otherExtents.z; z += otherExtents.z * 2.f)
            {
                const glm::vec3 worldPtPos = _otherTransform.GetWorldPosition()
                    + x * _otherTransform.GetLocalRight()
                    + y * _otherTransform.GetLocalForward()
                    + z * _otherTransform.GetLocalUp();

                if (first || worldPtPos.x < _min.x) _min.x = worldPtPos.x;
                if (first || worldPtPos.x > _max.x) _max.x = worldPtPos.x;
                if (first || worldPtPos.y < _min.y) _min.y = worldPtPos.y;
                if (first || worldPtPos.y > _max.y) _max.y = worldPtPos.y;
                if (first || worldPtPos.z < _min.z) _min.z = worldPtPos.z;
                if (first || worldPtPos.z > _max.z) _max.z = worldPtPos.z;

                glm::vec3 localPtPos = worldPtPos - transform.GetWorldPosition();

                // glm::vec3 localPtPos = glm::vec3(
                //     dot(worldPtPos - transform.GetWorldPosition(), transform.GetLocalRight()),
                //     dot(worldPtPos - transform.GetWorldPosition(), transform.GetLocalForward()),
                //     dot(worldPtPos - transform.GetWorldPosition(), transform.GetLocalUp())
                //     );
                
                if (localPtPos.x >= -extents.x && localPtPos.x <= extents.x &&
                    localPtPos.y >= -extents.y && localPtPos.y <= extents.y &&
                    localPtPos.z >= -extents.z && localPtPos.z <= extents.z)
                {
                    _numLocalContacts++;
                    _localContact += worldPtPos;
                }
                
                first = false;
            }
        }
    }
}
