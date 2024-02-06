#include "pch.h"
#include "GravityGameObject.h"

#include "MeshComponent.h"
#include "SphereColliderComponent.h"

GravityGameObject::GravityGameObject()
{
    // Initialize sphere collider
    m_sphereColl = ObjectComponent::CreateComponent<SphereColliderComponent>("Gravity");
    SetRoot(m_sphereColl);
    
    m_sphereColl->transform.SetWorldScale(glm::vec3(0.5f, 0.25f, 0.25f));
    m_sphereColl->SetUseGravity(true);
    
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
