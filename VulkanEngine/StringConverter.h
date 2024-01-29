#pragma once
namespace StringConverter
{
    static std::string LogVec3(const glm::vec3 _vec3)
    {
        return "{" + std::to_string(_vec3.x) + ", " + std::to_string(_vec3.y) + ", " + std::to_string(_vec3.z) + "}";
    }
}
