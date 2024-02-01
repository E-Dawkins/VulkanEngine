#pragma once
#include "GameObject.h"

class SphereColliderComponent;
class MeshComponent;

class TestGameObject : public GameObject
{
public:
    TestGameObject();
    void BeginPlay() override;
    void Tick(float _deltaSeconds) override;

private:
    MeshComponent* m_meshComponent = nullptr;
    SphereColliderComponent* m_sphereColl = nullptr;
};
