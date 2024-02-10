#pragma once
#include <map>

#include "Transform.h"

class MathHelpers
{
public:
    static float MinComponent(const glm::vec3 _vec)
    {
        const float minA = min(_vec.x, _vec.y);
        const float minB = min(_vec.y, _vec.z);

        return min(minA, minB);
    }

    static float MaxComponent(const glm::vec3 _vec)
    {
        const float minA = max(_vec.x, _vec.y);
        const float minB = max(_vec.y, _vec.z);

        return max(minA, minB);
    }
    
    static bool PointInSphere(const glm::vec3 _point, const glm::vec3 _spherePos, const float _sphereRadius)
    {
        return length(_point - _spherePos) <= _sphereRadius;
    }

    static glm::vec3 ClosestPointOnSphere(const glm::vec3 _point, const glm::vec3 _spherePos, const float _sphereRadius)
    {
        const glm::vec3 dirToPoint = normalize(_point - _spherePos);
        return _spherePos + dirToPoint * _sphereRadius;
    }

    static bool PointInCube(const glm::vec3 _point, const Transform& _cubeTransform)
    {
        const glm::vec3 dist = glm::abs(_point - _cubeTransform.GetWorldPosition());
        glm::vec3 extents = glm::abs(_cubeTransform.GetWorldRotation() * _cubeTransform.GetWorldScale()
            * conjugate(_cubeTransform.GetWorldRotation()));
        
        return (dist.x <= extents.x) && (dist.y <= extents.y) && (dist.z <= extents.z);
    }

    static glm::vec3 ClosestPointOnCube(const glm::vec3 _point, const Transform& _cubeTransform)
    {
        const glm::vec3 offsetPt = _point - _cubeTransform.GetWorldPosition();
        const glm::vec3 extents = glm::abs(_cubeTransform.GetWorldScale());
        
        const glm::vec3 closestPt =
        {
            glm::clamp(offsetPt.x, -extents.x, extents.x),
            glm::clamp(offsetPt.y, -extents.y, extents.y),
            glm::clamp(offsetPt.z, -extents.z, extents.z)
        };

        return _cubeTransform.GetWorldPosition() + closestPt;
    }

    static glm::vec3 PerpendicularVector(const glm::vec3 _vector)
    {
        float d1 = dot(_vector, glm::vec3(1, 0, 0));
        float d2 = dot(_vector, glm::vec3(0, 1, 0));
        float d3 = dot(_vector, glm::vec3(0, 0, 1));

        std::map<float, glm::vec3> pairs =
        {
            {d1, cross(_vector, glm::vec3(1, 0, 0))},
            {d2, cross(_vector, glm::vec3(0, 1, 0))},
            {d3, cross(_vector, glm::vec3(0, 0, 1))}
        };

        return pairs[MaxComponent(glm::vec3(d1, d2, d3))];
    }
};
