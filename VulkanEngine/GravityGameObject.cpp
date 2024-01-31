#include "pch.h"
#include "GravityGameObject.h"

#include "Material_Unlit.h"
#include "MeshComponent.h"
#include "SphereColliderComponent.h"

void GravityGameObject::BeginPlay()
{
    GameObject::BeginPlay();
    
    // Initialize sphere collider
    m_sphereColl = ObjectComponent::CreateComponent<SphereColliderComponent>("Gravity");
    SetRoot(m_sphereColl);
    
    m_sphereColl->SetRadius(0.35f);
    m_sphereColl->SetUseGravity(true);
    m_sphereColl->transform.SetWorldScale(glm::vec3(0.5f));
    
    // Initialize mesh component
    m_meshComponent = ObjectComponent::CreateComponent<MeshComponent>("Mesh");
    m_meshComponent->AttachTo(m_sphereColl);

    m_meshComponent->SetMesh(Renderer::GetInstance()->GetMesh("house"));

    const auto material = new Material_Unlit("shaders/unlit.vert.spv", "shaders/unlit.frag.spv", Renderer::GetInstance()->GetRenderPass());
    material->SetTexture(Renderer::GetInstance()->GetTexture("house"));
    material->Init();
    
    m_meshComponent->SetMaterial(material);
}

void GravityGameObject::Tick(const float _deltaSeconds)
{
    GameObject::Tick(_deltaSeconds);

    
}
