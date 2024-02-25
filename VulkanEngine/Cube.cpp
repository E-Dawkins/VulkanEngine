#include "pch.h"
#include "Cube.h"

Cube::Cube(Transform _transform)
{
    m_transform = _transform;
    
    glm::vec3 r = m_transform.GetLocalRight();
    glm::vec3 f = m_transform.GetLocalForward();
    glm::vec3 u = m_transform.GetLocalUp();

    glm::vec3 pos = m_transform.GetWorldPosition();
    glm::vec3 ext = m_transform.GetWorldScale();
    
    m_vertices =
    {
        pos + glm::vec3(-ext.x, -ext.y, -ext.z), // 0
        pos + glm::vec3(-ext.x, -ext.y,  ext.z), // 1
        pos + glm::vec3(-ext.x,  ext.y, -ext.z), // 2
        pos + glm::vec3(-ext.x,  ext.y,  ext.z), // 3
        pos + glm::vec3( ext.x, -ext.y, -ext.z), // 4
        pos + glm::vec3( ext.x, -ext.y,  ext.z), // 5
        pos + glm::vec3( ext.x,  ext.y, -ext.z), // 6
        pos + glm::vec3( ext.x,  ext.y,  ext.z), // 7
    };

    for (glm::vec3& v : m_vertices)
    {
        glm::vec3 v1 = (v - pos);
        // glm::vec3 rv1 = (m_transform.GetWorldRotation() * v1 * conjugate(m_transform.GetWorldRotation()));
        // v = pos + rv1;

        v = pos + glm::vec3(toMat4(m_transform.GetWorldRotation()) * glm::vec4(v1, 0));
    }

    m_faces =
    {
        Face( r, pos + r * ext, {m_vertices[4], m_vertices[5], m_vertices[6], m_vertices[7]}),
        Face(-r, pos - r * ext, {m_vertices[0], m_vertices[1], m_vertices[2], m_vertices[3]}),
        Face( f, pos + f * ext, {m_vertices[2], m_vertices[3], m_vertices[6], m_vertices[7]}),
        Face(-f, pos - f * ext, {m_vertices[0], m_vertices[1], m_vertices[4], m_vertices[5]}),
        Face( u, pos + u * ext, {m_vertices[1], m_vertices[3], m_vertices[5], m_vertices[7]}),
        Face(-u, pos - u * ext, {m_vertices[0], m_vertices[2], m_vertices[4], m_vertices[6]}),
    };
}

ContactInfo Cube::CheckCubeCollision(Cube _cubeA, Cube _cubeB)
{
    float minPen = FLT_MAX;
    glm::vec3 minAxis{};

    auto axes = GetAxes(_cubeA, _cubeB);
    
    for (glm::vec3 testAxis : axes)
    {
        float overlap;
        if (!IntersectsWhenProjected(_cubeA, _cubeB, testAxis, overlap))
        {
            ContactInfo info;
            info.isColliding = false;
            return info;
        }
        else
        {
            if (overlap < minPen)
            {
                minPen = overlap;
                minAxis = testAxis;
            }
        }
    }

    // Definitely colliding at this point, all axes tell us they are intersecting
    ContactInfo info{};
    info.isColliding = true;
    info.normal = minAxis;
    info.penetration = minPen;

    std::vector<glm::vec3> supports = _cubeA.GetSupports(-minAxis);
    Face refFace = _cubeB.GetFaceFromNormal(minAxis);
    
    std::vector<glm::vec3> pts = _cubeB.ClipAgainstOther(supports, refFace);
    
    info.contactPt = MathHelpers::GetAveragePoint(pts);
    
    return info;
}

bool Cube::IntersectsWhenProjected(Cube _cubeA, Cube _cubeB, glm::vec3 _axis, float& _overlap)
{
    glm::vec<3, bool> nan = isnan(_axis);
    bool isNan = nan.x || nan.y || nan.z;
    
    if (_axis == glm::vec3(0) || isNan)
    {
        return true;
    }

    float aMin =  FLT_MAX;
    float aMax = -FLT_MAX;
    float bMin =  FLT_MAX;
    float bMax = -FLT_MAX;

    for (int i = 0; i < 8; i++)
    {
        float aDist = dot(_cubeA.m_vertices[i], _axis);
        if(aDist < aMin) aMin = aDist;
        if(aDist > aMax) aMax = aDist;

        float bDist = dot(_cubeB.m_vertices[i], _axis);
        if(bDist < bMin) bMin = bDist;
        if(bDist > bMax) bMax = bDist;
    }

    float longSpan = max(aMax, bMax) - min(aMin, bMin);
    float sumSpan = aMax - aMin + bMax - bMin;
    
    _overlap = sumSpan - longSpan;

    // if (aMin < bMax)
    // {
    //     _overlap = glm::distance(aMin, bMax);
    // }
    // else
    // {
    //     _overlap = glm::distance(bMin, aMax);
    // }
    
    // <= because we want to include the case where they are touching but not overlapping
    return longSpan <= sumSpan;
}

