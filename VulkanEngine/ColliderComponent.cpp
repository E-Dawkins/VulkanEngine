#include "pch.h"
#include "ColliderComponent.h"

#include "PhysicsSolver.h"

ColliderComponent::ColliderComponent()
{
    PhysicsSolver::GetInstance()->RegisterCollider(this);
}

void ColliderComponent::ResolveCollision(ColliderComponent* _otherCollider)
{
    // Determine which collision function to use, based on the other colliders' type
    switch (_otherCollider->m_type)
    {
        case CT_SPHERE: SphereCollision(_otherCollider); break;

        case CT_UNKNOWN: std::cout << "Unknown collider type!" << std::endl;
    }
}

void ColliderComponent::SphereCollision(ColliderComponent* _otherCollider)
{
    std::cout << m_ctStrings[CT_SPHERE] << " collision not implemented for " << m_ctStrings[m_type] << std::endl;
}
