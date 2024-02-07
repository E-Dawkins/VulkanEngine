#include "pch.h"
#include "GravityGameObject.h"

#include "MeshComponent.h"
#include "SphereColliderComponent.h"

GravityGameObject::GravityGameObject()
{
    // Initialize sphere collider
    m_sphereColl = ObjectComponent::CreateComponent<SphereColliderComponent>("Gravity");
    m_sphereColl->SetUseGravity(true);
    SetRoot(m_sphereColl);
    
    // Initialize mesh component
    m_meshComponent = ObjectComponent::CreateComponent<MeshComponent>("Mesh");
    m_meshComponent->AttachTo(m_sphereColl);

    m_meshComponent->SetMesh(Renderer::GetInstance()->GetMesh("sphere"));
    m_meshComponent->SetMaterial(Renderer::GetInstance()->GetMaterial("house"));
}

void GravityGameObject::BeginPlay()
{
    GameObject::BeginPlay();
    
    
}

void GravityGameObject::Tick(const float _deltaSeconds)
{
    GameObject::Tick(_deltaSeconds);

    
}
