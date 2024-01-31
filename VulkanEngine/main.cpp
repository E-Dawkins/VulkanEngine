#include "pch.h"

#include "GravityGameObject.h"
#include "Renderer.h"
#include "PhysicsSolver.h"
#include "TestGameObject.h"

int main() 
{
    Renderer::GetInstance()->Initialize(800, 600);

    // Load Meshes
    Renderer::GetInstance()->LoadMesh("models/viking_room.obj", "house", true);

    const std::array<GameObject*, 3> gameObjects =
    {
        new TestGameObject(),
        new TestGameObject(),
        new GravityGameObject()
    };

    for (const auto object : gameObjects)
    {
        object->BeginPlay();
    }

    // TODO - remove explicit position setting
    gameObjects[1]->GetRoot()->transform.SetWorldPosition(glm::vec3(0, 1.65f, 0));
    
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
