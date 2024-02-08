#pragma once
class MathHelpers
{
public:
    static float MinComponent(const glm::vec3 _vec)
    {
        const float minA = min(_vec.x, _vec.y);
        const float minB = min(_vec.y, _vec.z);

        return min(minA, minB);
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
};
