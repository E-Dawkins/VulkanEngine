#include "pch.h"
#include "TestGameObject.h"
#include "Material_Unlit.h"
#include "MeshComponent.h"
#include "ObjectComponent.h"
#include "Renderer.h"
#include "SphereColliderComponent.h"

void TestGameObject::BeginPlay()
{
    GameObject::BeginPlay();
    
    // Initialize sphere collider
    m_sphereColl = ObjectComponent::CreateComponent<SphereColliderComponent>("Test");
    m_rootComponent = m_sphereColl;
    
    m_sphereColl->SetRadius(0.35f);
    m_sphereColl->SetKinematic(true);
    m_sphereColl->transform.SetWorldScale(glm::vec3(0.5f));
    
    // Initialize mesh component
    m_meshComponent = ObjectComponent::CreateComponent<MeshComponent>("Mesh");
    m_meshComponent->AttachTo(m_rootComponent);

    auto* material = new Material_Unlit("shaders/unlit.vert.spv", "shaders/unlit.frag.spv", Renderer::GetInstance()->GetRenderPass());
    material->SetTexture(new Texture("textures/viking_room.png"));
    material->Init();
    
    m_meshComponent->GetMesh()->InitMesh("models/viking_room.obj", true);
    m_meshComponent->GetMesh()->material = material;
}

void TestGameObject::Tick(const float _deltaSeconds)
{
    GameObject::Tick(_deltaSeconds);

    
}
