#pragma once
#include "RigidbodyComponent.h"

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

    void RegisterObject(RigidbodyComponent* _rb);
    void UnRegisterObject(const RigidbodyComponent* _rb);

    void Init();
    
private:
    void UpdateCellId(std::array<int, 3> _cellId);
    void CheckForCollisions(std::array<int, 3> _cellId);

    std::array<int, 3> GetCellId(const RigidbodyComponent* _rb) const;
    
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
    
    std::unordered_map<std::array<int, 3>, std::vector<RigidbodyComponent*>, ArrayHasher> m_objectMap{};

    std::array<int, 3> m_cellSize = {10, 10, 10};

    uint16_t m_colliderCount = 0;
};
