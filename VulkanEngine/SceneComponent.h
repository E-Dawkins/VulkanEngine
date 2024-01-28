#pragma once
#include "ObjectComponent.h"
#include "Transform.h"

class SceneComponent : public ObjectComponent
{
public:
    void AttachTo(ObjectComponent* _parentComp) override;
    void AttachTo(SceneComponent* _parentComp);
    
    void TickComponent(float _deltaSeconds) override;

public:
    Transform transform = Transform();
};
