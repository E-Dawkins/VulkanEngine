#include "pch.h"

#include "GravityGameObject.h"
#include "Material_Base.h"
#include "Material_Unlit.h"
#include "Renderer.h"
#include "PhysicsSolver.h"
#include "TestGameObject.h"

#include <glm/gtx/rotate_vector.hpp>

int main() 
{
    Renderer::GetInstance()->Initialize(800, 600);

    // Load Meshes
    Renderer::GetInstance()->LoadMesh("models/viking_room.obj", "house", true);
    Renderer::GetInstance()->LoadMesh("models/sphere.obj", "sphere", true);
    Renderer::GetInstance()->LoadMesh("models/cube.obj", "cube", true);

    // Load Textures
    Renderer::GetInstance()->LoadTexture("textures/viking_room.png", "house");

    // Load Materials
    const auto houseMat = Renderer::GetInstance()->LoadMaterial<Material_Unlit>("shaders/unlit.vert.spv", "shaders/unlit.frag.spv", "house");
    houseMat->SetTexture(Renderer::GetInstance()->GetTexture("house"));
    houseMat->Init();

    const auto colliderMat = Renderer::GetInstance()->LoadMaterial("shaders/color.vert.spv", "shaders/color.frag.spv", "collider");
    colliderMat->pushConstants.color = {0, 1, 0, 1};
    colliderMat->SetFillMode(VK_POLYGON_MODE_LINE);
    colliderMat->Init();
    
    // Initialize Game Objects
    std::vector<GameObject*> gameObjects;
    
    gameObjects.push_back(new GravityGameObject());
    gameObjects.back()->GetRoot()->transform.SetWorldPosition(glm::vec3(0, 1.2f, 3.f));
    gameObjects.back()->GetRoot()->transform.SetWorldScale(glm::vec3(0.5f));
    gameObjects.back()->GetRoot()->transform.SetWorldRotation(rotate(glm::quat(1, 0, 0, 0), glm::radians(65.f), g_rightVector));
    
    gameObjects.push_back(new TestGameObject());
    gameObjects.back()->GetRoot()->transform.SetWorldScale(glm::vec3(0.5f, 1.f, 0.5f));
    
    for (const auto object : gameObjects)
    {
        object->BeginPlay();
    }
    
    auto lastTime = high_resolution_clock::now();

    // Start the physics thread
    std::thread physicsThread([]
    {
        PhysicsSolver::GetInstance()->Init();
    });
        
    while (Renderer::GetInstance()->IsRunning())
    {
        try
        {
            const auto currentTime = high_resolution_clock::now();
            const float deltaTime = duration<float>(currentTime - lastTime).count();

            g_timeSinceAppStart += deltaTime;
                
            // Game object ticks happen first
            for (const auto object : gameObjects)
            {
                object->Tick(deltaTime);
            }

            // Drawing will happen at the end of the frame
            glfwPollEvents();
            Renderer::GetInstance()->DrawFrame();
            
            lastTime = currentTime;
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    // Wait for physics thread to finish
    physicsThread.join();

    // Wait for renderer to finish with this frame
    vkDeviceWaitIdle(Renderer::GetInstance()->GetDevice());

    // Cleanup all game objects (including materials and their graphics resources)
    for (const auto object : gameObjects)
    {
        object->EndPlay();
    }

    // Finally cleanup the renderer
    Renderer::GetInstance()->Cleanup();

    return EXIT_SUCCESS;
}
