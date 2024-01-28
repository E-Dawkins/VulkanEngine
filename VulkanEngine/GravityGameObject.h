#pragma once
#include "GameObject.h"

class SphereColliderComponent;
class MeshComponent;

class GravityGameObject : public GameObject
{
public:
    void BeginPlay() override;
    void Tick(float _deltaSeconds) override;

private:
    MeshComponent* m_meshComp = nullptr;
    SphereColliderComponent* m_sphereColl = nullptr;
};
