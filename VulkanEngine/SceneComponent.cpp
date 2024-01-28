#include "pch.h"
#include "SceneComponent.h"

void SceneComponent::AttachTo(ObjectComponent* _parentComp)
{
    ObjectComponent::AttachTo(_parentComp);

    transform.SetParent(nullptr);
}

void SceneComponent::AttachTo(SceneComponent* _parentComp)
{
    ObjectComponent::AttachTo(_parentComp);
    
    transform.SetParent(&_parentComp->transform);
}

void SceneComponent::TickComponent(const float _deltaSeconds)
{
    transform.UpdateTransform();
    
    ObjectComponent::TickComponent(_deltaSeconds);
}
