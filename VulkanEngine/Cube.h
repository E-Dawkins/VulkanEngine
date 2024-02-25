#pragma once
#include "Cube.h"
#include "Transform.h"

struct Face
{
    glm::vec3 normal;
    glm::vec3 origin;
    std::vector<glm::vec3> verts;

    Face() = default;
    
    Face(const glm::vec3 _n, const glm::vec3 _o, std::vector<glm::vec3> _verts) :
        normal(_n), origin(_o), verts(_verts) {}

    bool operator == (const Face& _f) const
    {
        return normal == _f.normal && origin == _f.origin && _f.verts == verts;
    }

    bool operator != (const Face& _f) const
    {
        return !(*this == _f);
    }
};

struct ContactInfo
{
    glm::vec3 normal;
    glm::vec3 contactPt;
    float penetration;
    bool isColliding;
};

class Cube
{
public:
    Cube(Transform _transform);

    static ContactInfo CheckCubeCollision(Cube _cubeA, Cube _cubeB);

    static bool IntersectsWhenProjected(Cube _cubeA, Cube _cubeB, glm::vec3 _axis, float& _overlap);
    static std::vector<glm::vec3> GetAxes(Cube _cubeA, Cube _cubeB);
    
    glm::vec3 GetSupport(glm::vec3 _axis);
    std::vector<glm::vec3> GetSupports(glm::vec3 _axis);

    std::vector<glm::vec3> ClipAgainstOther(std::vector<glm::vec3> _ptsToClip, Face _refFace);
    Face GetFaceFromNormal(glm::vec3 _norm) const;
    std::vector<Face> GetMostParallel(Face _testFace);

private:
    Transform m_transform;
    
    std::vector<Face> m_faces;
    std::vector<glm::vec3> m_vertices;
};
