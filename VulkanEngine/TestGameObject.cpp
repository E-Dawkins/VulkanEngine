#include "pch.h"
#include "TestGameObject.h"

#include "MeshComponent.h"
#include "Renderer.h"
#include "SphereColliderComponent.h"

TestGameObject::TestGameObject()
{
    // Initialize sphere collider
    m_sphereColl = ObjectComponent::CreateComponent<SphereColliderComponent>("Test");
    SetRoot(m_sphereColl);
    
    m_sphereColl->SetRadius(0.5f);
    m_sphereColl->SetMass(10.f);
    // m_sphereColl->SetKinematic(true);
    
    // Initialize mesh component
    m_meshComponent = ObjectComponent::CreateComponent<MeshComponent>("Mesh");
    m_meshComponent->AttachTo(GetRoot());

    m_meshComponent->SetMesh(Renderer::GetInstance()->GetMesh("house"));
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
