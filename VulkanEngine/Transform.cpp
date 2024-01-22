#include "pch.h"
#include "Transform.h"

Transform::Transform()
{
    position = glm::vec3(0.f);
    eulerAngles = glm::vec3(0.f);
    scale = glm::vec3(1.f);
}

Transform::Transform(glm::vec3 _position, glm::vec3 _eulerAngles, glm::vec3 _scale)
{
    position = _position;
    eulerAngles = _eulerAngles;
    scale = _scale;
}

glm::mat4 Transform::GetMatrix() const
{
    const glm::mat4 positionMatrix = glm::translate(glm::mat4(1.f), position);
    const glm::mat4 rotationMatrix = glm::eulerAngleXYZ(eulerAngles.x, eulerAngles.y, eulerAngles.z);
    const glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.f), scale);
    
    return positionMatrix * rotationMatrix * scaleMatrix;
}
