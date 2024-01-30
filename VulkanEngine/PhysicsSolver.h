#pragma once
#include "ColliderComponent.h"

class PhysicsSolver
{
public:
    PhysicsSolver() {}
    
    static PhysicsSolver* GetInstance()
    {
        if (m_instance == nullptr)
        {
            m_instance = new PhysicsSolver();
        }

        return m_instance;
    }

    void RegisterCollider(ColliderComponent* _collider);
    void UnRegisterCollider(const ColliderComponent* _collider);

    void Init();
    
private:
    void UpdatePhysicsBodies(float _deltaSeconds) const;
    void CheckForCollisions() const;
    
private:
    inline static PhysicsSolver* m_instance = nullptr;
    
    std::vector<ColliderComponent*> m_colliders{};
};
