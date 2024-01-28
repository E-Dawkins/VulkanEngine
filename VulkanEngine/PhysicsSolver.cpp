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
    }
}
