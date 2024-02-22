#include "pch.h"
#include "GravityGameObject.h"

#include "RigidbodyComponent.h"
#include "MeshComponent.h"

GravityGameObject::GravityGameObject()
{
    // Initialize sphere collider
    // m_coll = ObjectComponent::CreateComponent<SphereColliderComponent>("Gravity");
    m_rb = ObjectComponent::CreateComponent<RigidbodyComponent>("Gravity");
    // m_coll->SetVelocity(glm::vec3(0, 0, -1));
    m_rb->SetUsingGravity(true);
    m_rb->SetType(CT_CUBE);
    SetRoot(m_rb);
    
    // Initialize mesh component
    m_meshComponent = ObjectComponent::CreateComponent<MeshComponent>("Mesh");
    m_meshComponent->AttachTo(m_rb);

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
