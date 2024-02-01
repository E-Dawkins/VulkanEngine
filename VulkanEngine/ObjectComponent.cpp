#include "pch.h"
#include "ObjectComponent.h"

ObjectComponent::~ObjectComponent()
{
    for (const auto attachedComp : m_attachedComponents)
    {
        delete attachedComp;
    }
}

void ObjectComponent::AttachTo(ObjectComponent* _parentComp)
{
    _parentComp->m_attachedComponents.push_back(this);
}

void ObjectComponent::RemoveFrom(ObjectComponent* _parentComp) const
{
    const auto begin = _parentComp->m_attachedComponents.begin();
    const auto end = _parentComp->m_attachedComponents.end();

    const auto itr = std::find(begin, end, this);
    
    if(itr != end) // if the component exists in the parent attached components, erase it
    {
        _parentComp->m_attachedComponents.erase(itr);
    }
}

void ObjectComponent::BeginComponent()
{
    // Begin all attached components
    for (ObjectComponent* comp : m_attachedComponents)
    {
        comp->BeginComponent();
    }
}

void ObjectComponent::TickComponent(const float _deltaSeconds)
{
    // Tick all attached components
    for (ObjectComponent* comp : m_attachedComponents)
    {
        comp->TickComponent(_deltaSeconds);
    }
}

void ObjectComponent::CleanupComponent()
{
    // Cleanup all attached components
    for (ObjectComponent* comp : m_attachedComponents)
    {
        comp->CleanupComponent();
    }
}
