#pragma once
#include "GameObject.h"

class MeshComponent;

class TestGameObject : public GameObject
{
public:
    void BeginPlay() override;
    void Tick(float _deltaSeconds) override;

private:
    MeshComponent* m_meshComponent = nullptr;
};
