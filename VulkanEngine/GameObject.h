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
    
    virtual void BeginPlay()
    {
        m_rootComponent->BeginComponent();
    }
    virtual void Tick(const float _deltaSeconds)
    {
        m_rootComponent->TickComponent(_deltaSeconds);
    }
    virtual void EndPlay()
    {
        m_rootComponent->CleanupComponent();
    }

    /* Getters */
    SceneComponent*& GetRoot() { return m_rootComponent; }

    /* Setters */
    void SetRoot(SceneComponent* _newRoot)
    {
        // Copy the old transform across to the new root
        const Transform t = m_rootComponent->transform;
        m_rootComponent = _newRoot;
        m_rootComponent->transform = t;
    }

private:
    SceneComponent* m_rootComponent = nullptr;
};
