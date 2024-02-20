#include "pch.h"
#include "TestGameObject.h"

#include "RigidbodyComponent.h"
#include "MeshComponent.h"
#include "Renderer.h"

TestGameObject::TestGameObject()
{
    // Initialize sphere collider
    // m_coll = ObjectComponent::CreateComponent<SphereColliderComponent>("Test");
    m_rb = ObjectComponent::CreateComponent<RigidbodyComponent>("Test");
    m_rb->SetIsKinematic(true);
    m_rb->SetType(CT_SPHERE);
    SetRoot(m_rb);
    
    // Initialize mesh component
    m_meshComponent = ObjectComponent::CreateComponent<MeshComponent>("Mesh");
    m_meshComponent->AttachTo(GetRoot());

    m_meshComponent->SetMesh(Renderer::GetInstance()->GetMesh("sphere"));
    m_meshComponent->SetMaterial(Renderer::GetInstance()->GetMaterial("house"));
}

void TestGameObject::BeginPlay()
{
    GameObject::BeginPlay();

    
}

void TestGameObject::Tick(const float _deltaSeconds)
{
    GameObject::Tick(_deltaSeconds);

    
}
