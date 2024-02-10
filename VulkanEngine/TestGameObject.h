#pragma once
#include "GameObject.h"

class ColliderComponent;
class MeshComponent;

class TestGameObject : public GameObject
{
public:
    TestGameObject();
    void BeginPlay() override;
    void Tick(float _deltaSeconds) override;

private:
    MeshComponent* m_meshComponent = nullptr;
    ColliderComponent* m_coll = nullptr;
};
