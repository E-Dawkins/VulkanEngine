#pragma once
#include "GameObject.h"

class RigidbodyComponent;
class MeshComponent;

class TestGameObject : public GameObject
{
public:
    TestGameObject();
    void BeginPlay() override;
    void Tick(float _deltaSeconds) override;

private:
    MeshComponent* m_meshComponent = nullptr;
    RigidbodyComponent* m_rb = nullptr;
};
