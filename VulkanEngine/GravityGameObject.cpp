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
    m_rootComponent = m_sphereColl;
    
    m_sphereColl->SetRadius(0.35f);
    m_sphereColl->SetUseGravity(true);
    m_sphereColl->transform.m_worldScale = glm::vec3(0.5f);
    m_sphereColl->transform.m_worldPosition = glm::vec3(0, 0.1f, 2.f);
    
    // Initialize mesh component
    m_meshComp = ObjectComponent::CreateComponent<MeshComponent>("Mesh");
    m_meshComp->AttachTo(m_sphereColl);

    auto* material = new Material_Unlit("shaders/unlit.vert.spv", "shaders/unlit.frag.spv", Renderer::GetInstance()->GetRenderPass());
    material->SetTexture(new Texture("textures/viking_room.png"));
    material->Init();
    
    m_meshComp->GetMesh()->InitMesh("models/viking_room.obj", true);
    m_meshComp->GetMesh()->material = material;
}

void GravityGameObject::Tick(const float _deltaSeconds)
{
    GameObject::Tick(_deltaSeconds);

    
}
