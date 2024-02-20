#include "pch.h"
#include "PhysicsSolver.h"

#include <ranges>

void PhysicsSolver::RegisterObject(RigidbodyComponent* _rb)
{
    // Get cell index for the new collider
    const auto cellId = GetCellId(_rb);
    
    // Check if collider is already registered
    const auto begin = m_objectMap[cellId].begin();
    const auto end = m_objectMap[cellId].end();

    const auto itr = std::find(begin, end, _rb);
    
    if (itr == end) // collider is not already registered
    {
        m_objectMap[cellId].push_back(_rb);
        m_colliderCount++;
    }
}

void PhysicsSolver::UnRegisterObject(const RigidbodyComponent* _rb)
{
    const auto cellId = GetCellId(_rb);

    // Only remove the collider if it present at the cell id
    const auto begin = m_objectMap[cellId].begin();
    const auto end = m_objectMap[cellId].end();

    const auto itr = std::find(begin, end, _rb);

    if (itr != end) // collider is registered
    {
        m_objectMap[cellId].erase(itr);
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

        for (const auto cell : m_objectMap | std::views::keys)
        {
            UpdateCellId(cell);
            CheckForCollisions(cell);
        }

        // const float finalDelta = duration<float>(steady_clock::now() - frameStart).count();
        // std::cout << "PHYSICS => frame time " << finalDelta << "s \t| \tFPS " << 1.f / finalDelta << std::endl;
    }
}

void PhysicsSolver::UpdateCellId(const std::array<int, 3> _cellId)
{
    for (int i = static_cast<int>(m_objectMap[_cellId].size()) - 1; i >= 0; i--)
    {
        RigidbodyComponent* rb = m_objectMap[_cellId][i];
        auto newCellId = GetCellId(rb);

        // Cell id this frame does not match last frame
        if (newCellId != _cellId)
        {
            // Remove from previous cell
            auto itr = std::ranges::find(m_objectMap[_cellId], rb);
            if (itr != m_objectMap[_cellId].end())
            {
                m_objectMap[_cellId].erase(itr);
                i--;
            }

            // Add to new cell
            m_objectMap[newCellId].push_back(rb);
        }
    }
}

void PhysicsSolver::CheckForCollisions(const std::array<int, 3> _cellId)
{
    for (const auto rb1 : m_objectMap[_cellId])
    {
        if (!rb1->IsKinematic()) // if collider is static, don't check collision against other colliders
        {
            for (const auto rb2 : m_objectMap[_cellId])
            {
                if (rb1 != rb2) // don't check collision against itself
                {
                    rb1->CheckCollision(rb2);
                }
            }
        }
    }
}

std::array<int, 3> PhysicsSolver::GetCellId(const RigidbodyComponent* _rb) const
{
    const glm::vec3 worldPos = _rb->transform.GetWorldPosition();
    
    std::array<int, 3> cellId;
    cellId[0] = static_cast<int>(worldPos.x) / m_cellSize[0];
    cellId[1] = static_cast<int>(worldPos.y) / m_cellSize[1];
    cellId[2] = static_cast<int>(worldPos.z) / m_cellSize[2];

    return cellId;
}
