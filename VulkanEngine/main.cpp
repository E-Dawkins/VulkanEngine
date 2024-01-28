#include "pch.h"

#include "GravityGameObject.h"
#include "Renderer.h"
#include "PhysicsSolver.h"
#include "TestGameObject.h"

int main() 
{
    Renderer::GetInstance()->Initialize(800, 600);
    
    auto* testObject = new TestGameObject();
    auto* gravityObject = new GravityGameObject();
    
    testObject->BeginPlay();
    gravityObject->BeginPlay();
    
    auto mainThreadFunc = [&]()
    {
        auto lastTime = high_resolution_clock::now();
        
        while (Renderer::GetInstance()->IsRunning())
        {
            try
            {
                const auto currentTime = high_resolution_clock::now();
                const float deltaTime = duration<float>(currentTime - lastTime).count();

                // TODO - remove rudimentary fps cap
                constexpr int fps = 60;
                if (deltaTime < 1.f / static_cast<float>(fps))
                {
                    continue;
                }

                g_timeSinceAppStart += deltaTime;
                
                // Game object ticks happen first
                testObject->Tick(deltaTime);
                gravityObject->Tick(deltaTime);

                // Then we update physics
                PhysicsSolver::GetInstance()->CheckForCollisions();

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

        vkDeviceWaitIdle(Renderer::GetInstance()->GetDevice());
        
        Renderer::GetInstance()->Cleanup();
    };

    // Initialize threads
    std::thread mainThread(mainThreadFunc);

    // Wait for threads to finish
    mainThread.join();

    return EXIT_SUCCESS;
}
