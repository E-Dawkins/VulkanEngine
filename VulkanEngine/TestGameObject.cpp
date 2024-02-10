#include "pch.h"
#include "TestGameObject.h"

#include "CubeColliderComponent.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "SphereColliderComponent.h"

TestGameObject::TestGameObject()
{
    // Initialize sphere collider
    // m_coll = ObjectComponent::CreateComponent<SphereColliderComponent>("Test");
    m_coll = ObjectComponent::CreateComponent<CubeColliderComponent>("Test");
    m_coll->SetKinematic(true);
    SetRoot(m_coll);
    
    // Initialize mesh component
    m_meshComponent = ObjectComponent::CreateComponent<MeshComponent>("Mesh");
    m_meshComponent->AttachTo(GetRoot());

    m_meshComponent->SetMesh(Renderer::GetInstance()->GetMesh("cube"));
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
