#pragma once

class Transform
{
public:
    Transform() = default;

    void UpdateTransform();
    
    /* Getters */
    glm::mat4 GetWorldMatrix() const;
    glm::mat4 GetLocalMatrix() const;

    /* Setters */
    void SetParent(Transform* _parent) { m_parent = _parent; }
    
// private:
    glm::vec3 m_worldPosition = glm::vec3(0);
    glm::quat m_worldRotation = glm::quat(1, 0, 0, 0);
    glm::vec3 m_worldScale = glm::vec3(1);
    
    glm::vec3 m_localPosition = glm::vec3(0);
    glm::quat m_localRotation = glm::quat(1, 0, 0, 0);
    glm::vec3 m_localScale = glm::vec3(1);

    Transform* m_parent = nullptr;
};
