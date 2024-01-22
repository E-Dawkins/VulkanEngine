#pragma once

class Transform
{
public:
    Transform();
    Transform(glm::vec3 _position, glm::vec3 _eulerAngles, glm::vec3 _scale);

    glm::mat4 GetMatrix() const;
    
public:
    glm::vec3 position;
    glm::vec3 eulerAngles;
    glm::vec3 scale;
    
private:
    glm::mat4 m_matrix = glm::mat4(1.f);
};
