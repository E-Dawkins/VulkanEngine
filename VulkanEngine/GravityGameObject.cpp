#include "pch.h"
#include "GravityGameObject.h"

#include "CubeColliderComponent.h"
#include "MeshComponent.h"
#include "SphereColliderComponent.h"

GravityGameObject::GravityGameObject()
{
    // Initialize sphere collider
    // m_coll = ObjectComponent::CreateComponent<SphereColliderComponent>("Gravity");
    m_coll = ObjectComponent::CreateComponent<CubeColliderComponent>("Gravity");
    m_coll->SetUseGravity(true);
    SetRoot(m_coll);
    
    // Initialize mesh component
    m_meshComponent = ObjectComponent::CreateComponent<MeshComponent>("Mesh");
    m_meshComponent->AttachTo(m_coll);

    m_meshComponent->SetMesh(Renderer::GetInstance()->GetMesh("cube"));
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
