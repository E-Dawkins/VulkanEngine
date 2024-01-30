#pragma once

class Transform
{
public:
    Transform() = default;

    void UpdateTransform();
    
    /* Getters */
    glm::mat4 GetWorldMatrix() const;
    glm::mat4 GetLocalMatrix() const;
    glm::vec3 GetWorldPosition() const;
    glm::quat GetWorldRotation() const;
    glm::vec3 GetWorldScale() const;
    glm::vec3 GetLocalPosition() const;
    glm::quat GetLocalRotation() const;
    glm::vec3 GetLocalScale() const;

    /* Setters */
    void SetParent(Transform* _parent);
    void SetWorldPosition(glm::vec3 _worldPos);
    void SetWorldRotation(glm::quat _worldRot);
    void SetWorldScale(glm::vec3 _worldScale);
    void SetLocalPosition(glm::vec3 _localPos);
    void SetLocalRotation(glm::quat _localRot);
    void SetLocalScale(glm::vec3 _localScale);
    
private:
    glm::vec3 m_worldPosition = glm::vec3(0);
    glm::quat m_worldRotation = glm::quat(1, 0, 0, 0);
    glm::vec3 m_worldScale = glm::vec3(1);
    
    glm::vec3 m_localPosition = glm::vec3(0);
    glm::quat m_localRotation = glm::quat(1, 0, 0, 0);
    glm::vec3 m_localScale = glm::vec3(1);

    Transform* m_parent = nullptr;
};
