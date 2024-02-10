#include "pch.h"
#include "CubeColliderComponent.h"

CubeColliderComponent::CubeColliderComponent()
{
    m_type = CT_CUBE;

    m_visualizedMesh = Renderer::GetInstance()->GetMesh("cube");
}

bool CubeColliderComponent::CubeCollision(ColliderComponent* _otherCollider)
{
    const auto otherCube = dynamic_cast<CubeColliderComponent*>(_otherCollider);

    glm::vec3 normal = glm::vec3(0);
    glm::vec3 contact = glm::vec3(0);
    float pen = 0;
    int numContacts = 0;

    CheckCubeCorners(otherCube, contact, numContacts, pen, normal);

    if (otherCube->CheckCubeCorners(this, contact, numContacts, pen, normal))
    {
        normal = -normal;
    }

    if (pen > 0)
    {
        ResolveCollision(otherCube, contact / (float)numContacts, normal, pen);
        
        return true;
    }
    
    return false;
}

bool CubeColliderComponent::CheckCubeCorners(CubeColliderComponent* _otherCube, glm::vec3& _contact, int& _numContacts,
                                             float& _pen, glm::vec3& _edgeNormal)
{
    float minX, maxX, minY, maxY, minZ, maxZ;
    glm::vec3 extents = glm::abs(transform.GetWorldScale());
    glm::vec3 otherExtents = glm::abs(_otherCube->transform.GetWorldScale());
    float boxW = otherExtents.x * 2.f, boxD = otherExtents.y * 2.f, boxH = otherExtents.z * 2.f;
    int numLocalContacts = 0;
    glm::vec3 localContact = glm::vec3(0);
    bool first = true;

    // Loop over all corners of other cube
    for (float x = -otherExtents.x; x < boxW; x += boxW)
    {
        for (float y = -otherExtents.y; y < boxD; y += boxD)
        {
            for (float z = -otherExtents.z; z < boxH; z += boxH)
            {
                // Get point position in world space
                glm::vec3 p = _otherCube->transform.GetWorldPosition()
                    + x * _otherCube->transform.GetLocalRight()
                    + y * _otherCube->transform.GetLocalForward()
                    + z * _otherCube->transform.GetLocalUp();

                // Get position in this cube's space
                // glm::vec3 p0 = p - transform.GetWorldPosition();
                
                glm::vec3 p0 = glm::vec3(
                    dot(p - transform.GetWorldPosition(), transform.GetLocalRight()),
                    dot(p - transform.GetWorldPosition(), transform.GetLocalForward()),
                    dot(p - transform.GetWorldPosition(), transform.GetLocalUp())
                    );

                // Update extents in each cardinal direction in this cube's space, i.e. along the separating axes
                if (first || p0.x < minX) minX = p0.x;
                if (first || p0.x > maxX) maxX = p0.x;
                if (first || p0.y < minY) minY = p0.y;
                if (first || p0.y > maxY) maxY = p0.y;
                if (first || p0.z < minZ) minZ = p0.z;
                if (first || p0.z > maxZ) maxZ = p0.z;

                // If this corner is inside the box, add it to the list of contact points
                if (p0.x >= -extents.x && p0.x <= extents.x &&
                    p0.y >= -extents.y && p0.y <= extents.y &&
                    p0.z >= -extents.z && p0.z <= extents.z)
                {
                    numLocalContacts++;
                    localContact += p0;
                }

                first = false;
            }
        }
    }

    // If we lie entirely to one side of the cube along one
    // axis, we've found a separating axis and can exit
    if (maxX <= -extents.x || minX >= extents.x ||
        maxY <= -extents.y || minY >= extents.y ||
        maxZ <= -extents.z || minZ >= extents.z)
    {
        return false;
    }

    // No contact point, exit
    if (numLocalContacts == 0)
    {
        return false;
    }

    bool res = false;
    _contact += transform.GetWorldPosition() + (localContact.x * transform.GetLocalRight() + localContact.y * transform.GetLocalForward()
        + localContact.z * transform.GetLocalUp()) / (float)numLocalContacts;
    _numContacts++;

    // Find the minimum penetration vector as a penetration amount and normal
    float pen0 = extents.x - minX;
    if (pen0 > 0 && (pen0 < _pen || _pen == 0))
    {
        _edgeNormal = transform.GetLocalRight();
        _pen = pen0;
        res = true;
    }

    pen0 = maxX + extents.x;
    if (pen0 > 0 && (pen0 < _pen || _pen == 0))
    {
        _edgeNormal = -transform.GetLocalRight();
        _pen = pen0;
        res = true;
    }

    pen0 = extents.y - minY;
    if (pen0 > 0 && (pen0 < _pen || _pen == 0))
    {
        _edgeNormal = transform.GetLocalForward();
        _pen = pen0;
        res = true;
    }

    pen0 = maxY + extents.y;
    if (pen0 > 0 && (pen0 < _pen || _pen == 0))
    {
        _edgeNormal = -transform.GetLocalForward();
        _pen = pen0;
        res = true;
    }

    pen0 = extents.z - minZ;
    if (pen0 > 0 && (pen0 < _pen || _pen == 0))
    {
        _edgeNormal = transform.GetLocalUp();
        _pen = pen0;
        res = true;
    }

    pen0 = maxZ + extents.z;
    if (pen0 > 0 && (pen0 < _pen || _pen == 0))
    {
        _edgeNormal = -transform.GetLocalUp();
        _pen = pen0;
        res = true;
    }

    std::cout << "Pen " << _pen << " Norm " << StringConverter::LogVec3(_edgeNormal) << std::endl;

    return res;
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
