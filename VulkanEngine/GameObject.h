#pragma once
#include "SceneComponent.h"

class GameObject
{
public:
    GameObject()
    {
        m_rootComponent = new SceneComponent();
    }

    virtual ~GameObject()
    {
        delete m_rootComponent;
    }
    
    virtual void BeginPlay() {}
    virtual void Tick(const float _deltaSeconds)
    {
        m_rootComponent->TickComponent(_deltaSeconds);
    }
    virtual void EndPlay() {}

    /* Getters */
    SceneComponent* GetRoot() const { return m_rootComponent; }

protected:
    SceneComponent* m_rootComponent = nullptr;
};