std::vector<glm::vec3> Cube::GetAxes(Cube _cubeA, Cube _cubeB)
{
    return
    {
        _cubeA.m_transform.GetLocalRight(),
        _cubeA.m_transform.GetLocalForward(),
        _cubeA.m_transform.GetLocalUp(),
        _cubeB.m_transform.GetLocalRight(),
        _cubeB.m_transform.GetLocalForward(),
        _cubeB.m_transform.GetLocalUp(),
        normalize(cross(_cubeA.m_transform.GetLocalRight(), _cubeB.m_transform.GetLocalRight())),
        normalize(cross(_cubeA.m_transform.GetLocalRight(), _cubeB.m_transform.GetLocalForward())),
        normalize(cross(_cubeA.m_transform.GetLocalRight(), _cubeB.m_transform.GetLocalUp())),
        normalize(cross(_cubeA.m_transform.GetLocalForward(), _cubeB.m_transform.GetLocalRight())),
        normalize(cross(_cubeA.m_transform.GetLocalForward(), _cubeB.m_transform.GetLocalForward())),
        normalize(cross(_cubeA.m_transform.GetLocalForward(), _cubeB.m_transform.GetLocalUp())),
        normalize(cross(_cubeA.m_transform.GetLocalUp(), _cubeB.m_transform.GetLocalRight())),
        normalize(cross(_cubeA.m_transform.GetLocalUp(), _cubeB.m_transform.GetLocalForward())),
        normalize(cross(_cubeA.m_transform.GetLocalUp(), _cubeB.m_transform.GetLocalUp())),
    };
}

glm::vec3 Cube::GetSupport(glm::vec3 _axis)
{
    float bestDist = FLT_MIN;
    glm::vec3 support;

    for (glm::vec3 vert : m_vertices)
    {
        const float dotVal = dot(_axis, vert - m_transform.GetWorldPosition());

        if (dotVal > bestDist)
        {
            bestDist = dotVal;
            support = vert;
        }
    }

    return support;
}

std::vector<glm::vec3> Cube::GetSupports(glm::vec3 _axis)
{
    float bestDist = FLT_MIN;
    std::map<float, std::vector<glm::vec3>> points;

    for (glm::vec3 vert : m_vertices)
    {
        const float d = dot(_axis, vert - m_transform.GetWorldPosition());

        if (points.contains(d))
        {
            points.at(d).push_back(vert);
        }
        else
        {
            points.emplace(d, std::vector{vert});
        }

        if (d > bestDist)
        {
            bestDist = d;
        }
    }
    
    return points[bestDist];
}

std::vector<glm::vec3> Cube::ClipAgainstOther(std::vector<glm::vec3> _ptsToClip, Face _refFace)
{
    // Get side planes
    glm::vec3 t1 = MathHelpers::GetTangentToVector(_refFace.normal);
    glm::vec3 t2 = cross(_refFace.normal, t1);
    
    std::vector facesToClipAgainst
    {
        GetFaceFromNormal( t1),
        GetFaceFromNormal(-t1),
        GetFaceFromNormal( t2),
        GetFaceFromNormal(-t2),
        _refFace
    };

    for (Face f : facesToClipAgainst)
    {
        for (int i = _ptsToClip.size() - 1; i >= 0; i--)
        {
            glm::vec3& v = _ptsToClip[i];
        
            float dotVal = dot(f.normal, v - f.origin);
            
            if (dotVal >= 0.f) // wrong side of face, clip it
            {
                v = MathHelpers::ProjectPointOnPlane(f.normal, f.origin, v);
                int a = 4;
            }
        }
    }
    
    return _ptsToClip;
}

Face Cube::GetFaceFromNormal(const glm::vec3 _norm) const
{
    for (Face f : m_faces)
    {
        if (f.normal == _norm)
        {
            return f;
        }
    }

    return Face();
}

std::vector<Face> Cube::GetMostParallel(Face _testFace)
{
    std::vector<Face> output;
    float lastDot = -FLT_MAX;
    
    for (Face f : m_faces)
    {
        float dotVal = dot(-_testFace.normal, f.normal);

        if (dotVal == lastDot || output.empty())
        {
            output.push_back(f);
        }

        if (dotVal > lastDot)
        {
            output.back() = f;
        }
    }

    return output;
}
