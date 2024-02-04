#pragma once
namespace StringConverter
{
    static std::string LogVec3(const glm::vec3 _vec3)
    {
        return "{" + std::to_string(_vec3.x) + ", " + std::to_string(_vec3.y) + ", " + std::to_string(_vec3.z) + "}";
    }

    static std::string LogQuat(const glm::quat _quat)
    {
        return "{" + std::to_string(_quat.x) + ", " + std::to_string(_quat.y) + ", " + std::to_string(_quat.z) + ", " +std::to_string(_quat.w) + "}";
    }
}
