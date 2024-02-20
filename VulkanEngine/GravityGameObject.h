#pragma once
#include "GameObject.h"

class RigidbodyComponent;
class MeshComponent;

class GravityGameObject : public GameObject
{
public:
    GravityGameObject();
    
    void BeginPlay() override;
    void Tick(float _deltaSeconds) override;

private:
    MeshComponent* m_meshComponent = nullptr;
    RigidbodyComponent* m_rb = nullptr;
};
