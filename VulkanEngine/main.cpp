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
    
    auto* testObject = new TestGameObject();
    auto* testObject2 = new TestGameObject();
    auto* gravityObject = new GravityGameObject();
    
    testObject->BeginPlay();
    testObject2->BeginPlay();
    testObject2->GetRoot()->transform.SetWorldPosition(glm::vec3(0, 1.65f, 0));
    gravityObject->BeginPlay();
    
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

            // TODO - remove rudimentary fps cap
            // constexpr int fps = 60;
            // if (deltaTime < 1.f / static_cast<float>(fps))
            // {
            //     continue;
            // }

            g_timeSinceAppStart += deltaTime;
                
            // Game object ticks happen first
            testObject->Tick(deltaTime);
            testObject2->Tick(deltaTime);
            gravityObject->Tick(deltaTime);

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
    
    vkDeviceWaitIdle(Renderer::GetInstance()->GetDevice());
        
    Renderer::GetInstance()->Cleanup();

    return EXIT_SUCCESS;
}
