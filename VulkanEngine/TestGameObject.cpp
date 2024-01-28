#include "pch.h"
#include "TestGameObject.h"
#include "Material_Unlit.h"
#include "MeshComponent.h"
#include "ObjectComponent.h"
#include "Renderer.h"

void TestGameObject::BeginPlay()
{
    GameObject::BeginPlay();

    auto* material = new Material_Unlit("shaders/unlit.vert.spv", "shaders/unlit.frag.spv", Renderer::GetInstance()->GetRenderPass());
    material->SetTexture(new Texture("textures/viking_room.png"));
    
    material->Init();
    
    m_meshComponent = ObjectComponent::CreateComponent<MeshComponent>("Mesh");
    m_meshComponent->AttachTo(m_rootComponent);
    
    m_meshComponent->GetMesh() = new Mesh("models/viking_room.obj", true);
    m_meshComponent->GetMesh()->material = material;

    m_rootComponent->transform.m_worldScale = glm::vec3(0.5f);
}

void TestGameObject::Tick(const float _deltaSeconds)
{
    GameObject::Tick(_deltaSeconds);

    static float accumulatedTime = 0.f;
    accumulatedTime += _deltaSeconds;
    
    // Rotation
    auto rotation = m_rootComponent->transform.m_worldRotation;
    rotation = rotate(rotation, _deltaSeconds * glm::radians(90.f), glm::vec3(0, 0, 1));
    m_rootComponent->transform.m_worldRotation = rotation;

    // Position
    const float zPos = cosf(accumulatedTime) * 0.5f;
    m_rootComponent->transform.m_worldPosition.z = zPos;
}
