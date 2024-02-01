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
    void UpdatePhysicsBodies(float _deltaSeconds, std::array<int, 3> _cellId);
    void CheckForCollisions(std::array<int, 3> _cellId);

    std::array<int, 3> GetCellId(const ColliderComponent* _collider) const;
    
private:
    inline static PhysicsSolver* m_instance = nullptr;

    struct ArrayHasher {
        std::size_t operator()(const std::array<int, 3>& a) const {
            std::size_t h = 0;

            for (auto e : a) {
                h ^= std::hash<int>{}(e)  + 0x9e3779b9 + (h << 6) + (h >> 2); 
            }
            return h;
        }   
    };
    
    std::unordered_map<std::array<int, 3>, std::vector<ColliderComponent*>, ArrayHasher> m_colliderMap{};

    std::array<int, 3> m_cellSize = {3, 3, 3};
};
