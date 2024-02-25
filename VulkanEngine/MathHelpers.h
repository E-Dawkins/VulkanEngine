#pragma once
#include "GlobalConstants.h"

class MathHelpers
{
public:
    static float MinComponent(const glm::vec3 _vec)
    {
        const float minA = min(_vec.x, _vec.y);
        const float minB = min(_vec.y, _vec.z);

        return min(minA, minB);
    }

    static float MinComponentNonZero(glm::vec3 _vec)
    {
        _vec[0] = _vec[0] == 0.f ? FLT_MAX : _vec[0];
        _vec[1] = _vec[1] == 0.f ? FLT_MAX : _vec[1];
        _vec[2] = _vec[2] == 0.f ? FLT_MAX : _vec[2];

        return MinComponent(_vec);
    }

    static float MaxComponent(const glm::vec3 _vec)
    {
        const float maxA = max(_vec.x, _vec.y);
        const float maxB = max(_vec.y, _vec.z);

        return max(maxA, maxB);
    }

    static glm::vec3 ProjectVectorOnAxis(const glm::vec3 _vector, const glm::vec3 _axis)
    {
        return (dot(_vector, _axis) / dot(_axis, _axis)) * _axis;
    }

    static std::vector<glm::vec3> GetCubeCorners(const glm::vec3 _cubePos, const glm::vec3 _cubeExtents)
    {
        return
        {
            _cubePos + glm::vec3(-_cubeExtents.x, -_cubeExtents.y, -_cubeExtents.z),
            _cubePos + glm::vec3(-_cubeExtents.x, -_cubeExtents.y,  _cubeExtents.z),
            _cubePos + glm::vec3(-_cubeExtents.x,  _cubeExtents.y, -_cubeExtents.z),
            _cubePos + glm::vec3(-_cubeExtents.x,  _cubeExtents.y,  _cubeExtents.z),
            _cubePos + glm::vec3( _cubeExtents.x, -_cubeExtents.y, -_cubeExtents.z),
            _cubePos + glm::vec3( _cubeExtents.x, -_cubeExtents.y,  _cubeExtents.z),
            _cubePos + glm::vec3( _cubeExtents.x,  _cubeExtents.y, -_cubeExtents.z),
            _cubePos + glm::vec3( _cubeExtents.x,  _cubeExtents.y,  _cubeExtents.z),
        };
    }

    static glm::vec3 GetTangentToVector(const glm::vec3 _vec)
    {
        const glm::vec3 c1 = cross(_vec, g_rightVector);
        const glm::vec3 c2 = cross(_vec, g_forwardVector);

        if (length(c1) > length(c2))
        {
            return c1;
        }

        return c2;
    }

    static glm::vec3 GetAveragePoint(const std::vector<glm::vec3> _points)
    {
        glm::vec3 sum(0);

        for (const glm::vec3 pt : _points)
        {
            sum += pt;
        }
        
        return sum / static_cast<float>(_points.size());
    }

    static glm::vec3 ProjectPointOnPlane(glm::vec3 _pNormal, glm::vec3 _pOrigin, glm::vec3 _pointToProject)
    {
        return _pointToProject - dot(_pNormal, _pointToProject - _pOrigin) * _pNormal;
    }
};
