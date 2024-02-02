#include "pch.h"
#include "ColliderComponent.h"

#include "Material_Base.h"
#include "Mesh.h"
#include "PhysicsSolver.h"

void ColliderComponent::BeginComponent()
{
    SceneComponent::BeginComponent();

    PhysicsSolver::GetInstance()->RegisterCollider(this);

    // Set-up visualized mesh if it has been set
    if (m_visualizedMesh)
    {
        Renderer::GetInstance()->AddDrawCall([&] (const VkCommandBuffer _buffer)
        {
            if (g_visualizeColliders)
            {
                const auto material = Renderer::GetInstance()->GetMaterial("collider");
                material->pushConstants.model = transform.GetWorldMatrix();
                material->UpdateMaterial();
                material->RenderMaterial(_buffer);
                m_visualizedMesh->DrawMesh(_buffer);
            }
        });
    }
}

void ColliderComponent::ResolveCollision(ColliderComponent* _otherCollider)
{
    // If we have already collided with the other collider, skip calculations
    if (std::ranges::find(m_previousCollisions, _otherCollider) != m_previousCollisions.end())
    {
        return;
    }
    
    float collisionVelocity = 0.f;
    glm::vec3 collisionPoint;
    bool didCollide = false;
    
    // Determine which collision function to use, based on the other colliders' type
    switch (_otherCollider->m_type)
    {
        case CT_SPHERE: didCollide = SphereCollision(_otherCollider, collisionVelocity, collisionPoint); break;

        case CT_UNKNOWN: std::cout << "Unknown collider type!" << std::endl;
    }

    // If the colliders did collide, then calculate their individual output velocity
    if (didCollide)
    {
        const glm::vec3 collisionDir = normalize(transform.GetWorldPosition() - collisionPoint);
        const glm::vec3 finalVelocity = collisionDir * collisionVelocity;
        
        // If either collider is kinematic, then set the non-kinematic colliders' final velocity to the net velocity
        if (_otherCollider->m_kinematic)
        {
            m_velocity = finalVelocity;
        }
        else if (m_kinematic)
        {
            _otherCollider->m_velocity = -finalVelocity;
        }
        else // if neither collider is kinematic, just run normal velocity calculations
        {
            m_velocity += finalVelocity;
            _otherCollider->m_velocity += -finalVelocity;
        }

        // Add this collider to list of resolved colliders, to avoid re-running calculations
        _otherCollider->m_previousCollisions.push_back(this);
    }
}

bool ColliderComponent::SphereCollision(ColliderComponent* _otherCollider, float& _collisionVelocity, glm::vec3& _collisionPoint)
{
    std::cout << m_ctStrings[CT_SPHERE] << " collision not implemented for " << m_ctStrings[m_type] << std::endl;

    return false;
}
