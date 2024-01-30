#include "pch.h"
#include "PhysicsSolver.h"

void PhysicsSolver::RegisterCollider(ColliderComponent* _collider)
{
    const auto begin = m_colliders.begin();
    const auto end = m_colliders.end();

    const auto itr = std::find(begin, end, _collider);
    
    if (itr == end) // collider is not already registered
    {
        m_colliders.push_back(_collider);
    }
}

void PhysicsSolver::UnRegisterCollider(const ColliderComponent* _collider)
{
    const auto begin = m_colliders.begin();
    const auto end = m_colliders.end();

    const auto itr = std::find(begin, end, _collider);

    if (itr != end) // collider is registered
    {
        m_colliders.erase(itr);
    }
}

void PhysicsSolver::Init()
{
    constexpr int tickRate = 120;
    constexpr float targetDelta = 1.f / static_cast<float>(tickRate);

    while (Renderer::GetInstance()->IsRunning())
    {
        auto frameStart = steady_clock::now();
        
        // force wait until end of 'frame', i.e. 1 / tickRate
        auto workTime = steady_clock::now() - frameStart;

        while (duration<float>(workTime).count() < targetDelta)
        {
            workTime = steady_clock::now() - frameStart;
        }

        const float actualDelta = duration<float>(workTime).count();
        
        UpdatePhysicsBodies(actualDelta);
        CheckForCollisions();
        
        // std::cout << actualDelta << "ms | FPS " << 1.f / actualDelta << std::endl;
    }
}

void PhysicsSolver::UpdatePhysicsBodies(const float _deltaSeconds) const
{
    for (const auto collider : m_colliders)
    {
        if (collider->m_kinematic)
        {
            continue;
        }
    
        if (collider->m_useGravity)
        {
            collider->m_velocity += g_gravity * _deltaSeconds;
        }

        collider->transform.SetWorldPosition(collider->transform.GetWorldPosition() + (collider->m_velocity * _deltaSeconds));
    }
}

void PhysicsSolver::CheckForCollisions() const
{
    for (const auto collider1 : m_colliders)
    {
        for (const auto collider2 : m_colliders)
        {
            if (collider1 != collider2) // don't check collision against itself
            {
                collider1->ResolveCollision(collider2);
            }
        }

        // After checking collisions with other objects, clear collision list
        collider1->ClearPreviousCollisions();
    }
}
