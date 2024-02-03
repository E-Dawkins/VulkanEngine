#include "pch.h"
#include "PhysicsSolver.h"

#include <ranges>

void PhysicsSolver::RegisterCollider(ColliderComponent* _collider)
{
    // Get cell index for the new collider
    const auto cellId = GetCellId(_collider);
    
    // Check if collider is already registered
    const auto begin = m_colliderMap[cellId].begin();
    const auto end = m_colliderMap[cellId].end();

    const auto itr = std::find(begin, end, _collider);
    
    if (itr == end) // collider is not already registered
    {
        m_colliderMap[cellId].push_back(_collider);
        m_colliderCount++;
    }
}

void PhysicsSolver::UnRegisterCollider(const ColliderComponent* _collider)
{
    const auto cellId = GetCellId(_collider);

    // Only remove the collider if it present at the cell id
    const auto begin = m_colliderMap[cellId].begin();
    const auto end = m_colliderMap[cellId].end();

    const auto itr = std::find(begin, end, _collider);

    if (itr != end) // collider is registered
    {
        m_colliderMap[cellId].erase(itr);
        m_colliderCount--;
    }
}

void PhysicsSolver::Init()
{
    constexpr int targetTickRate = 120;
    constexpr float targetDelta = 1.f / static_cast<float>(targetTickRate);

    while (Renderer::GetInstance()->IsRunning())
    {
        auto frameStart = steady_clock::now();
        
        // force wait until end of 'frame', i.e. 1 / tickRate
        float delta;

        do
        {
            delta = duration<float>(steady_clock::now() - frameStart).count();
        }
        while (delta < targetDelta);

        for (const auto cell : m_colliderMap | std::views::keys)
        {
            UpdatePhysicsBodies(delta, cell);
            CheckForCollisions(cell);
        }

        // const float finalDelta = duration<float>(steady_clock::now() - frameStart).count();
        // std::cout << "PHYSICS => frame time " << finalDelta << "s \t| \tFPS " << 1.f / finalDelta << std::endl;
    }
}

void PhysicsSolver::UpdatePhysicsBodies(const float _deltaSeconds, const std::array<int, 3> _cellId)
{
    for (int i = static_cast<int>(m_colliderMap[_cellId].size()) - 1; i >= 0; i--)
    {
        auto collider = m_colliderMap[_cellId][i];

        collider->UpdateCollider(_deltaSeconds);
        
        auto newCellId = GetCellId(collider);

        // Cell id after move does not match cell id before move
        if (newCellId != _cellId)
        {
            // Remove from previous cell
            auto itr = std::ranges::find(m_colliderMap[_cellId], collider);
            if (itr != m_colliderMap[_cellId].end())
            {
                m_colliderMap[_cellId].erase(itr);
                i--;
            }

            // Add to new cell
            m_colliderMap[newCellId].push_back(collider);
        }
    }
}

void PhysicsSolver::CheckForCollisions(const std::array<int, 3> _cellId)
{
    for (const auto collider1 : m_colliderMap[_cellId])
    {
        if (!collider1->m_kinematic) // if collider is static, don't check collision against other colliders
        {
            for (const auto collider2 : m_colliderMap[_cellId])
            {
                if (collider1 != collider2) // don't check collision against itself
                {
                    collider1->CheckForCollision(collider2);
                }
            }
        }

        // After checking collisions with other objects, clear collision list
        collider1->ClearPreviousCollisions();
    }
}

std::array<int, 3> PhysicsSolver::GetCellId(const ColliderComponent* _collider) const
{
    const glm::vec3 worldPos = _collider->transform.GetWorldPosition();
    
    std::array<int, 3> cellId;
    cellId[0] = static_cast<int>(worldPos.x) / m_cellSize[0];
    cellId[1] = static_cast<int>(worldPos.y) / m_cellSize[1];
    cellId[2] = static_cast<int>(worldPos.z) / m_cellSize[2];

    return cellId;
}
